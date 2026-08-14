/**
 * Caligari trueSpace binary (.COB) parser for Three.js
 * Parses PolH chunks and transforms vertices using the 3D current_position matrix.
 * Exact port of Chaotic Order's load_cobFN in src/diff2c.c
 */

export class COBParser {
    /**
     * Parse an ArrayBuffer containing a Caligari .COB binary file
     * @param {ArrayBuffer} buffer
     * @param {boolean} applyPositionMatrix - Whether to apply the trueSpace world position matrix
     * @returns {{ name: string, positions: Float32Array, uvs: Float32Array | null, vertexCount: number, faceCount: number }}
     */
    static parse(buffer, applyPositionMatrix = true) {
        const view = new DataView(buffer);
        const bytes = new Uint8Array(buffer);

        // Verify Caligari header
        const magic = String.fromCharCode(...bytes.slice(0, 9));
        if (magic !== 'Caligari ') {
            throw new Error('Invalid COB file: missing Caligari header');
        }

        let pos = 32; // Skip 32-byte file header
        let objectName = 'COB_Object';
        let rawVertices = [];
        let uvs = [];
        let faces = [];
        let currentPosMatrix = [];

        while (pos + 20 <= buffer.byteLength) {
            const chunkType = String.fromCharCode(...bytes.slice(pos, pos + 4));
            const chunkSize = view.getUint32(pos + 16, true);

            pos += 20; // Move past chunk header
            const chunkEnd = pos + chunkSize;

            if (chunkType === 'END ') {
                break;
            } else if (chunkType === 'PolH') {
                const dupeCount = view.getInt16(pos, true);
                const strLen = view.getInt16(pos + 2, true);
                pos += 4;

                objectName = String.fromCharCode(...bytes.slice(pos, pos + strLen)).replace(/\0/g, '');
                pos += strLen;

                // Skip 4x3 local axes matrix (12 floats)
                pos += 48;

                // Read 4x3 / 4x4 current position matrix (12 floats)
                for (let i = 0; i < 12; i++) {
                    currentPosMatrix.push(view.getFloat32(pos, true));
                    pos += 4;
                }

                // Read vertices
                const numVertices = view.getInt32(pos, true);
                pos += 4;

                for (let i = 0; i < numVertices; i++) {
                    const x = view.getFloat32(pos, true);
                    const y = view.getFloat32(pos + 4, true);
                    const z = view.getFloat32(pos + 8, true);
                    pos += 12;
                    rawVertices.push({ x, y, z });
                }

                // Read UV texture vertices
                const numUvs = view.getInt32(pos, true);
                pos += 4;

                for (let i = 0; i < numUvs; i++) {
                    const u = view.getFloat32(pos, true);
                    const v = view.getFloat32(pos + 4, true);
                    pos += 8;
                    uvs.push(u, v);
                }

                // Read polygon face table
                const numPolygons = view.getInt32(pos, true);
                pos += 4;

                for (let i = 0; i < numPolygons; i++) {
                    const flags = view.getUint8(pos);
                    const numVertsInFace = view.getInt16(pos + 1, true);
                    const materialIndex = view.getInt16(pos + 3, true);
                    pos += 5;

                    const faceVerts = [];
                    const faceUvs = [];

                    for (let vIdx = 0; vIdx < numVertsInFace; vIdx++) {
                        const vI = view.getInt32(pos, true);
                        const uvI = view.getInt32(pos + 4, true);
                        pos += 8;
                        faceVerts.push(vI);
                        faceUvs.push(uvI);
                    }

                    // Triangulate n-gons (fan triangulation)
                    for (let t = 1; t < numVertsInFace - 1; t++) {
                        faces.push({
                            a: faceVerts[0],
                            b: faceVerts[t],
                            c: faceVerts[t + 1],
                            uva: faceUvs[0],
                            uvb: faceUvs[t],
                            uvc: faceUvs[t + 1]
                        });
                    }
                }
            }

            pos = chunkEnd; // Advance to next chunk
        }

        // Apply trueSpace 3D current_position transformation
        const transformedVertices = [];
        const m = currentPosMatrix.length >= 12 ? currentPosMatrix : [
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0
        ];

        for (let i = 0; i < rawVertices.length; i++) {
            const vx = rawVertices[i].x;
            const vy = rawVertices[i].y;
            const vz = rawVertices[i].z;

            if (applyPositionMatrix && currentPosMatrix.length >= 12) {
                const tx = vx * m[0] + vy * m[3] + vz * m[6] + m[9];
                const ty = vx * m[1] + vy * m[4] + vz * m[7] + m[10];
                const tz = vx * m[2] + vy * m[5] + vz * m[8] + m[11];
                transformedVertices.push(tx, ty, tz);
            } else {
                transformedVertices.push(vx, vy, vz);
            }
        }

        // Build position and UV arrays for THREE.BufferGeometry
        const positions = [];
        const texCoords = [];

        for (const face of faces) {
            const ax = transformedVertices[face.a * 3], ay = transformedVertices[face.a * 3 + 1], az = transformedVertices[face.a * 3 + 2];
            const bx = transformedVertices[face.b * 3], by = transformedVertices[face.b * 3 + 1], bz = transformedVertices[face.b * 3 + 2];
            const cx = transformedVertices[face.c * 3], cy = transformedVertices[face.c * 3 + 1], cz = transformedVertices[face.c * 3 + 2];

            positions.push(ax, ay, az, bx, by, bz, cx, cy, cz);

            if (uvs.length > 0) {
                const au = uvs[face.uva * 2] || 0, av = 1.0 - (uvs[face.uva * 2 + 1] || 0);
                const bu = uvs[face.uvb * 2] || 0, bv = 1.0 - (uvs[face.uvb * 2 + 1] || 0);
                const cu = uvs[face.uvc * 2] || 0, cv = 1.0 - (uvs[face.uvc * 2 + 1] || 0);
                texCoords.push(au, av, bu, bv, cu, cv);
            }
        }

        return {
            name: objectName,
            positions: new Float32Array(positions),
            uvs: texCoords.length > 0 ? new Float32Array(texCoords) : null,
            vertexCount: transformedVertices.length / 3,
            faceCount: faces.length
        };
    }
}

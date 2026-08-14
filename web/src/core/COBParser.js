/**
 * Caligari trueSpace binary (.COB) parser for Three.js
 * Parses PolH chunks into THREE.BufferGeometry with normals and UV coordinates.
 * Faithful port of Chaotic Order's 1997 load_cobFN in src/diff2c.c
 */

export class COBParser {
    /**
     * Parse an ArrayBuffer containing a Caligari .COB binary file
     * @param {ArrayBuffer} buffer
     * @returns {{ name: string, positions: Float32Array, uvs: Float32Array | null, vertexCount: number, faceCount: number }}
     */
    static parse(buffer) {
        const view = new DataView(buffer);
        const bytes = new Uint8Array(buffer);

        // Verify Caligari header
        const magic = String.fromCharCode(...bytes.slice(0, 9));
        if (magic !== 'Caligari ') {
            throw new Error('Invalid COB file: missing Caligari header');
        }

        let pos = 32; // Skip 32-byte file header
        let objectName = 'COB_Object';
        let vertices = [];
        let uvs = [];
        let faces = [];
        let localAxes = [];
        let currentPosition = [];

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

                // Read 4x3 local axes matrix (12 floats)
                for (let i = 0; i < 12; i++) {
                    localAxes.push(view.getFloat32(pos, true));
                    pos += 4;
                }

                // Read 3x4 position matrix (12 floats)
                for (let i = 0; i < 12; i++) {
                    currentPosition.push(view.getFloat32(pos, true));
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
                    vertices.push(x, y, z);
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

                    // Triangulate n-gons if needed (fan triangulation)
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

        // Build position and UV arrays for THREE.BufferGeometry
        const positions = [];
        const texCoords = [];

        for (const face of faces) {
            const ax = vertices[face.a * 3], ay = vertices[face.a * 3 + 1], az = vertices[face.a * 3 + 2];
            const bx = vertices[face.b * 3], by = vertices[face.b * 3 + 1], bz = vertices[face.b * 3 + 2];
            const cx = vertices[face.c * 3], cy = vertices[face.c * 3 + 1], cz = vertices[face.c * 3 + 2];

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
            vertexCount: vertices.length / 3,
            faceCount: faces.length
        };
    }
}

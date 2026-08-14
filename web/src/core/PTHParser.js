/**
 * Camera Waypoint Stream (.PTH) binary parser
 * Reads 36-byte records: viewpoint (x,y,z), lookat (x,y,z), light (x,y,z)
 * Faithful port of Chaotic Order's 1997 path format
 */

export class PTHParser {
    /**
     * Parse an ArrayBuffer containing a .PTH binary file
     * @param {ArrayBuffer} buffer
     * @returns {{ numPoints: number, points: Array<{ viewpoint: {x:number, y:number, z:number}, lookat: {x:number, y:number, z:number}, light: {x:number, y:number, z:number} }> }}
     */
    static parse(buffer) {
        const view = new DataView(buffer);
        const numPoints = view.getInt32(0, true);
        const points = [];

        let pos = 4;
        for (let i = 0; i < numPoints; i++) {
            // Viewpoint (16.16 fixed point / 65536)
            const vx = view.getInt32(pos, true) / 65536.0;
            const vy = view.getInt32(pos + 4, true) / 65536.0;
            const vz = view.getInt32(pos + 8, true) / 65536.0;

            // Look-at target
            const lx = view.getInt32(pos + 12, true) / 65536.0;
            const ly = view.getInt32(pos + 16, true) / 65536.0;
            const lz = view.getInt32(pos + 20, true) / 65536.0;

            // Light vector
            const ix = view.getInt32(pos + 24, true) / 65536.0;
            const iy = view.getInt32(pos + 28, true) / 65536.0;
            const iz = view.getInt32(pos + 32, true) / 65536.0;

            pos += 36;
            points.push({
                viewpoint: { x: vx, y: vy, z: vz },
                lookat: { x: lx, y: ly, z: lz },
                light: { x: ix, y: iy, z: iz }
            });
        }

        return { numPoints, points };
    }
}

import { COBParser } from './COBParser.js';

/**
 * World Loader (.WLD)
 * Parses Chaotic Order's world scene scripts and loads all genuine .COB models.
 */

export class WorldLoader {
    constructor(app) {
        this.app = app;
        this.THREE = app.THREE;
    }

    /**
     * Load a world definition and return a THREE.Group with all models
     * @param {string} wldPath - e.g. 'assets/worlds/COINTRO.WLD'
     */
    async loadWorld(wldPath) {
        const THREE = this.THREE;
        const group = new THREE.Group();

        try {
            const resp = await fetch(wldPath);
            const text = await resp.text();
            const tokens = text.split(/\s+/).filter(t => t.length > 0);

            if (tokens.length < 4) return group;

            const runtime = parseInt(tokens[0]);
            const maxfps = parseInt(tokens[1]);
            const numObjects = parseInt(tokens[2]);
            const textureName = tokens[3];

            let idx = 4;
            const modelsToLoad = [];

            for (let i = 0; i < numObjects; i++) {
                if (idx >= tokens.length) break;
                const cobName = tokens[idx];
                const anim = parseInt(tokens[idx + 1]) || 0;
                const shadeType = parseInt(tokens[idx + 2]) || 0;
                const useTsPos = parseInt(tokens[idx + 3]) || 0;
                idx += 4;

                let wx = 0, wy = 0, wz = 0;
                if (useTsPos === 0 && idx + 3 <= tokens.length) {
                    wx = parseFloat(tokens[idx]) || 0;
                    wy = parseFloat(tokens[idx + 1]) || 0;
                    wz = parseFloat(tokens[idx + 2]) || 0;
                    idx += 3;
                }

                const tmapIdx = parseInt(tokens[idx]) || 0;
                const frustum = parseInt(tokens[idx + 1]) || 0;
                const trans = parseInt(tokens[idx + 2]) || 0;
                const r = parseInt(tokens[idx + 3]) || 32;
                const g = parseInt(tokens[idx + 4]) || 32;
                const b = parseInt(tokens[idx + 5]) || 32;
                const cull = parseInt(tokens[idx + 6]) || 0;
                const clipZ = parseInt(tokens[idx + 7]) || 0;
                idx += 8;

                modelsToLoad.push({
                    cob: cobName.toUpperCase(),
                    shade: shadeType,
                    pos: { x: wx, y: wy, z: wz },
                    color: new THREE.Color(r / 64, g / 64, b / 64),
                    useTsPos: useTsPos === 1
                });
            }

            // Load and instantiate each .COB model
            for (const item of modelsToLoad) {
                try {
                    const cobUrl = `assets/models/${item.cob}`;
                    const cobResp = await fetch(cobUrl);
                    if (!cobResp.ok) continue;

                    const arrayBuf = await cobResp.arrayBuffer();
                    const cobData = COBParser.parse(arrayBuf, true);

                    const geo = new THREE.BufferGeometry();
                    geo.setAttribute('position', new THREE.BufferAttribute(cobData.positions, 3));
                    if (cobData.uvs) {
                        geo.setAttribute('uv', new THREE.BufferAttribute(cobData.uvs, 2));
                    }
                    geo.computeVertexNormals();

                    // Materials
                    const modernMat = new THREE.MeshStandardMaterial({
                        color: item.color.getHex() === 0 ? 0x00ffee : item.color,
                        metalness: 0.8,
                        roughness: 0.2,
                        side: THREE.DoubleSide
                    });

                    const retroMat = new THREE.MeshLambertMaterial({
                        color: item.color.getHex() === 0 ? 0x00ccaa : item.color,
                        side: THREE.DoubleSide
                    });

                    const mesh = new THREE.Mesh(geo, modernMat);
                    mesh.userData = { retroMat, modernMat, originalPos: item.pos, cobName: item.cob };

                    if (!item.useTsPos) {
                        mesh.position.set(item.pos.x, item.pos.y, item.pos.z);
                    }

                    group.add(mesh);
                } catch (err) {
                    console.warn(`Could not load model ${item.cob}:`, err);
                }
            }

        } catch (e) {
            console.warn(`Could not load world ${wldPath}:`, e);
        }

        return group;
    }
}

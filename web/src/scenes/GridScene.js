import { WorldLoader } from '../core/WorldLoader.js';

/**
 * Scene 1: Authentic Waving Box Matrix (SCENE1.WLD)
 */
export class GridScene {
    constructor(app) {
        this.app = app;
        this.group = new app.THREE.Group();
        this.worldGroup = null;
        this.boxMeshes = [];
        this.nutMesh = null;
    }

    async init() {
        const THREE = this.app.THREE;
        const loader = new WorldLoader(this.app);

        // Load genuine 1997 SCENE1.WLD scene graph
        this.worldGroup = await loader.loadWorld('assets/worlds/SCENE1.WLD');
        this.group.add(this.worldGroup);

        this.worldGroup.children.forEach(child => {
            if (child.userData && child.userData.cobName) {
                if (child.userData.cobName.startsWith('BOX')) {
                    this.boxMeshes.push(child);
                } else if (child.userData.cobName === 'NUT.COB' || child.userData.cobName === 'WHEEL.COB') {
                    this.nutMesh = child;
                }
            }
        });

        // Stage lighting
        const pLight = new THREE.PointLight(0x00ff88, 50, 40);
        pLight.position.set(0, 15, 0);
        this.group.add(pLight);
    }

    setMode(mode) {
        const isRetro = mode === 'retro';
        if (this.worldGroup) {
            this.worldGroup.traverse(child => {
                if (child.isMesh && child.userData && child.userData.retroMat) {
                    child.material = isRetro ? child.userData.retroMat : child.userData.modernMat;
                }
            });
        }
    }

    update(delta, time, bassEnergy, freqData) {
        // Mathematical wave oscillation on the 25 genuine BOX models
        this.boxMeshes.forEach((mesh, idx) => {
            const wave = Math.sin(time * 3.5 + idx * 0.4) * 2.5;
            mesh.position.y = wave;
            mesh.rotation.y = time * 0.5 + idx * 0.1;
        });

        if (this.nutMesh) {
            this.nutMesh.rotation.x += delta * 1.5;
            this.nutMesh.rotation.y += delta * 2.0;
        }
    }
}

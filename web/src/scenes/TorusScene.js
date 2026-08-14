import { WorldLoader } from '../core/WorldLoader.js';

/**
 * Scene 2: Authentic Multi-Axis 3D Toruses (SCENE2.WLD)
 */
export class TorusScene {
    constructor(app) {
        this.app = app;
        this.group = new app.THREE.Group();
        this.worldGroup = null;
        this.torusMeshes = [];
    }

    async init() {
        const THREE = this.app.THREE;
        const loader = new WorldLoader(this.app);

        // Load genuine 1997 SCENE2.WLD scene graph
        this.worldGroup = await loader.loadWorld('assets/worlds/SCENE2.WLD');
        this.group.add(this.worldGroup);

        this.worldGroup.children.forEach(child => {
            if (child.userData && child.userData.cobName && child.userData.cobName.startsWith('TOR')) {
                this.torusMeshes.push(child);
            }
        });

        // Stage lighting
        const pLight = new THREE.PointLight(0x00ffff, 45, 50);
        pLight.position.set(0, 10, 0);
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

    update(delta, time, bassEnergy) {
        this.torusMeshes.forEach((mesh, idx) => {
            mesh.rotation.x += delta * (1.0 + idx * 0.4);
            mesh.rotation.y += delta * (1.5 - idx * 0.3);
            mesh.rotation.z += delta * 0.8;
        });
    }
}

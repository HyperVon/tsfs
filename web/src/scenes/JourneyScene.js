import { WorldLoader } from '../core/WorldLoader.js';

/**
 * Scene 3: Authentic Road Journey (SCENE3.WLD)
 */
export class JourneyScene {
    constructor(app) {
        this.app = app;
        this.group = new app.THREE.Group();
        this.worldGroup = null;
        this.signMeshes = [];
    }

    async init() {
        const THREE = this.app.THREE;
        const loader = new WorldLoader(this.app);

        // Load genuine 1997 SCENE3.WLD scene graph
        this.worldGroup = await loader.loadWorld('assets/worlds/SCENE3.WLD');
        this.group.add(this.worldGroup);

        this.worldGroup.children.forEach(child => {
            if (child.userData && child.userData.cobName && child.userData.cobName.startsWith('SIGN')) {
                this.signMeshes.push(child);
            }
        });

        // Stage lighting
        const pLight = new THREE.PointLight(0x00ffff, 40, 80);
        pLight.position.set(0, 0, 0);
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

    update(delta, time) {
        this.signMeshes.forEach((mesh, idx) => {
            mesh.rotation.y = Math.sin(time * 2.0 + idx) * 0.3;
        });
    }
}

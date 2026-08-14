import { WorldLoader } from '../core/WorldLoader.js';

/**
 * Scene 0: Authentic Chaotic Order 3D Logo & World (COINTRO.WLD)
 */
export class IntroScene {
    constructor(app) {
        this.app = app;
        this.group = new app.THREE.Group();
        this.worldGroup = null;
        this.coMesh = null;
    }

    async init() {
        const THREE = this.app.THREE;
        const loader = new WorldLoader(this.app);

        // Load genuine 1997 COINTRO.WLD scene graph
        this.worldGroup = await loader.loadWorld('assets/worlds/COINTRO.WLD');
        this.group.add(this.worldGroup);

        // Find the CO logo mesh for specific animation
        this.worldGroup.children.forEach(child => {
            if (child.userData && child.userData.cobName === 'CO.COB') {
                this.coMesh = child;
            }
        });

        // Add stage lighting
        const pLight1 = new THREE.PointLight(0x00ffff, 40, 50);
        pLight1.position.set(10, 15, 10);
        this.group.add(pLight1);

        const pLight2 = new THREE.PointLight(0xff0066, 30, 50);
        pLight2.position.set(-10, 10, -10);
        this.group.add(pLight2);
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
        if (this.coMesh) {
            this.coMesh.rotation.y += delta * 1.5;
            this.coMesh.rotation.x = Math.sin(time * 2.0) * 0.2;
        }

        // Animate spikes in the scene
        if (this.worldGroup) {
            this.worldGroup.children.forEach((child, idx) => {
                if (child.userData && child.userData.cobName && child.userData.cobName.startsWith('SPIKE')) {
                    child.position.y = Math.sin(time * 3.0 + idx) * 1.2;
                }
            });
        }
    }
}

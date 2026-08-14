import { VideoCube } from '../video/VideoCube.js';
import { WorldLoader } from '../core/WorldLoader.js';

/**
 * Scene 4: Authentic 3D Video Cube (SCENE4.WLD)
 */
export class VideoCubeScene {
    constructor(app) {
        this.app = app;
        this.group = new app.THREE.Group();
        this.videoCube = null;
        this.worldGroup = null;
    }

    async init() {
        const THREE = this.app.THREE;
        const loader = new WorldLoader(this.app);

        // Load genuine 1997 SCENE4.WLD scene graph (planes & sphere)
        this.worldGroup = await loader.loadWorld('assets/worlds/SCENE4.WLD');
        this.group.add(this.worldGroup);

        // Add 3D Video Cube with video stream
        this.videoCube = new VideoCube('assets/videos/cube_video.mp4');
        this.videoCube.init(THREE);
        this.group.add(this.videoCube.mesh);

        // Orbital rings
        for (let i = 0; i < 3; i++) {
            const ringGeo = new THREE.TorusGeometry(10 + i * 3, 0.2, 16, 64);
            const ringMat = new THREE.MeshStandardMaterial({
                color: i === 0 ? 0x00ffcc : 0xff0088,
                emissive: i === 0 ? 0x00ffcc : 0xff0088,
                emissiveIntensity: 0.5
            });
            const ring = new THREE.Mesh(ringGeo, ringMat);
            this.group.add(ring);
        }

        // Lighting
        const l1 = new THREE.PointLight(0x00ffff, 40, 50);
        l1.position.set(5, 10, 5);
        this.group.add(l1);
    }

    setMode(mode) {
        if (this.videoCube) {
            this.videoCube.setMode(mode);
        }
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
        if (this.videoCube) {
            this.videoCube.update(delta, bassEnergy);
        }
    }
}

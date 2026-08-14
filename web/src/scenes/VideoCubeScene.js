import { VideoCube } from '../video/VideoCube.js';

/**
 * Scene 4: The 3D Video Cube (Live Video Texture Mapped)
 */
export class VideoCubeScene {
    constructor(app) {
        this.app = app;
        this.group = new app.THREE.Group();
        this.videoCube = null;
        this.backgroundRings = [];
    }

    async init() {
        const THREE = this.app.THREE;

        // Create VideoCube using dedicated lightweight cube video loop
        this.videoCube = new VideoCube('assets/videos/cube_video.mp4');
        this.videoCube.init(THREE);
        this.group.add(this.videoCube.mesh);

        // Surrounding orbital neon rings (TORENV.COB / RINGS.COB)
        for (let i = 0; i < 3; i++) {
            const ringGeo = new THREE.TorusGeometry(8 + i * 3, 0.25, 16, 64);
            const ringMat = new THREE.MeshStandardMaterial({
                color: i === 0 ? 0x00ffcc : (i === 1 ? 0xff0088 : 0xffee00),
                emissive: i === 0 ? 0x00ffcc : (i === 1 ? 0xff0088 : 0xffee00),
                emissiveIntensity: 0.5,
                metalness: 0.9,
                roughness: 0.1
            });
            const ring = new THREE.Mesh(ringGeo, ringMat);
            this.backgroundRings.push(ring);
            this.group.add(ring);
        }

        // Surrounding starfield
        const starGeo = new THREE.BufferGeometry();
        const starCount = 300;
        const starPositions = new Float32Array(starCount * 3);
        for (let i = 0; i < starCount * 3; i++) {
            starPositions[i] = (Math.random() - 0.5) * 160;
        }
        starGeo.setAttribute('position', new THREE.BufferAttribute(starPositions, 3));
        const starMat = new THREE.PointsMaterial({ color: 0x00ffff, size: 0.5 });
        this.stars = new THREE.Points(starGeo, starMat);
        this.group.add(this.stars);

        // Point lights
        const l1 = new THREE.PointLight(0x00ffea, 50, 40);
        l1.position.set(8, 8, 8);
        this.group.add(l1);
    }

    setMode(mode) {
        if (this.videoCube) {
            this.videoCube.setMode(mode);
        }
    }

    update(delta, time, bassEnergy) {
        if (this.videoCube) {
            this.videoCube.update(delta, bassEnergy);
        }

        this.backgroundRings.forEach((r, i) => {
            r.rotation.x += delta * (0.4 + i * 0.2);
            r.rotation.y += delta * (0.6 - i * 0.1);
        });

        if (this.stars) {
            this.stars.rotation.y += delta * 0.05;
        }
    }
}

/**
 * Scene 5: Ending Screen & Demoscene Tribute
 */

export class OutroScene {
    constructor(app) {
        this.app = app;
        this.group = new app.THREE.Group();
    }

    async init() {
        const THREE = this.app.THREE;

        // Ending Screen Billboard (Steve / TG97 tribute)
        const planeGeo = new THREE.PlaneGeometry(16, 10);
        const texture = new THREE.TextureLoader().load('assets/images/ending_steve.png');
        texture.minFilter = THREE.LinearFilter;
        texture.magFilter = THREE.NearestFilter;

        const planeMat = new THREE.MeshBasicMaterial({
            map: texture,
            transparent: true,
            side: THREE.DoubleSide
        });

        this.billboard = new THREE.Mesh(planeGeo, planeMat);
        this.billboard.position.set(0, 0, 0);
        this.group.add(this.billboard);

        // Orbiting particle ring
        const partGeo = new THREE.BufferGeometry();
        const count = 100;
        const pos = new Float32Array(count * 3);
        for (let i = 0; i < count; i++) {
            const a = (i / count) * Math.PI * 2;
            pos[i * 3] = Math.cos(a) * 12;
            pos[i * 3 + 1] = (Math.random() - 0.5) * 4;
            pos[i * 3 + 2] = Math.sin(a) * 12;
        }
        partGeo.setAttribute('position', new THREE.BufferAttribute(pos, 3));
        const partMat = new THREE.PointsMaterial({ color: 0x00ffaa, size: 0.4 });
        this.particles = new THREE.Points(partGeo, partMat);
        this.group.add(this.particles);
    }

    setMode(mode) {
        // Mode adjustments
    }

    update(delta, time) {
        if (this.billboard) {
            this.billboard.rotation.y = Math.sin(time * 0.8) * 0.15;
            this.billboard.position.y = Math.sin(time * 1.5) * 0.4;
        }
        if (this.particles) {
            this.particles.rotation.y += delta * 0.3;
        }
    }
}

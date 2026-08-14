/**
 * Scene 2: Multi-Axis 3D Toruses & Chrome Pedestal
 * Rotating concentric toruses, specular chrome spheres, and pedestal.
 */

export class TorusScene {
    constructor(app) {
        this.app = app;
        this.group = new app.THREE.Group();
        this.rings = [];
        this.spheres = [];
    }

    async init() {
        const THREE = this.app.THREE;

        // Concentric 3D Torus Rings (TOR1.COB, TOR2.COB, TOR3.COB, RINGS.COB)
        const ringConfigs = [
            { radius: 6.5, tube: 0.6, color: 0x00ffff, rotSpeed: [1.2, 0.8, 0.4] },
            { radius: 4.8, tube: 0.5, color: 0xff0088, rotSpeed: [-0.9, 1.4, -0.6] },
            { radius: 3.2, tube: 0.4, color: 0xffee00, rotSpeed: [0.7, -1.1, 1.3] }
        ];

        ringConfigs.forEach(cfg => {
            const geo = new THREE.TorusGeometry(cfg.radius, cfg.tube, 24, 48);
            const retroMat = new THREE.MeshLambertMaterial({ color: cfg.color });
            const modernMat = new THREE.MeshStandardMaterial({
                color: cfg.color,
                roughness: 0.1,
                metalness: 0.95,
                emissive: cfg.color,
                emissiveIntensity: 0.25
            });

            const mesh = new THREE.Mesh(geo, modernMat);
            this.rings.push({ mesh, retroMat, modernMat, rotSpeed: cfg.rotSpeed });
            this.group.add(mesh);
        });

        // Specular Center Chrome Ball (BALL1.COB / SPHERE2.COB)
        const sphereGeo = new THREE.SphereGeometry(1.8, 32, 32);
        this.sphereRetroMat = new THREE.MeshLambertMaterial({ color: 0xffffff });
        this.sphereModernMat = new THREE.MeshStandardMaterial({
            color: 0xffffff,
            roughness: 0.05,
            metalness: 1.0
        });

        this.centerSphere = new THREE.Mesh(sphereGeo, this.sphereModernMat);
        this.group.add(this.centerSphere);

        // Orbiting Satellite Spheres (BALL2.COB)
        for (let i = 0; i < 4; i++) {
            const satGeo = new THREE.SphereGeometry(0.6, 16, 16);
            const satMat = new THREE.MeshStandardMaterial({ color: 0x00ffcc, metalness: 0.9 });
            const sat = new THREE.Mesh(satGeo, satMat);
            this.spheres.push(sat);
            this.group.add(sat);
        }

        // Lower Pedestal (PED.COB / PEDFLOR.COB)
        const pedGeo = new THREE.CylinderGeometry(8, 9, 2.5, 32);
        const pedMat = new THREE.MeshStandardMaterial({ color: 0x112233, metalness: 0.8 });
        const pedestal = new THREE.Mesh(pedGeo, pedMat);
        pedestal.position.y = -7;
        this.group.add(pedestal);

        // Point lights
        const pl = new THREE.PointLight(0x00ffff, 45, 40);
        pl.position.set(0, 5, 0);
        this.group.add(pl);
    }

    setMode(mode) {
        const isRetro = mode === 'retro';
        this.rings.forEach(r => {
            r.mesh.material = isRetro ? r.retroMat : r.modernMat;
        });
        if (this.centerSphere) {
            this.centerSphere.material = isRetro ? this.sphereRetroMat : this.sphereModernMat;
        }
    }

    update(delta, time, bassEnergy) {
        this.rings.forEach(r => {
            r.mesh.rotation.x += delta * r.rotSpeed[0];
            r.mesh.rotation.y += delta * r.rotSpeed[1];
            r.mesh.rotation.z += delta * r.rotSpeed[2];
        });

        // Orbiting satellite spheres
        this.spheres.forEach((sat, i) => {
            const angle = time * 2.0 + (i * Math.PI / 2);
            const radius = 8.5;
            sat.position.set(Math.cos(angle) * radius, Math.sin(time * 3.0 + i) * 2.5, Math.sin(angle) * radius);
        });

        if (this.centerSphere) {
            const pulse = 1.0 + Math.sin(time * 4.0) * 0.1 + (bassEnergy * 0.2);
            this.centerSphere.scale.set(pulse, pulse, pulse);
        }
    }
}

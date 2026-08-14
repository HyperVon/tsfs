/**
 * Scene 0: Chaotic Order 3D Logo & Spikes Intro
 */

export class IntroScene {
    constructor(app) {
        this.app = app;
        this.group = new app.THREE.Group();
        this.logoMesh = null;
        this.spikes = [];
        this.floor = null;
        this.pointLights = [];
    }

    async init() {
        const THREE = this.app.THREE;

        // Ground grid / floor
        const gridGeo = new THREE.PlaneGeometry(120, 120, 32, 32);
        this.floorMatRetro = new THREE.MeshBasicMaterial({ color: 0x004422, wireframe: true });
        this.floorMatModern = new THREE.MeshStandardMaterial({
            color: 0x051510,
            roughness: 0.1,
            metalness: 0.9,
            wireframe: false
        });

        this.floor = new THREE.Mesh(gridGeo, this.floorMatModern);
        this.floor.rotation.x = -Math.PI / 2;
        this.floor.position.y = -6;
        this.group.add(this.floor);

        // Neon Floor Grid overlay for modern mode
        const gridHelper = new THREE.GridHelper(120, 40, 0x00ffaa, 0x003322);
        gridHelper.position.y = -5.95;
        this.gridHelper = gridHelper;
        this.group.add(gridHelper);

        // Load CO.COB 3D logo or procedural fallback
        try {
            const logoGeo = await this.app.loadCOBGeometry('assets/models/CO.COB');
            this.logoMatRetro = new THREE.MeshLambertMaterial({ color: 0x00ffff });
            this.logoMatModern = new THREE.MeshStandardMaterial({
                color: 0x00f0ff,
                roughness: 0.1,
                metalness: 0.95,
                emissive: 0x003344,
                emissiveIntensity: 0.4
            });
            this.logoMesh = new THREE.Mesh(logoGeo, this.logoMatModern);
            this.logoMesh.scale.set(0.04, 0.04, 0.04);
            this.logoMesh.position.set(0, 0, 0);
            this.group.add(this.logoMesh);
        } catch (e) {
            // Procedural fallback for CO letters
            const torusGeo = new THREE.TorusGeometry(3.5, 0.9, 16, 32);
            this.logoMesh = new THREE.Mesh(torusGeo, new THREE.MeshStandardMaterial({ color: 0x00ffee, metalness: 0.9 }));
            this.group.add(this.logoMesh);
        }

        // Spike pillars (SPIKE1.COB .. SPIKE11.COB)
        const spikeCount = 10;
        for (let i = 0; i < spikeCount; i++) {
            const angle = (i / spikeCount) * Math.PI * 2;
            const radius = 18;
            const coneGeo = new THREE.ConeGeometry(1.2, 14, 8);
            const coneMat = new THREE.MeshStandardMaterial({
                color: i % 2 === 0 ? 0xff0077 : 0x00ffcc,
                metalness: 0.8,
                roughness: 0.2
            });
            const spike = new THREE.Mesh(coneGeo, coneMat);
            spike.position.set(Math.cos(angle) * radius, 1, Math.sin(angle) * radius);
            this.spikes.push(spike);
            this.group.add(spike);
        }

        // Dynamic colored point lights
        const light1 = new THREE.PointLight(0x00ffff, 40, 50);
        light1.position.set(10, 8, 10);
        this.group.add(light1);

        const light2 = new THREE.PointLight(0xff0055, 30, 50);
        light2.position.set(-10, 5, -10);
        this.group.add(light2);

        this.pointLights = [light1, light2];
    }

    setMode(mode) {
        const isRetro = mode === 'retro';
        if (this.floor) this.floor.material = isRetro ? this.floorMatRetro : this.floorMatModern;
        if (this.logoMesh) this.logoMesh.material = isRetro ? this.logoMatRetro : this.logoMatModern;
        if (this.gridHelper) this.gridHelper.visible = !isRetro;
    }

    update(delta, time, bassEnergy) {
        if (this.logoMesh) {
            this.logoMesh.rotation.y += delta * 1.2;
            this.logoMesh.rotation.x = Math.sin(time * 1.5) * 0.25;
            this.logoMesh.position.y = Math.sin(time * 2.0) * 0.5;

            if (this.app.mode === 'modern') {
                const s = 0.04 * (1.0 + bassEnergy * 0.3);
                this.logoMesh.scale.set(s, s, s);
            }
        }

        // Oscillate spikes
        this.spikes.forEach((spike, idx) => {
            spike.position.y = 1 + Math.sin(time * 3.0 + idx) * 1.5;
            spike.rotation.y += delta * 0.5;
        });
    }
}

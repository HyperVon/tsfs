/**
 * Scene 1: Waving 3D Box Matrix & Morphing Geometry
 * 25 dynamic boxes with sine-wave mathematical displacement + live music reactivity.
 */

export class GridScene {
    constructor(app) {
        this.app = app;
        this.group = new app.THREE.Group();
        this.boxes = [];
        this.floatingObjects = [];
    }

    async init() {
        const THREE = this.app.THREE;
        const gridSize = 5; // 5x5 = 25 boxes matching BOX1.COB .. BOX25.COB
        const spacing = 3.2;

        const boxGeo = new THREE.BoxGeometry(2.4, 2.4, 2.4);

        for (let x = 0; x < gridSize; x++) {
            for (let z = 0; z < gridSize; z++) {
                const posX = (x - (gridSize - 1) / 2) * spacing;
                const posZ = (z - (gridSize - 1) / 2) * spacing;

                const retroMat = new THREE.MeshLambertMaterial({
                    color: (x + z) % 2 === 0 ? 0x00cc88 : 0x0088cc
                });

                const modernMat = new THREE.MeshStandardMaterial({
                    color: (x + z) % 2 === 0 ? 0x00ffaa : 0x00aaff,
                    roughness: 0.1,
                    metalness: 0.85,
                    emissive: 0x002233,
                    emissiveIntensity: 0.2
                });

                const mesh = new THREE.Mesh(boxGeo, modernMat);
                mesh.position.set(posX, 0, posZ);

                this.boxes.push({
                    mesh,
                    retroMat,
                    modernMat,
                    baseX: posX,
                    baseZ: posZ,
                    gridX: x,
                    gridZ: z
                });

                this.group.add(mesh);
            }
        }

        // Center spinning centerpiece (STAR.COB / NUT.COB / WHEEL.COB)
        try {
            const nutGeo = await this.app.loadCOBGeometry('assets/models/NUT.COB');
            const centerMat = new THREE.MeshStandardMaterial({
                color: 0xff0055,
                metalness: 0.9,
                roughness: 0.15,
                emissive: 0x330011,
                emissiveIntensity: 0.5
            });
            this.centerpiece = new THREE.Mesh(nutGeo, centerMat);
            this.centerpiece.scale.set(0.05, 0.05, 0.05);
            this.centerpiece.position.set(0, 8, 0);
            this.group.add(this.centerpiece);
        } catch (e) {
            const knotGeo = new THREE.TorusKnotGeometry(2.5, 0.7, 64, 16);
            this.centerpiece = new THREE.Mesh(knotGeo, new THREE.MeshStandardMaterial({ color: 0xff0066, metalness: 0.9 }));
            this.centerpiece.position.set(0, 8, 0);
            this.group.add(this.centerpiece);
        }

        // Ambient and point lights
        const pl1 = new THREE.PointLight(0x00ff88, 50, 40);
        pl1.position.set(0, 10, 0);
        this.group.add(pl1);
    }

    setMode(mode) {
        const isRetro = mode === 'retro';
        this.boxes.forEach(b => {
            b.mesh.material = isRetro ? b.retroMat : b.modernMat;
        });
    }

    update(delta, time, bassEnergy, freqData) {
        const isModern = this.app.mode === 'modern';

        // 25-box mathematical wave displacement
        this.boxes.forEach((b, idx) => {
            const distFromCenter = Math.sqrt(b.baseX * b.baseX + b.baseZ * b.baseZ);
            const waveY = Math.sin(time * 3.5 - distFromCenter * 0.6) * 3.0;

            let audioBoost = 0;
            if (isModern && freqData && freqData.length > idx) {
                audioBoost = (freqData[idx % 32] / 255.0) * 4.0;
            }

            b.mesh.position.y = waveY + audioBoost;
            b.mesh.rotation.y = time * 0.8 + (b.gridX + b.gridZ) * 0.2;
            b.mesh.rotation.x = Math.sin(time * 2.0 + b.gridX) * 0.3;
        });

        if (this.centerpiece) {
            this.centerpiece.rotation.x += delta * 1.5;
            this.centerpiece.rotation.y += delta * 2.0;
            this.centerpiece.rotation.z += delta * 0.8;
            this.centerpiece.position.y = 8 + Math.sin(time * 2.5) * 1.8;
        }
    }
}

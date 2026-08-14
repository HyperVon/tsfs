/**
 * Scene 3: The Search Journey & Road Signs
 * Camera sweeps through a 3D landscape with floating road signs (SIGN1.COB .. SIGN6.COB).
 */

export class JourneyScene {
    constructor(app) {
        this.app = app;
        this.group = new app.THREE.Group();
        this.signs = [];
        this.terrain = null;
    }

    async init() {
        const THREE = this.app.THREE;

        // Long futuristic highway / tunnel geometry
        const tunnelGeo = new THREE.CylinderGeometry(14, 14, 180, 24, 32, true);
        this.tunnelMatRetro = new THREE.MeshBasicMaterial({ color: 0x002244, wireframe: true });
        this.tunnelMatModern = new THREE.MeshStandardMaterial({
            color: 0x020814,
            roughness: 0.2,
            metalness: 0.9,
            wireframe: false,
            side: THREE.BackSide
        });

        this.tunnel = new THREE.Mesh(tunnelGeo, this.tunnelMatModern);
        this.tunnel.rotation.x = Math.PI / 2;
        this.group.add(this.tunnel);

        // Road Signs along the path (SIGN1.COB .. SIGN6.COB)
        const signLabels = ['SEARCH', 'FOR', 'STEVE?', 'HAMAR', 'TG97', 'CO'];
        for (let i = 0; i < 6; i++) {
            const signGeo = new THREE.BoxGeometry(4.0, 2.5, 0.4);

            // Create canvas texture for sign text
            const canvas = document.createElement('canvas');
            canvas.width = 256;
            canvas.height = 160;
            const ctx = canvas.getContext('2d');
            ctx.fillStyle = '#001a33';
            ctx.fillRect(0, 0, 256, 160);
            ctx.strokeStyle = '#00ffcc';
            ctx.lineWidth = 8;
            ctx.strokeRect(8, 8, 240, 144);
            ctx.fillStyle = '#00ffee';
            ctx.font = 'bold 36px monospace';
            ctx.textAlign = 'center';
            ctx.textBaseline = 'middle';
            ctx.fillText(signLabels[i], 128, 80);

            const texture = new THREE.CanvasTexture(canvas);
            const signMat = new THREE.MeshStandardMaterial({
                map: texture,
                metalness: 0.7,
                roughness: 0.3,
                emissive: 0x003344,
                emissiveMap: texture,
                emissiveIntensity: 0.6
            });

            const signMesh = new THREE.Mesh(signGeo, signMat);
            signMesh.position.set((i % 2 === 0 ? -5 : 5), (Math.random() - 0.5) * 4, -60 + i * 24);
            this.signs.push(signMesh);
            this.group.add(signMesh);
        }

        // Lighting
        const pLight = new THREE.PointLight(0x00ffff, 40, 80);
        pLight.position.set(0, 0, 0);
        this.group.add(pLight);
    }

    setMode(mode) {
        const isRetro = mode === 'retro';
        if (this.tunnel) {
            this.tunnel.material = isRetro ? this.tunnelMatRetro : this.tunnelMatModern;
        }
    }

    update(delta, time) {
        this.signs.forEach((s, idx) => {
            s.rotation.y = Math.sin(time * 1.5 + idx) * 0.2;
            s.position.z += delta * 18;
            if (s.position.z > 40) {
                s.position.z = -80;
            }
        });
    }
}

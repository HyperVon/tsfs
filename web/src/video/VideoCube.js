/**
 * 3D Video Cube Engine
 * Maps the live video stream onto a 3D cube with particle emitters and neon shaders.
 */

export class VideoCube {
    constructor(videoUrl) {
        this.video = document.createElement('video');
        this.video.src = videoUrl;
        this.video.crossOrigin = 'anonymous';
        this.video.loop = true;
        this.video.muted = true;
        this.video.playsInline = true;
        this.video.autoplay = true;

        this.texture = null;
        this.mesh = null;
        this.particleGroup = null;
    }

    init(THREE) {
        this.texture = new THREE.VideoTexture(this.video);
        this.texture.minFilter = THREE.LinearFilter;
        this.texture.magFilter = THREE.NearestFilter;

        const geometry = new THREE.BoxGeometry(6, 6, 6);

        this.retroMaterial = new THREE.MeshBasicMaterial({
            map: this.texture,
            color: 0xffffff
        });

        this.modernMaterial = new THREE.MeshStandardMaterial({
            map: this.texture,
            roughness: 0.15,
            metalness: 0.85,
            emissive: 0x223344,
            emissiveMap: this.texture,
            emissiveIntensity: 0.4
        });

        this.mesh = new THREE.Mesh(geometry, this.modernMaterial);
        this.mesh.position.set(0, 0, 0);

        // Particle halo
        const particleCount = 100;
        const particleGeo = new THREE.BufferGeometry();
        const positions = new Float32Array(particleCount * 3);

        for (let i = 0; i < particleCount; i++) {
            positions[i * 3] = (Math.random() - 0.5) * 12;
            positions[i * 3 + 1] = (Math.random() - 0.5) * 12;
            positions[i * 3 + 2] = (Math.random() - 0.5) * 12;
        }

        particleGeo.setAttribute('position', new THREE.BufferAttribute(positions, 3));

        const particleMat = new THREE.PointsMaterial({
            size: 0.4,
            color: 0x00ffcc,
            transparent: true,
            opacity: 0.8
        });

        this.particleGroup = new THREE.Points(particleGeo, particleMat);
        this.mesh.add(this.particleGroup);

        this.play();
    }

    play() {
        if (this.video) {
            this.video.play().catch(() => {});
        }
    }

    setMode(mode) {
        if (this.mesh) {
            this.mesh.material = mode === 'retro' ? this.retroMaterial : this.modernMaterial;
            if (this.particleGroup) {
                this.particleGroup.visible = mode === 'modern';
            }
        }
    }

    update(delta, bassEnergy = 0) {
        if (this.video && this.video.paused) {
            this.video.play().catch(() => {});
        }

        if (this.mesh) {
            this.mesh.rotation.x += delta * 0.9;
            this.mesh.rotation.y += delta * 1.4;
            this.mesh.rotation.z += delta * 0.6;

            const targetScale = 1.0 + bassEnergy * 0.15;
            this.mesh.scale.set(targetScale, targetScale, targetScale);
        }
    }
}

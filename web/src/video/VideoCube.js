/**
 * 3D Video Cube Engine
 * Maps the live video stream onto a 3D cube with particle emitters and neon shaders.
 */

export class VideoCube {
    /**
     * @param {string} videoUrl (URL to MP4 video stream or animated frame sequence)
     */
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
        this.texture.magFilter = THREE.NearestFilter; // Crisp retro texels

        const geometry = new THREE.BoxGeometry(4.5, 4.5, 4.5);

        // Retro material vs Modern material
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
            emissiveIntensity: 0.35
        });

        this.mesh = new THREE.Mesh(geometry, this.modernMaterial);

        // Create trailing particle system
        const particleCount = 120;
        const particleGeo = new THREE.BufferGeometry();
        const positions = new Float32Array(particleCount * 3);
        const colors = new Float32Array(particleCount * 3);

        for (let i = 0; i < particleCount; i++) {
            positions[i * 3] = (Math.random() - 0.5) * 8;
            positions[i * 3 + 1] = (Math.random() - 0.5) * 8;
            positions[i * 3 + 2] = (Math.random() - 0.5) * 8;

            colors[i * 3] = 0.2 + Math.random() * 0.8; // R
            colors[i * 3 + 1] = 0.6 + Math.random() * 0.4; // G
            colors[i * 3 + 2] = 1.0; // B
        }

        particleGeo.setAttribute('position', new THREE.BufferAttribute(positions, 3));
        particleGeo.setAttribute('color', new THREE.BufferAttribute(colors, 3));

        const particleMat = new THREE.PointsMaterial({
            size: 0.3,
            vertexColors: true,
            transparent: true,
            opacity: 0.8,
            blending: THREE.AdditiveBlending
        });

        this.particleGroup = new THREE.Points(particleGeo, particleMat);
        this.mesh.add(this.particleGroup);

        this.video.play().catch(() => {});
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
        if (this.mesh) {
            // Rotate cube
            this.mesh.rotation.x += delta * 0.9;
            this.mesh.rotation.y += delta * 1.4;
            this.mesh.rotation.z += delta * 0.7;

            // Audio reactive scale pulse in modern mode
            const targetScale = 1.0 + bassEnergy * 0.25;
            this.mesh.scale.lerp(new THREE.Vector3(targetScale, targetScale, targetScale), 0.2);

            if (this.particleGroup && this.particleGroup.visible) {
                this.particleGroup.rotation.y -= delta * 0.8;
            }
        }
    }
}

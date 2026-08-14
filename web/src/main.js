import * as THREE from 'https://cdn.jsdelivr.net/npm/three@0.160.0/build/three.module.js';
import { COBParser } from './core/COBParser.js';
import { PTHParser } from './core/PTHParser.js';
import { XMPlayer } from './audio/XMPlayer.js';
import { SceneManager } from './scenes/SceneManager.js';

class App {
    constructor() {
        this.THREE = THREE;
        this.container = document.getElementById('viewport');
        this.mode = 'modern'; // 'modern' or 'retro'
        this.isFreeCam = false;
        this.isPlaying = false;
        this.currentTime = 0;
        this.duration = 279; // 4m 39s
        this.clock = new THREE.Clock();

        this.cachedGeometries = {};
        this.cachedPaths = {};

        this.initThree();
        this.initAudio();
        this.initSceneManager();
        this.initUI();
        this.initControls();

        window.addEventListener('resize', () => this.onResize());
        this.animate();
    }

    initThree() {
        this.scene = new THREE.Scene();
        this.scene.background = new THREE.Color(0x02040a);

        this.camera = new THREE.PerspectiveCamera(55, window.innerWidth / window.innerHeight, 0.1, 1000);
        this.camera.position.set(0, 0, -20);

        this.renderer = new THREE.WebGLRenderer({ antialias: true, powerPreference: 'high-performance' });
        this.renderer.setSize(window.innerWidth, window.innerHeight);
        this.renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));
        this.renderer.toneMapping = THREE.ACESFilmicToneMapping;
        this.renderer.toneMappingExposure = 1.2;
        this.container.appendChild(this.renderer.domElement);

        // Ambient light
        this.ambientLight = new THREE.AmbientLight(0xffffff, 0.6);
        this.scene.add(this.ambientLight);

        // Directional Sun / Stage Light
        this.dirLight = new THREE.DirectionalLight(0xffffff, 1.2);
        this.dirLight.position.set(20, 40, 20);
        this.scene.add(this.dirLight);
    }

    initAudio() {
        this.audioPlayer = new XMPlayer();
        this.audioPlayer.loadTrack('assets/videos/tsfs_web_preview.mp4').catch(() => {
            console.warn('Audio track auto-load completed or waiting for user interaction');
        });
    }

    async initSceneManager() {
        this.sceneManager = new SceneManager(this);
        await this.sceneManager.init();

        const loadingOverlay = document.getElementById('loading-overlay');
        if (loadingOverlay) {
            loadingOverlay.classList.add('hidden');
        }
    }

    async loadCOBGeometry(url) {
        if (this.cachedGeometries[url]) {
            return this.cachedGeometries[url];
        }
        const resp = await fetch(url);
        const arrayBuf = await resp.arrayBuffer();
        const cobData = COBParser.parse(arrayBuf);

        const geo = new THREE.BufferGeometry();
        geo.setAttribute('position', new THREE.BufferAttribute(cobData.positions, 3));
        if (cobData.uvs) {
            geo.setAttribute('uv', new THREE.BufferAttribute(cobData.uvs, 2));
        }
        geo.computeVertexNormals();
        this.cachedGeometries[url] = geo;
        return geo;
    }

    async loadPTHPath(url) {
        if (this.cachedPaths[url]) {
            return this.cachedPaths[url];
        }
        const resp = await fetch(url);
        const arrayBuf = await resp.arrayBuffer();
        const pthData = PTHParser.parse(arrayBuf);
        this.cachedPaths[url] = pthData;
        return pthData;
    }

    initUI() {
        this.playBtn = document.getElementById('play-btn');
        this.modeBtn = document.getElementById('mode-btn');
        this.freeCamBtn = document.getElementById('freecam-btn');
        this.muteBtn = document.getElementById('mute-btn');
        this.progressBar = document.getElementById('progress-bar');
        this.progressFilled = document.getElementById('progress-filled');
        this.timeDisplay = document.getElementById('time-display');
        this.sceneTitle = document.getElementById('scene-title');
        this.visualizerCanvas = document.getElementById('visualizer');
        this.visCtx = this.visualizerCanvas.getContext('2d');

        // Play/Pause
        this.playBtn.addEventListener('click', () => this.togglePlay());

        // Mode toggle (Modern vs Retro)
        this.modeBtn.addEventListener('click', () => {
            this.mode = this.mode === 'modern' ? 'retro' : 'modern';
            this.modeBtn.innerHTML = this.mode === 'modern' ? '⚡ Modern' : '🕹️ Retro';
            this.container.classList.toggle('retro-mode', this.mode === 'retro');
            if (this.sceneManager) {
                this.sceneManager.setMode(this.mode);
            }
        });

        // Free-Cam toggle
        this.freeCamBtn.addEventListener('click', () => this.toggleFreeCam());

        // Mute toggle
        this.muteBtn.addEventListener('click', () => {
            const muted = this.audioPlayer.toggleMute();
            this.muteBtn.innerHTML = muted ? '🔇' : '🔊';
        });

        // Progress bar scrubbing
        this.progressBar.addEventListener('click', (e) => {
            const rect = this.progressBar.getBoundingClientRect();
            const pos = (e.clientX - rect.left) / rect.width;
            const targetTime = pos * this.duration;
            this.seek(targetTime);
        });

        // Scene jump buttons
        document.querySelectorAll('.scene-jump-btn').forEach(btn => {
            btn.addEventListener('click', () => {
                const targetTime = parseFloat(btn.dataset.time);
                this.seek(targetTime);
            });
        });
    }

    initControls() {
        this.moveState = { f: 0, b: 0, l: 0, r: 0, u: 0, d: 0 };
        this.isMouseDown = false;
        this.lastMouseX = 0;
        this.lastMouseY = 0;
        this.camYaw = 0;
        this.camPitch = 0;

        window.addEventListener('keydown', (e) => {
            if (e.code === 'Space') {
                e.preventDefault();
                this.toggleFreeCam();
            } else if (e.code === 'KeyK') {
                this.togglePlay();
            }
            if (this.isFreeCam) {
                if (e.code === 'KeyW') this.moveState.f = 1;
                if (e.code === 'KeyS') this.moveState.b = 1;
                if (e.code === 'KeyA') this.moveState.l = 1;
                if (e.code === 'KeyD') this.moveState.r = 1;
                if (e.code === 'KeyE') this.moveState.u = 1;
                if (e.code === 'KeyQ') this.moveState.d = 1;
            }
        });

        window.addEventListener('keyup', (e) => {
            if (e.code === 'KeyW') this.moveState.f = 0;
            if (e.code === 'KeyS') this.moveState.b = 0;
            if (e.code === 'KeyA') this.moveState.l = 0;
            if (e.code === 'KeyD') this.moveState.r = 0;
            if (e.code === 'KeyE') this.moveState.u = 0;
            if (e.code === 'KeyQ') this.moveState.d = 0;
        });

        this.container.addEventListener('mousedown', (e) => {
            this.isMouseDown = true;
            this.lastMouseX = e.clientX;
            this.lastMouseY = e.clientY;
        });

        window.addEventListener('mouseup', () => {
            this.isMouseDown = false;
        });

        window.addEventListener('mousemove', (e) => {
            if (this.isFreeCam && this.isMouseDown) {
                const deltaX = e.clientX - this.lastMouseX;
                const deltaY = e.clientY - this.lastMouseY;
                this.lastMouseX = e.clientX;
                this.lastMouseY = e.clientY;

                this.camYaw -= deltaX * 0.003;
                this.camPitch -= deltaY * 0.003;
                this.camPitch = Math.max(-Math.PI / 2.2, Math.min(Math.PI / 2.2, this.camPitch));

                this.camera.rotation.order = 'YXZ';
                this.camera.rotation.y = this.camYaw;
                this.camera.rotation.x = this.camPitch;
            }
        });
    }

    togglePlay() {
        this.isPlaying = !this.isPlaying;
        this.playBtn.innerHTML = this.isPlaying ? '⏸' : '▶';
        if (this.isPlaying) {
            this.audioPlayer.play();
        } else {
            this.audioPlayer.pause();
        }
    }

    toggleFreeCam() {
        this.isFreeCam = !this.isFreeCam;
        this.freeCamBtn.classList.toggle('active', this.isFreeCam);
        this.freeCamBtn.innerHTML = this.isFreeCam ? '🎥 Free-Cam (ON)' : '🎥 Auto-Spline';
    }

    seek(time) {
        this.currentTime = Math.max(0, Math.min(this.duration, time));
        this.audioPlayer.seek(this.currentTime);
        this.updateTimeUI();
    }

    onSceneChange(seg) {
        if (this.sceneTitle) {
            this.sceneTitle.innerHTML = `<span>Scene:</span> ${seg.name}`;
        }
    }

    onResize() {
        this.camera.aspect = window.innerWidth / window.innerHeight;
        this.camera.updateProjectionMatrix();
        this.renderer.setSize(window.innerWidth, window.innerHeight);
    }

    updateTimeUI() {
        const format = (t) => {
            const m = Math.floor(t / 60);
            const s = Math.floor(t % 60);
            return `${m.toString().padStart(2, '0')}:${s.toString().padStart(2, '0')}`;
        };
        this.timeDisplay.textContent = `${format(this.currentTime)} / ${format(this.duration)}`;
        const percent = (this.currentTime / this.duration) * 100;
        this.progressFilled.style.width = `${percent}%`;
    }

    drawVisualizer() {
        if (!this.visCtx || !this.audioPlayer) return;
        const freq = this.audioPlayer.getFrequencyData();
        const w = this.visualizerCanvas.width;
        const h = this.visualizerCanvas.height;

        this.visCtx.clearRect(0, 0, w, h);
        const barWidth = w / 32;

        for (let i = 0; i < 32; i++) {
            const val = freq[i] / 255.0;
            const barH = val * h;

            this.visCtx.fillStyle = this.mode === 'modern' ? '#00ffee' : '#00cc66';
            this.visCtx.fillRect(i * barWidth, h - barH, barWidth - 1, barH);
        }
    }

    animate() {
        requestAnimationFrame(() => this.animate());

        const delta = this.clock.getDelta();

        if (this.isPlaying) {
            this.currentTime += delta;
            if (this.currentTime >= this.duration) {
                this.currentTime = 0;
                this.seek(0);
            }
            this.updateTimeUI();
        }

        // Free-Cam movement
        if (this.isFreeCam) {
            const speed = 25 * delta;
            const dir = new THREE.Vector3();
            this.camera.getWorldDirection(dir);
            const side = new THREE.Vector3().crossVectors(dir, this.camera.up).normalize();

            if (this.moveState.f) this.camera.position.addScaledVector(dir, speed);
            if (this.moveState.b) this.camera.position.addScaledVector(dir, -speed);
            if (this.moveState.r) this.camera.position.addScaledVector(side, speed);
            if (this.moveState.l) this.camera.position.addScaledVector(side, -speed);
            if (this.moveState.u) this.camera.position.y += speed;
            if (this.moveState.d) this.camera.position.y -= speed;
        }

        if (this.sceneManager) {
            this.sceneManager.update(this.currentTime, delta);
        }

        this.drawVisualizer();
        this.renderer.render(this.scene, this.camera);
    }
}

// Start application
window.addEventListener('DOMContentLoaded', () => {
    new App();
});

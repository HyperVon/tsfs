/**
 * Web Audio Tracker & Soundtrack Engine
 * Plays the authentic music with real-time frequency spectrum analysis for 3D reactivity.
 */

export class XMPlayer {
    constructor() {
        this.ctx = null;
        this.analyser = null;
        this.gainNode = null;
        this.audioElement = null;
        this.sourceNode = null;
        this.isPlaying = false;
        this.freqData = new Uint8Array(64);
        this.voiceBuffers = {};
        this.isMuted = false;
    }

    init() {
        if (!this.ctx) {
            const AudioContextClass = window.AudioContext || window.webkitAudioContext;
            this.ctx = new AudioContextClass();

            this.analyser = this.ctx.createAnalyser();
            this.analyser.fftSize = 128;
            this.analyser.smoothingTimeConstant = 0.8;

            this.gainNode = this.ctx.createGain();
            this.gainNode.gain.value = 0.85;

            this.analyser.connect(this.gainNode);
            this.gainNode.connect(this.ctx.destination);
        }
        if (this.ctx.state === 'suspended') {
            this.ctx.resume();
        }
    }

    /**
     * Load soundtrack audio file (from converted full demo stream or tracker export)
     */
    async loadTrack(audioUrl) {
        this.init();

        if (!this.audioElement) {
            this.audioElement = new Audio();
            this.audioElement.crossOrigin = 'anonymous';
            this.audioElement.src = audioUrl;

            this.sourceNode = this.ctx.createMediaElementSource(this.audioElement);
            this.sourceNode.connect(this.analyser);
        } else {
            this.audioElement.src = audioUrl;
        }

        return new Promise((resolve, reject) => {
            this.audioElement.oncanplay = () => resolve();
            this.audioElement.onerror = (e) => reject(e);
            this.audioElement.load();
        });
    }

    play() {
        this.init();
        if (this.audioElement) {
            this.audioElement.play();
            this.isPlaying = true;
        }
    }

    pause() {
        if (this.audioElement) {
            this.audioElement.pause();
            this.isPlaying = false;
        }
    }

    seek(timeSeconds) {
        if (this.audioElement) {
            this.audioElement.currentTime = timeSeconds;
        }
    }

    getCurrentTime() {
        return this.audioElement ? this.audioElement.currentTime : 0;
    }

    getDuration() {
        return this.audioElement ? this.audioElement.duration || 279 : 279;
    }

    setVolume(val) {
        if (this.gainNode) {
            this.gainNode.gain.value = val;
        }
    }

    toggleMute() {
        this.isMuted = !this.isMuted;
        if (this.gainNode) {
            this.gainNode.gain.value = this.isMuted ? 0 : 0.85;
        }
        return this.isMuted;
    }

    /**
     * Get live FFT frequency spectrum data (0.0 to 1.0 per band)
     */
    getFrequencyData() {
        if (this.analyser) {
            this.analyser.getByteFrequencyData(this.freqData);
            return this.freqData;
        }
        return new Uint8Array(64);
    }

    /**
     * Get aggregate bass energy (0.0 to 1.0)
     */
    getBassEnergy() {
        if (!this.analyser) return 0;
        this.analyser.getByteFrequencyData(this.freqData);
        let sum = 0;
        for (let i = 0; i < 8; i++) {
            sum += this.freqData[i];
        }
        return sum / (8 * 255.0);
    }

    /**
     * Trigger a 3D spatialized voice clip
     * @param {string} wavUrl
     * @param {number} pan (-1.0 to 1.0)
     * @param {number} volume (0.0 to 1.0)
     */
    async playSpatialVoice(wavUrl, pan = 0, volume = 1.0) {
        this.init();
        try {
            if (!this.voiceBuffers[wavUrl]) {
                const resp = await fetch(wavUrl);
                const arrayBuffer = await resp.arrayBuffer();
                this.voiceBuffers[wavUrl] = await this.ctx.decodeAudioData(arrayBuffer);
            }

            const bufferSource = this.ctx.createBufferSource();
            bufferSource.buffer = this.voiceBuffers[wavUrl];

            const voiceGain = this.ctx.createGain();
            voiceGain.gain.value = Math.max(0, Math.min(1, volume));

            if (this.ctx.createStereoPanner) {
                const panner = this.ctx.createStereoPanner();
                panner.pan.value = Math.max(-1, Math.min(1, pan));
                bufferSource.connect(voiceGain);
                voiceGain.connect(panner);
                panner.connect(this.gainNode);
            } else {
                bufferSource.connect(voiceGain);
                voiceGain.connect(this.gainNode);
            }

            bufferSource.start();
        } catch (e) {
            console.warn('Voice playback skipped:', e);
        }
    }
}

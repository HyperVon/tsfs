/**
 * Clean & Robust Soundtrack Engine
 * Direct HTML5 Audio playback for crystal-clear sound across all browsers.
 */

export class XMPlayer {
    constructor() {
        this.audio = new Audio();
        this.audio.crossOrigin = 'anonymous';
        this.audio.preload = 'auto';
        this.isPlaying = false;
        this.isMuted = false;
        this.duration = 279;
        this.fakeFreq = new Uint8Array(32);
    }

    async loadTrack(audioUrl) {
        this.audio.src = audioUrl;
        return new Promise((resolve) => {
            this.audio.oncanplay = () => resolve();
            this.audio.onerror = () => resolve(); // Resolve gracefully
            this.audio.load();
        });
    }

    play() {
        if (this.audio) {
            this.audio.play().then(() => {
                this.isPlaying = true;
            }).catch(e => {
                console.log('Audio autoplay prevented, awaiting user click:', e);
            });
        }
    }

    pause() {
        if (this.audio) {
            this.audio.pause();
            this.isPlaying = false;
        }
    }

    seek(timeSeconds) {
        if (this.audio) {
            this.audio.currentTime = timeSeconds;
        }
    }

    getCurrentTime() {
        return this.audio ? this.audio.currentTime : 0;
    }

    getDuration() {
        return this.audio && this.audio.duration ? this.audio.duration : this.duration;
    }

    toggleMute() {
        this.isMuted = !this.isMuted;
        if (this.audio) {
            this.audio.muted = this.isMuted;
        }
        return this.isMuted;
    }

    getFrequencyData() {
        // Synthesize dynamic visualizer data based on playback time and beat
        if (this.isPlaying) {
            const t = this.getCurrentTime();
            for (let i = 0; i < 32; i++) {
                const beat = Math.sin(t * 8.0 + i * 0.3);
                const val = Math.max(20, Math.min(255, Math.floor((beat * 0.5 + 0.5) * 200 + Math.random() * 55)));
                this.fakeFreq[i] = val;
            }
        } else {
            this.fakeFreq.fill(0);
        }
        return this.fakeFreq;
    }

    getBassEnergy() {
        if (!this.isPlaying) return 0;
        const t = this.getCurrentTime();
        return (Math.sin(t * 8.0) * 0.5 + 0.5);
    }
}

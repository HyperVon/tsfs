import { IntroScene } from './IntroScene.js';
import { GridScene } from './GridScene.js';
import { TorusScene } from './TorusScene.js';
import { JourneyScene } from './JourneyScene.js';
import { VideoCubeScene } from './VideoCubeScene.js';
import { OutroScene } from './OutroScene.js';
import { SplineEngine } from '../core/SplineEngine.js';

/**
 * Master Scene Director & Timeline Coordinator
 * 4m 39s (279 seconds) matching the exact original 1997 demoparty timeline.
 */
export class SceneManager {
    constructor(app) {
        this.app = app;
        this.scenes = [];
        this.currentSceneIndex = -1;
        this.splines = {};

        // Timeline segments (timestamps in seconds)
        this.timeline = [
            { id: 'intro', name: 'Chaotic Order 3D Logo', start: 0, end: 50, sceneIdx: 0, pth: 'assets/paths/COINTRO.PTH' },
            { id: 'grid', name: 'Waving Box Matrix', start: 50, end: 115, sceneIdx: 1, pth: 'assets/paths/SCENE1A.PTH' },
            { id: 'torus', name: 'Multi-Axis 3D Toruses', start: 115, end: 185, sceneIdx: 2, pth: 'assets/paths/SCENE2.PTH' },
            { id: 'journey', name: 'The Search (Road Signs)', start: 185, end: 230, sceneIdx: 3, pth: 'assets/paths/SCENE3.PTH' },
            { id: 'videocube', name: 'The 3D Video Cube', start: 230, end: 265, sceneIdx: 4, pth: 'assets/paths/SCENE4.PTH' },
            { id: 'outro', name: 'Steve Tribute & Credits', start: 265, end: 279, sceneIdx: 5, pth: null }
        ];
    }

    async init() {
        // Instantiate all scene controllers
        this.scenes = [
            new IntroScene(this.app),
            new GridScene(this.app),
            new TorusScene(this.app),
            new JourneyScene(this.app),
            new VideoCubeScene(this.app),
            new OutroScene(this.app)
        ];

        for (const s of this.scenes) {
            await s.init();
            s.group.visible = false;
            this.app.scene.add(s.group);
        }

        // Load camera spline path files
        for (const seg of this.timeline) {
            if (seg.pth) {
                try {
                    const pthData = await this.app.loadPTHPath(seg.pth);
                    if (pthData && pthData.points.length > 0) {
                        this.splines[seg.id] = new SplineEngine(pthData.points);
                    }
                } catch (e) {
                    console.warn(`Could not load spline path ${seg.pth}:`, e);
                }
            }
        }
    }

    setMode(mode) {
        this.scenes.forEach(s => s.setMode(mode));
    }

    update(currentTime, delta) {
        // Determine active timeline segment
        let activeSeg = this.timeline.find(s => currentTime >= s.start && currentTime < s.end);
        if (!activeSeg) {
            activeSeg = this.timeline[this.timeline.length - 1];
        }

        const newIdx = activeSeg.sceneIdx;
        if (newIdx !== this.currentSceneIndex) {
            // Scene switch
            if (this.currentSceneIndex >= 0 && this.scenes[this.currentSceneIndex]) {
                this.scenes[this.currentSceneIndex].group.visible = false;
            }
            this.currentSceneIndex = newIdx;
            if (this.scenes[newIdx]) {
                this.scenes[newIdx].group.visible = true;
            }
            this.app.onSceneChange(activeSeg);
        }

        const activeScene = this.scenes[this.currentSceneIndex];
        const bassEnergy = this.app.audioPlayer ? this.app.audioPlayer.getBassEnergy() : 0;
        const freqData = this.app.audioPlayer ? this.app.audioPlayer.getFrequencyData() : null;

        if (activeScene) {
            activeScene.update(delta, currentTime, bassEnergy, freqData);
        }

        // Update Camera Position via Spline if NOT in Free-Cam mode
        if (!this.app.isFreeCam) {
            const spline = this.splines[activeSeg.id];
            if (spline) {
                const segDuration = activeSeg.end - activeSeg.start;
                const segProgress = (currentTime - activeSeg.start) / segDuration;
                const camState = spline.evaluate(segProgress);

                this.app.camera.position.set(camState.viewpoint.x, camState.viewpoint.y, camState.viewpoint.z);
                this.app.camera.lookAt(camState.lookat.x, camState.lookat.y, camState.lookat.z);
            } else {
                // Default orbit camera fallback for scenes without paths
                const angle = currentTime * 0.4;
                this.app.camera.position.set(Math.cos(angle) * 20, 4, Math.sin(angle) * 20);
                this.app.camera.lookAt(0, 0, 0);
            }
        }
    }
}

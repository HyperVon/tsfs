/**
 * Kochanek-Bartels (TCB) & Cubic Hermite Spline Engine
 * Exact mathematical port of Chaotic Order's 1997 spline_1.c & spline_2.c
 * Provides smooth, cinematic camera paths with Ease-in/Ease-out velocity profiles.
 */

export class SplineEngine {
    constructor(points, tension = 0, continuity = 0, bias = 0) {
        this.points = points;
        this.tension = tension;
        this.continuity = continuity;
        this.bias = bias;
        this.numKeys = points.length;

        // Build keys for viewpoint and lookat
        this.viewKeys = [];
        this.lookKeys = [];

        for (let i = 0; i < this.numKeys; i++) {
            const pt = points[i];
            this.viewKeys.push({
                frame: i,
                pos: { x: pt.viewpoint.x, y: pt.viewpoint.y, z: pt.viewpoint.z },
                ds: { x: 0, y: 0, z: 0 },
                dd: { x: 0, y: 0, z: 0 },
                tens: tension,
                cont: continuity,
                bias: bias
            });

            this.lookKeys.push({
                frame: i,
                pos: { x: pt.lookat.x, y: pt.lookat.y, z: pt.lookat.z },
                ds: { x: 0, y: 0, z: 0 },
                dd: { x: 0, y: 0, z: 0 },
                tens: tension,
                cont: continuity,
                bias: bias
            });
        }

        if (this.numKeys >= 2) {
            this.computeDerivatives(this.viewKeys);
            this.computeDerivatives(this.lookKeys);
        }
    }

    /**
     * Compute Hermite ease factor
     */
    static ease(t, a = 0, b = 0) {
        const s = a + b;
        if (s === 0.0) return t;
        if (s > 1.0) {
            a = a / s;
            b = b / s;
        }
        const k = 1.0 / (2.0 - a - b);
        if (t < a) {
            return (k / a) * t * t;
        } else if (t < 1.0 - b) {
            return k * (2 * t - a);
        } else {
            const invT = 1.0 - t;
            return 1.0 - (k / b) * invT * invT;
        }
    }

    computeDerivatives(keys) {
        const n = keys.length;
        if (n < 2) return;

        for (let i = 1; i < n - 1; i++) {
            this.compDeriv(keys[i - 1], keys[i], keys[i + 1]);
        }

        // First and Last key derivatives
        if (n >= 3) {
            this.compDerivFirst(keys[0], keys[1], keys[2]);
            this.compDerivLast(keys[n - 3], keys[n - 2], keys[n - 1]);
        } else {
            // Linear fallbacks for 2 points
            const dx = keys[1].pos.x - keys[0].pos.x;
            const dy = keys[1].pos.y - keys[0].pos.y;
            const dz = keys[1].pos.z - keys[0].pos.z;
            keys[0].dd = { x: dx, y: dy, z: dz };
            keys[1].ds = { x: dx, y: dy, z: dz };
        }
    }

    compDeriv(keyp, key, keyn) {
        const dt = 0.5 * (keyn.frame - keyp.frame);
        let fp = (key.frame - keyp.frame) / dt;
        let fn = (keyn.frame - key.frame) / dt;
        const c = Math.abs(key.cont);
        fp = fp + c - c * fp;
        fn = fn + c - c * fn;

        const cm = 1.0 - key.cont;
        const tm = 0.5 * (1.0 - key.tens);
        const cp = 2.0 - cm;
        const bm = 1.0 - key.bias;
        const bp = 2.0 - bm;

        const tmcm = tm * cm;
        const tmcp = tm * cp;

        const ksm = tmcm * bp * fp;
        const ksp = tmcp * bm * fp;
        const kdm = tmcp * bp * fn;
        const kdp = tmcm * bm * fn;

        ['x', 'y', 'z'].forEach(axis => {
            const delm = key.pos[axis] - keyp.pos[axis];
            const delp = keyn.pos[axis] - key.pos[axis];
            key.ds[axis] = ksm * delm + ksp * delp;
            key.dd[axis] = kdm * delm + kdp * delp;
        });
    }

    compDerivFirst(key, keyn, keynn) {
        const f20 = keynn.frame - key.frame;
        const f10 = keyn.frame - key.frame;
        ['x', 'y', 'z'].forEach(axis => {
            const v20 = keynn.pos[axis] - key.pos[axis];
            const v10 = keyn.pos[axis] - key.pos[axis];
            key.dd[axis] = (1 - key.tens) * (v20 * (0.25 - f10 / (2 * f20)) + (v10 - v20 / 2) * 1.5 + v20 / 2);
        });
    }

    compDerivLast(keypp, keyp, key) {
        const f20 = key.frame - keypp.frame;
        const f10 = key.frame - keyp.frame;
        ['x', 'y', 'z'].forEach(axis => {
            const v20 = key.pos[axis] - keypp.pos[axis];
            const v10 = key.pos[axis] - keyp.pos[axis];
            key.ds[axis] = (1 - key.tens) * (v20 * (0.25 - f10 / (2 * f20)) + (v10 - v20 / 2) * 1.5 + v20 / 2);
        });
    }

    /**
     * Interpolate a Hermite curve at normalized progress t (0.0 to 1.0) across the whole path
     * @param {number} progress (0.0 - 1.0)
     * @returns {{ viewpoint: {x:number, y:number, z:number}, lookat: {x:number, y:number, z:number} }}
     */
    evaluate(progress) {
        if (this.numKeys === 0) {
            return {
                viewpoint: { x: 0, y: 0, z: -20 },
                lookat: { x: 0, y: 0, z: 0 }
            };
        }
        if (this.numKeys === 1) {
            return {
                viewpoint: this.points[0].viewpoint,
                lookat: this.points[0].lookat
            };
        }

        const clamped = Math.max(0, Math.min(0.9999, progress));
        const totalSegments = this.numKeys - 1;
        const segmentProgress = clamped * totalSegments;
        const segIdx = Math.floor(segmentProgress);
        const t = segmentProgress - segIdx;

        const vp = this.interpolateHermite(this.viewKeys[segIdx], this.viewKeys[segIdx + 1], t);
        const lp = this.interpolateHermite(this.lookKeys[segIdx], this.lookKeys[segIdx + 1], t);

        return { viewpoint: vp, lookat: lp };
    }

    interpolateHermite(k0, k1, t) {
        const t2 = t * t;
        const t3 = t2 * t;

        // Hermite blending polynomials
        const h1 = 2 * t3 - 3 * t2 + 1;
        const h2 = -2 * t3 + 3 * t2;
        const h3 = t3 - 2 * t2 + t;
        const h4 = t3 - t2;

        return {
            x: h1 * k0.pos.x + h2 * k1.pos.x + h3 * k0.dd.x + h4 * k1.ds.x,
            y: h1 * k0.pos.y + h2 * k1.pos.y + h3 * k0.dd.y + h4 * k1.ds.y,
            z: h1 * k0.pos.z + h2 * k1.pos.z + h3 * k0.dd.z + h4 * k1.ds.z
        };
    }
}

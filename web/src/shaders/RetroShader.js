/**
 * Custom 1997 CRT & 256-Color VGA Palette Shaders
 */

export const RetroShader = {
    uniforms: {
        tDiffuse: { value: null },
        time: { value: 0 },
        resolution: { value: null },
        scanlineIntensity: { value: 0.28 },
        curvature: { value: 0.08 },
        posterizeSteps: { value: 32.0 } // 256 color feel
    },

    vertexShader: `
        varying vec2 vUv;
        void main() {
            vUv = uv;
            gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
        }
    `,

    fragmentShader: `
        uniform sampler2D tDiffuse;
        uniform float time;
        uniform vec2 resolution;
        uniform float scanlineIntensity;
        uniform float curvature;
        uniform float posterizeSteps;
        varying vec2 vUv;

        vec2 curve(vec2 uv) {
            uv = (uv - 0.5) * 2.0;
            uv *= 1.0 + pow((abs(uv.yx) / 4.0), vec2(2.0)) * curvature;
            uv = (uv / 2.0) + 0.5;
            return uv;
        }

        void main() {
            vec2 uv = curve(vUv);

            // Screen border cutoff for curved CRT
            if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
                gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
                return;
            }

            // RGB color aberration / phosphor shift
            float r = texture2D(tDiffuse, uv + vec2(0.0015, 0.0)).r;
            float g = texture2D(tDiffuse, uv).g;
            float b = texture2D(tDiffuse, uv - vec2(0.0015, 0.0)).b;
            vec3 color = vec3(r, g, b);

            // 256-color palette quantization (VGA Mode 13h simulation)
            color = floor(color * posterizeSteps) / posterizeSteps;

            // CRT Scanlines
            float scanline = sin(uv.y * resolution.y * 1.5) * scanlineIntensity;
            color -= scanline;

            // Subtle vignette
            float vignette = uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y);
            color *= clamp(pow(16.0 * vignette, 0.25), 0.0, 1.0);

            gl_FragColor = vec4(color, 1.0);
        }
    `
};

#version 150

uniform vec3 lightView;
uniform vec4 baseColor;
uniform float bands;
uniform float ambient;
uniform float outline;
uniform float useTexture;
uniform float useVertexColor;
uniform float hasNormals;
uniform sampler2D colorTex;

in vec3 viewNormal;
in vec3 viewPosition;
in vec4 vertexColor;
in vec2 texCoordVarying;

out vec4 outputColor;

void main() {
    float shade = 1.0;
    if (hasNormals > 0.5) {
        float len2 = dot(viewNormal, viewNormal);
        if (len2 > 0.000001) {
            vec3 n = viewNormal * inversesqrt(len2);
            vec3 l = normalize(lightView - viewPosition);
            float ndotl = clamp(dot(n, l), 0.0, 1.0);
            float steps = max(floor(bands + 0.5), 2.0);
            float band = min(floor(ndotl * steps), steps - 1.0);
            shade = mix(clamp(ambient, 0.0, 1.0), 1.0, band / (steps - 1.0));

            if (outline > 0.001) {
                float facing = clamp(dot(n, normalize(-viewPosition)), 0.0, 1.0);
                shade *= smoothstep(outline, outline + 0.08, facing);
            }
        }
    }

    vec4 albedo = baseColor;
    if (useTexture > 0.5) albedo *= texture(colorTex, texCoordVarying);
    if (useVertexColor > 0.5) albedo *= vertexColor;
    outputColor = vec4(albedo.rgb * shade, albedo.a);
}

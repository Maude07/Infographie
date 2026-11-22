#version 150

const int MAX_LIGHTS = 8;      
const int LIGHT_AMBIENT = 0;       
const int LIGHT_DIRECTIONAL = 1;
const int LIGHT_POINT = 2;
const int LIGHT_SPOT = 3;

uniform int lightCount;
uniform int lightType[MAX_LIGHTS];
uniform vec3 lightPosition[MAX_LIGHTS];
uniform vec3 lightDirection[MAX_LIGHTS];
uniform vec3 lightColor[MAX_LIGHTS];
uniform vec3 lightAttenuation[MAX_LIGHTS];
uniform float lightSpotCosCutoff[MAX_LIGHTS];
uniform float lightSpotExponent[MAX_LIGHTS];

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

float quantize(float intensity, float steps) {
    float band = min(floor(clamp(intensity, 0.0, 1.0) * steps), steps - 1.0);
    return band / (steps - 1.0);
}


void main() {
    vec3 lighting = vec3(1.0);
    if (hasNormals > 0.5) {
        float len2 = dot(viewNormal, viewNormal);
        if (len2 > 0.000001) {
            vec3 n = viewNormal * inversesqrt(len2);
            float steps = max(floor(bands + 0.5), 2.0);
            float floorLevel = clamp(ambient, 0.0, 1.0);
            lighting = vec3(floorLevel);

            for (int i = 0; i < MAX_LIGHTS; ++i) {
                if (i >= lightCount) break;

                if (lightType[i] == LIGHT_AMBIENT) {
                    lighting += lightColor[i];
                    continue;
                }

                vec3 l;
                float attenuation = 1.0;
                if (lightType[i] == LIGHT_DIRECTIONAL) {
                    l = -lightDirection[i];
                } else {
                    vec3 toLight = lightPosition[i] - viewPosition;
                    float d = length(toLight);
                    l = toLight / max(d, 0.0001);
                    vec3 k = lightAttenuation[i];
                    attenuation = 1.0 / max(k.x + k.y * d + k.z * d * d, 0.0001);

                    if (lightType[i] == LIGHT_SPOT) {
                        float cosAngle = dot(-l, lightDirection[i]);
                        attenuation *= cosAngle >= lightSpotCosCutoff[i]
                            ? pow(max(cosAngle, 0.0), lightSpotExponent[i])
                            : 0.0;
                    }
                }

                float intensity = max(dot(n, l), 0.0) * attenuation;
                lighting += (1.0 - floorLevel) * quantize(intensity, steps) * lightColor[i];
            }
            lighting = min(lighting, vec3(1.0));

            if (outline > 0.001) {
                float facing = clamp(dot(n, normalize(-viewPosition)), 0.0, 1.0);
                lighting *= smoothstep(outline, outline + 0.08, facing);
            }
        }
    }

    vec4 albedo = baseColor;
    if (useTexture > 0.5) albedo *= texture(colorTex, texCoordVarying);
    if (useVertexColor > 0.5) albedo *= vertexColor;
    outputColor = vec4(albedo.rgb * lighting, albedo.a);
}

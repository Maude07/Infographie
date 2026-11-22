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
uniform vec3 specularColor;
uniform float shininess;
uniform float useTexture;
uniform float hasNormals;
uniform sampler2D colorTex;
uniform sampler2D normalTex;

in vec3 viewNormal;
in vec3 viewPosition;
in vec2 texCoordVarying;

out vec4 outputColor;

mat3 cotangentFrame(vec3 n, vec3 p, vec2 uv) {
    vec3 dp1 = dFdx(p);
    vec3 dp2 = dFdy(p);
    vec2 duv1 = dFdx(uv);
    vec2 duv2 = dFdy(uv);

    vec3 dp2perp = cross(dp2, n);
    vec3 dp1perp = cross(n, dp1);
    vec3 tangent = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 bitangent = dp2perp * duv1.y + dp1perp * duv2.y;

    float invmax = inversesqrt(max(dot(tangent, tangent), dot(bitangent, bitangent)));
    return mat3(tangent * invmax, bitangent * invmax, n);
}

void main() {
    vec3 nGeo = viewNormal;
    if (hasNormals < 0.5 || dot(nGeo, nGeo) < 0.000001) {
        nGeo = cross(dFdx(viewPosition), dFdy(viewPosition));
    }
    float nLen2 = dot(nGeo, nGeo);
    vec3 n = nLen2 > 0.000001 ? nGeo * inversesqrt(nLen2) : vec3(0.0, 0.0, 1.0);

    vec2 duv1 = dFdx(texCoordVarying);
    vec2 duv2 = dFdy(texCoordVarying);
    float uvArea = duv1.x * duv2.y - duv1.y * duv2.x;
    if (abs(uvArea) > 0.00000001) {
        vec3 mapNormal = texture(normalTex, texCoordVarying).xyz * 2.0 - 1.0;
        n = normalize(cotangentFrame(n, viewPosition, texCoordVarying) * mapNormal);
    }

    vec3 diffuseLight = vec3(0.0);
    vec3 specLight = vec3(0.0);
    vec3 viewDir = normalize(-viewPosition);
    float gloss = max(shininess, 1.0);

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (i >= lightCount) break;

        if (lightType[i] == LIGHT_AMBIENT) {
            diffuseLight += lightColor[i];
            continue;
        }

        vec3 lightDir;
        float attenuation = 1.0;
        if (lightType[i] == LIGHT_DIRECTIONAL) {
            lightDir = -lightDirection[i];
        } else {
            vec3 toLight = lightPosition[i] - viewPosition;
            float distance = length(toLight);
            lightDir = toLight / max(distance, 0.0001);
            vec3 k = lightAttenuation[i];
            attenuation = 1.0 / max(k.x + k.y * distance + k.z * distance * distance, 0.0001);

            if (lightType[i] == LIGHT_SPOT) {
                float cosAngle = dot(-lightDir, lightDirection[i]);
                attenuation *= cosAngle >= lightSpotCosCutoff[i]
                    ? pow(max(cosAngle, 0.0), lightSpotExponent[i])
                    : 0.0;
            }
        }

        float ndotl = max(dot(n, lightDir), 0.0) * attenuation;
        diffuseLight += ndotl * lightColor[i];

        if (ndotl > 0.0) {
            vec3 halfDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(n, halfDir), 0.0), gloss) * attenuation;
            specLight += spec * lightColor[i];
        }
    }

    vec4 albedo = baseColor;
    if (useTexture > 0.5) albedo *= texture(colorTex, texCoordVarying);
    vec3 color = albedo.rgb * diffuseLight + specularColor * specLight;
    outputColor = vec4(color, albedo.a);
}

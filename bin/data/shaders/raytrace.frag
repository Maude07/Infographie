#version 150

const int MAX_OBJECTS = 16;
const int MAX_LIGHTS = 8;
const int SHAPE_SPHERE = 0;
const int SHAPE_BOX = 1;
const int LIGHT_AMBIENT = 0;
const int LIGHT_DIRECTIONAL = 1;
const int LIGHT_POINT = 2;
const int LIGHT_SPOT = 3;

const int NO_OBJECT = -1;
const int FLOOR = MAX_OBJECTS;
const float NO_HIT = 1e20;
const float SURFACE_OFFSET = 0.5;

uniform mat4 inverseViewProjection;
uniform vec2 viewportSize;

uniform int objectCount;
uniform int objectType[MAX_OBJECTS];
uniform mat4 objectWorldToLocal[MAX_OBJECTS];
uniform vec3 objectColor[MAX_OBJECTS];

uniform int floorEnabled;
uniform float floorY;
uniform int shadowsEnabled;

uniform int lightCount;
uniform int lightType[MAX_LIGHTS];
uniform vec3 lightPosition[MAX_LIGHTS];
uniform vec3 lightDirection[MAX_LIGHTS];
uniform vec3 lightColor[MAX_LIGHTS];
uniform vec3 lightAttenuation[MAX_LIGHTS];
uniform float lightSpotCosCutoff[MAX_LIGHTS];
uniform float lightSpotExponent[MAX_LIGHTS];

out vec4 outputColor;

struct Hit {
    float t;
    vec3 normal;
    int id;
};

float intersectSphere(vec3 ro, vec3 rd, out vec3 normal) {
    float a = dot(rd, rd);
    float b = dot(ro, rd);
    float c = dot(ro, ro) - 0.25;
    float discriminant = b * b - a * c;
    if (discriminant < 0.0) return NO_HIT;

    float root = sqrt(discriminant);
    float t = (-b - root) / a;
    if (t < 0.0) t = (-b + root) / a;
    if (t < 0.0) return NO_HIT;

    normal = ro + rd * t;
    return t;
}

float intersectBox(vec3 ro, vec3 rd, out vec3 normal) {
    vec3 inverseDirection = 1.0 / rd;
    vec3 t0 = (-0.5 - ro) * inverseDirection;
    vec3 t1 = ( 0.5 - ro) * inverseDirection;
    vec3 tNear = min(t0, t1);
    vec3 tFar = max(t0, t1);
    float tEnter = max(max(tNear.x, tNear.y), tNear.z);
    float tExit = min(min(tFar.x, tFar.y), tFar.z);
    if (tEnter > tExit || tExit < 0.0) return NO_HIT;

    float t = tEnter > 0.0 ? tEnter : tExit;
    vec3 p = abs(ro + rd * t);
    vec3 s = sign(ro + rd * t);
    if (p.x > p.y && p.x > p.z) normal = vec3(s.x, 0.0, 0.0);
    else if (p.y > p.z)         normal = vec3(0.0, s.y, 0.0);
    else                        normal = vec3(0.0, 0.0, s.z);
    return t;
}

Hit traceScene(vec3 ro, vec3 rd, float maxT) {
    Hit hit;
    hit.t = maxT;
    hit.id = NO_OBJECT;
    hit.normal = vec3(0.0);

    for (int i = 0; i < MAX_OBJECTS; ++i) {
        if (i >= objectCount) break;

        vec3 localOrigin = (objectWorldToLocal[i] * vec4(ro, 1.0)).xyz;
        vec3 localDirection = mat3(objectWorldToLocal[i]) * rd;

        vec3 localNormal;
        float t = objectType[i] == SHAPE_SPHERE
            ? intersectSphere(localOrigin, localDirection, localNormal)
            : intersectBox(localOrigin, localDirection, localNormal);

        if (t < hit.t) {
            hit.t = t;
            hit.id = i;
            hit.normal = normalize(transpose(mat3(objectWorldToLocal[i])) * localNormal);
        }
    }

    if (floorEnabled == 1 && abs(rd.y) > 1e-6) {
        float t = (floorY - ro.y) / rd.y;
        if (t > 0.0 && t < hit.t) {
            hit.t = t;
            hit.id = FLOOR;
            hit.normal = vec3(0.0, -1.0, 0.0);
        }
    }
    return hit;
}

vec3 floorColor(vec3 p) {
    float checker = mod(floor(p.x / 50.0) + floor(p.z / 50.0), 2.0);
    return mix(vec3(0.55), vec3(0.75), checker);
}

vec3 shade(vec3 p, vec3 n, vec3 viewDirection, vec3 albedo) {
    vec3 color = vec3(0.0);
    vec3 shadowOrigin = p + n * SURFACE_OFFSET;

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (i >= lightCount) break;

        if (lightType[i] == LIGHT_AMBIENT) {
            color += albedo * lightColor[i];
            continue;
        }

        vec3 l;
        float distanceToLight;
        float attenuation = 1.0;
        if (lightType[i] == LIGHT_DIRECTIONAL) {
            l = -lightDirection[i];
            distanceToLight = NO_HIT;
        } else {
            vec3 toLight = lightPosition[i] - p;
            distanceToLight = length(toLight);
            l = toLight / max(distanceToLight, 0.0001);
            vec3 k = lightAttenuation[i];
            attenuation = 1.0 / max(k.x + k.y * distanceToLight + k.z * distanceToLight * distanceToLight, 0.0001);

            if (lightType[i] == LIGHT_SPOT) {
                float cosAngle = dot(-l, lightDirection[i]);
                attenuation *= cosAngle >= lightSpotCosCutoff[i]
                    ? pow(max(cosAngle, 0.0), lightSpotExponent[i])
                    : 0.0;
            }
        }

        float diffuse = max(dot(n, l), 0.0);
        if (diffuse <= 0.0 || attenuation <= 0.0) continue;

        if (shadowsEnabled == 1 && traceScene(shadowOrigin, l, distanceToLight).id != NO_OBJECT) continue;

        vec3 halfway = normalize(l + viewDirection);
        float specular = 0.3 * pow(max(dot(n, halfway), 0.0), 32.0);
        color += (albedo * diffuse + specular) * lightColor[i] * attenuation;
    }
    return min(color, vec3(1.0));
}

void main() {
    vec2 ndc = gl_FragCoord.xy / viewportSize * 2.0 - 1.0;
    vec4 nearPoint = inverseViewProjection * vec4(ndc, -1.0, 1.0);
    vec4 farPoint = inverseViewProjection * vec4(ndc, 1.0, 1.0);
    vec3 ro = nearPoint.xyz / nearPoint.w;
    vec3 rd = normalize(farPoint.xyz /farPoint.w - ro);

    Hit hit = traceScene(ro, rd, NO_HIT);
    if (hit.id == NO_OBJECT) discard;

    vec3 p = ro + rd * hit.t;
    vec3 n = dot(hit.normal, rd) > 0.0 ? -hit.normal : hit.normal;

    vec3 albedo;
    if (hit.id == FLOOR) albedo = floorColor(p);
    else albedo = objectColor[hit.id];

    outputColor = vec4(shade(p, n, -rd, albedo), 1.0);
}

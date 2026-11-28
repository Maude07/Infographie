#version 150

const int MAX_STEPS = 128;
const float MAX_DISTANCE = 200.0;
const float HIT_EPSILON = 0.001;

uniform mat4 inverseViewProjection;
uniform vec2 viewportSize;
uniform vec3 center;
uniform float size;
uniform float blend;
uniform float time;

out vec4 outputColor;

float sdSphere(vec3 p, float radius) {
    return length(p) - radius;
}

float sdTorus(vec3 p, vec2 radii) {
    vec2 q = vec2(length(p.xz) - radii.x, p.y);
    return length(q) - radii.y;
}

float sdRoundBox(vec3 p, vec3 halfSize, float rounding) {
    vec3 q = abs(p) - halfSize + rounding;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0) - rounding;
}

float smoothUnion(float a, float b, float k) {
    if (k <= 0.0) return min(a, b);
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return mix(b, a, h) - k * h * (1.0 - h);
}

mat2 rotation(float angle) {
    float c = cos(angle), s = sin(angle);
    return mat2(c, -s, s, c);
}

vec3 shapeDistances(vec3 p) {
    vec3 spherePoint = p - vec3(-1.6, 0.0, 0.0);

    vec3 torusPoint = p;
    torusPoint.yz = rotation(time * 0.8) * torusPoint.yz;

    vec3 boxPoint = p - vec3(1.6, 0.0, 0.0);
    boxPoint.xz = rotation(time * 0.5) * boxPoint.xz;

    return vec3(
        sdSphere(spherePoint, 0.6),
        sdTorus(torusPoint, vec2(0.6, 0.2)),
        sdRoundBox(boxPoint, vec3(0.5), 0.1));
}

float sceneDistance(vec3 p) {
    vec3 d = shapeDistances(p);
    return smoothUnion(smoothUnion(d.x, d.y, blend), d.z, blend);
}

vec3 shapeColor(vec3 p) {
    vec3 weights = exp(-8.0 * max(shapeDistances(p), 0.0));
    vec3 color = weights.x * vec3(0.90, 0.30, 0.30)
            + weights.y * vec3(0.20, 0.70, 0.70)
            + weights.z * vec3(0.95, 0.80, 0.25);
    return color / (weights.x + weights.y + weights.z);
}

vec3 sceneNormal(vec3 p) {
    const vec2 e = vec2(0.001, 0.0);
    return normalize(vec3(
        sceneDistance(p + e.xyy) - sceneDistance(p - e.xyy),
        sceneDistance(p + e.yxy) - sceneDistance(p - e.yxy),
        sceneDistance(p + e.yyx) - sceneDistance(p - e.yyx)));
}

float softShadow(vec3 ro, vec3 rd) {
    float result = 1.0;
    float t = 0.02;
    for (int i = 0; i < 64; ++i) {
        float h = sceneDistance(ro + rd * t);
        if (h < HIT_EPSILON) return 0.0;
        result = min(result, 8.0 * h / t);
        t += clamp(h, 0.01, 0.5);
        if (t > 20.0) break;
    }
    return clamp(result, 0.0, 1.0);
}

void main() {
    vec2 ndc = gl_FragCoord.xy / viewportSize * 2.0 - 1.0;
    vec4 nearPoint = inverseViewProjection * vec4(ndc, -1.0, 1.0);
    vec4 farPoint = inverseViewProjection * vec4(ndc, 1.0, 1.0);
    vec3 ro = nearPoint.xyz / nearPoint.w;
    vec3 rd = normalize(farPoint.xyz / farPoint.w - ro);

    vec3 origin = (ro - center) / size;

    float t = 0.0;
    bool hit = false;
    for (int i = 0; i < MAX_STEPS; ++i) {
        float d = sceneDistance(origin + rd * t);
        if (d < HIT_EPSILON) { hit = true; break; }
        t += d;
        if (t > MAX_DISTANCE) break;
    }

    if (!hit) discard;
    
    vec3 p = origin + rd * t;
    vec3 n = sceneNormal(p);
    vec3 l = normalize(vec3(-0.5, -1.0, 0.6));

    float shadow = softShadow(p + n * 0.01, l);
    float diffuse = max(dot(n, l), 0.0) * shadow;
    float specular = pow(max(dot(n, normalize(l - rd)), 0.0), 32.0) * shadow;

    vec3 color = shapeColor(p) * (0.15 + 0.85 * diffuse) + 0.3 * specular;
    outputColor = vec4(min(color, vec3(1.0)), 1.0);
}

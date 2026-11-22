#version 150

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;

in vec4 position;
in vec3 normal;
in vec2 texcoord;

out vec3 viewNormal;
out vec3 viewPosition;
out vec2 texCoordVarying;

void main() {
    vec4 viewPos = modelViewMatrix * position;
    mat3 normalMatrix = transpose(inverse(mat3(modelViewMatrix)));
    viewNormal = normalMatrix * normal;
    viewPosition = viewPos.xyz;
    texCoordVarying = texcoord;
    gl_Position = modelViewProjectionMatrix * position;
}

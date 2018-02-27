#version 330 core

in vec3 inPos;
in vec3 inNormal;
in vec2 texCoords;

uniform mat4 VP;
uniform mat4 normalMatrix;

out vec3 pos;
out vec2 UV;
out vec3 normal;

void main() {
    pos = inPos;
    UV = texCoords;
    normal = inNormal;
    // normal = (normalMatrix * vec4(inNormal, 0)).xyz;

    gl_Position = VP * vec4(inPos, 1);
}

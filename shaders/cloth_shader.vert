#version 330 core

in vec3 inPos;
in vec2 texCoords;

uniform mat4 VP;

out vec3 pos;
out vec2 UV;
// out vec3 normal;

void main() {
    pos = inPos;
    UV = texCoords;
    // normal = (transpose(inverse(model)) * vec4(normals, 0)).xyz;

    gl_Position = VP * vec4(inPos, 1);
}

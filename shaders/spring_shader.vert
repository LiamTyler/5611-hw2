#version 330 core

in vec3 verts;

uniform mat4 model;
uniform mat4 VP;

out vec3 pos;
// out vec3 normal;

void main() {
    pos = (model * vec4(verts, 1)).xyz;
    // normal = (transpose(inverse(model)) * vec4(normals, 0)).xyz;

    gl_Position = VP * model * vec4(verts, 1);
}

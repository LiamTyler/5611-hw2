#version 330 core

in vec3 verts;

uniform mat4 model;
uniform mat4 VP;

out vec2 UV;

void main() {
    gl_Position = VP * model * vec4(verts, 1);

    UV = verts.xy + vec2(.5, .5);
}

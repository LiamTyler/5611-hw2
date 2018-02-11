#version 330 core

in vec3 pos;

uniform vec4 color;

out vec4 finalColor;

void main() {
    finalColor = color;
}

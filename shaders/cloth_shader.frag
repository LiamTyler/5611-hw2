#version 330 core

in vec3 pos;
in vec2 UV;
// in vec3 normal;

uniform sampler2D tex;

out vec4 color;

void main() {
    vec3 outColor = vec3(0, 0, 0);

    outColor += texture(tex, UV).xyz;

    color = vec4(outColor, 1);
}

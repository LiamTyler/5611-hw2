#version 330 core

in vec3 pos;
in vec3 normal;
in vec2 UV;

uniform sampler2D tex;
uniform bool textured;

out vec4 color;

const float ambient = .1;
const vec3 lightDir = normalize(vec3(1, 1, 1));
// const vec3 lightDir = normalize(vec3(.5, .1, 1));

void main() {
    vec3 outColor = vec3(0, 0, 0);
    vec3 objColor;
    if (textured) {
        objColor = texture(tex, UV).xyz;
        color = vec4(objColor, 1);
    } else {
        objColor = vec3(0, 0, 1);
    }

    vec3 n = normalize(normal);
    outColor += ambient * objColor;
    outColor += max(0.0, dot(lightDir, n)) * objColor;

    color = vec4(outColor, 1);
}

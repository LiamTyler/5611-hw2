#version 330 core

in vec3 pos;
in vec2 UV;
in vec3 normal;

uniform sampler2D tex;

out vec4 color;

const float ambient = .1;
const vec3 lightDir = normalize(vec3(0,1,1));
// const vec3 lightDir = normalize(vec3(0, 1, 0));

void main() {
    vec3 outColor = vec3(0, 0, 0);
    vec3 n = normalize(normal);
    vec3 objColor = texture(tex, UV).xyz;

    outColor += ambient * objColor;
    outColor += max(0.0, dot(lightDir, n)) * objColor;
    // outColor = objColor;

    color = vec4(outColor, 1);
}

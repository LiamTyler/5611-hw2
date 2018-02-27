#version 330 core

in vec3 verts;
in vec3 normals;
in vec2 texCoords;

uniform mat4 model;
uniform mat4 normalMatrix;
uniform mat4 VP;

out vec3 pos;
out vec3 normal;
out vec2 UV;

void main() {
    pos = (model * vec4(verts, 1)).xyz;
    normal = normalize((transpose(inverse(model)) * vec4(normals, 0)).xyz);
    // normal = normalize((normalMatrix * vec4(normals, 0)).xyz);
    UV = texCoords;

    gl_Position = VP * model * vec4(verts, 1);
}

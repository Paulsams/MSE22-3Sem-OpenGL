#version 330 core

in vec3 frag_position;
in vec3 vert_normal;
in vec2 vert_tex;

uniform sampler2D duffuse_tex;
uniform vec3 duffuse_color;

layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_diffuse;
layout (location = 2) out vec3 out_normal;

void main() {
    vec3 texturePixel = vec3(texture(duffuse_tex, vert_tex).xyz);
    vec3 objectColor = texturePixel * duffuse_color;

    out_position = frag_position;
    out_diffuse = objectColor;
    out_normal = normalize(vert_normal);
}

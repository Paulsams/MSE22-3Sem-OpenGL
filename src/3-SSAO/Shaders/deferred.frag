#version 330 core

in vec3 frag_position;
in vec3 vert_normal;
in vec2 vert_tex;

uniform sampler2D duffuse_tex;
uniform vec3 duffuse_color;

layout (location = 0) out vec4 out_position;
layout (location = 1) out vec4 out_diffuse;
layout (location = 2) out vec3 out_normal;
layout (location = 3) out vec3 out_texCoord;

void main() {
    vec3 texturePixel = vec3(texture(duffuse_tex, vert_tex).xyz);
    vec3 objectColor = texturePixel * duffuse_color;

    out_position = vec4(frag_position, 1.0);
    out_diffuse = vec4(objectColor, 1.0);
    out_normal = normalize(vert_normal);
    out_texCoord = vec3(vert_tex, 0.0);
}

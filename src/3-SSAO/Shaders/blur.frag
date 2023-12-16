#version 330 core

const float Offsets[4] = float[]( -1.5, -0.5, 0.5, 1.5 );

in vec2 vert_tex_coord;

uniform sampler2D g_input_buffer;
uniform int count_points;
uniform float stride_points;

out vec4 out_color;

float getOffsetFromIndex(int i) {
    return (i + 0.5f - count_points / 2) * stride_points;
}

void main() {
    vec3 Color = vec3(0.0);

    for (int i = 0 ; i < count_points; ++i) {
        for (int j = 0 ; j < count_points; ++j) {
            vec2 coordinates = vert_tex_coord;
            coordinates.x = vert_tex_coord.x + getOffsetFromIndex(j) / textureSize(g_input_buffer, 0).x;
            coordinates.y = vert_tex_coord.y + getOffsetFromIndex(i) / textureSize(g_input_buffer, 0).y;
            Color += texture(g_input_buffer, coordinates).xyz;
        }
    }

    Color /= count_points * count_points;

    out_color = vec4(Color, 1.0);
}

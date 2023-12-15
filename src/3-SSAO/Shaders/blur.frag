#version 330 core

const float Offsets[4] = float[]( -1.5, -0.5, 0.5, 1.5 );

in vec2 vert_tex_coord;

uniform sampler2D g_input_buffer;

out vec4 out_color;

void main()
{
    vec3 Color = vec3(0.0);

    for (int i = 0 ; i < 4; i++) {
        for (int j = 0 ; j < 4; j++) {
            vec2 tc = vert_tex_coord;
            tc.x = vert_tex_coord.x + Offsets[j] / textureSize(g_input_buffer, 0).x;
            tc.y = vert_tex_coord.y + Offsets[i] / textureSize(g_input_buffer, 0).y;
            Color += texture(g_input_buffer, tc).xyz;
        }
    }

    Color /= 16.0;

    out_color = vec4(Color, 1.0);
}

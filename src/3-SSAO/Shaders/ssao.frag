#version 330 core

const int MAX_KERNEL_SIZE = 128;

in vec2 vert_tex_coord;

uniform sampler2D g_buffer_position;
uniform float sample_radius;
uniform mat4 projection;
uniform int kernel_points_count;
uniform vec3 kernel_points[MAX_KERNEL_SIZE];

layout (location = 0) out vec4 out_color;

void main()
{
    vec3 position = texture(g_buffer_position, vert_tex_coord).xyz;

    float count_ao = 0.0;

    for (int i = 0 ; i < kernel_points_count; ++i) {
        vec3 samplePosition = position + kernel_points[i]; // генерируем случайные точки
        vec4 offset = vec4(samplePosition, 1.0);     // преобразуем в 4-вектор
        offset = projection * offset;                // проецируем на ближнюю плоскость клиппера
        offset.xy /= offset.w;                       // производим деление перспективы
        offset.xy = offset.xy * 0.5 + vec2(0.5);     // переносим на отрезок (0,1)

        float sampleDepth = texture(g_buffer_position, offset.xy).b;

        if (abs(position.z - sampleDepth) < sample_radius)
            count_ao += step(sampleDepth, samplePosition.z);
    }

    count_ao = 1.0 - count_ao / MAX_KERNEL_SIZE;

    out_color = vec4(pow(count_ao, 2.0));
}

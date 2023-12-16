#version 330 core

const int MAX_KERNEL_SIZE = 128;

in vec2 vert_tex_coord;
in vec2 vert_view_ray;

uniform sampler2D depth_buffer;
uniform float sample_radius;
uniform mat4 projection;
uniform int kernel_points_count;
uniform vec3 kernel_points[MAX_KERNEL_SIZE];

layout (location = 0) out vec4 out_color;

float calculateViewZ(vec2 coordinates)
{
    float depth = texture(depth_buffer, coordinates).x;
    return projection[3][2] / (2 * depth - 1 + projection[2][2]);
}

void main()
{
    float viewZ = calculateViewZ(vert_tex_coord);

    float viewX = vert_view_ray.x * viewZ;
    float viewY = vert_view_ray.y * viewZ;

    vec3 position = vec3(-viewX, -viewY, viewZ);

    float count_ao = 0.0;

    for (int i = 0 ; i < kernel_points_count; ++i) {
        vec3 samplePosition = position + kernel_points[i]; // генерируем случайные точки
        vec4 offset = vec4(samplePosition, 1.0);           // преобразуем в 4-вектор
        offset = projection * offset;                      // проецируем на ближнюю плоскость клиппера
        offset.xy /= offset.w;                             // производим деление перспективы
        offset.xy = offset.xy * 0.5 + vec2(0.5);           // переносим на отрезок (0,1)

        float sampleDepth = calculateViewZ(offset.xy);
        if (abs(position.z - sampleDepth) < sample_radius)
            count_ao += step(sampleDepth, samplePosition.z);
    }

    count_ao = 1.0 - count_ao / MAX_KERNEL_SIZE;

    out_color = vec4(pow(count_ao, 2.0));
}

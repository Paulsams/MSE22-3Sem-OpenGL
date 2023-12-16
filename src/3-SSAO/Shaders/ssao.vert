#version 330 core

uniform float aspect_ratio;
uniform float tan_half_FoV;

out vec2 vert_tex_coord;
out vec2 vert_view_ray;

// https://stackoverflow.com/a/59738005
void main() {
    const vec2 positions[4] = vec2[](
        vec2(-1, -1),
        vec2(+1, -1),
        vec2(-1, +1),
        vec2(+1, +1)
    );
    const vec2 coords[4] = vec2[](
        vec2(0, 0),
        vec2(1, 0),
        vec2(0, 1),
        vec2(1, 1)
    );

    gl_Position = vec4(positions[gl_VertexID], 0, 1);
    vert_tex_coord = coords[gl_VertexID];

    vert_view_ray.x = positions[gl_VertexID].x * aspect_ratio * tan_half_FoV;
    vert_view_ray.y = positions[gl_VertexID].y * tan_half_FoV;
}

#version 330 core

out vec2 vert_tex_coord; // texcoords are in the normalized [0,1] range for the viewport-filling quad part of the triangle

// https://stackoverflow.com/a/59739538
void main() {
    vec2 vertices[3] = vec2[3](vec2(-1, -1), vec2(3, -1), vec2(-1, 3));
    gl_Position = vec4(vertices[gl_VertexID], 0, 1);
    vert_tex_coord = 0.5 * gl_Position.xy + vec2(0.5);
}

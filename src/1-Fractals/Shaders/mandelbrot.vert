#version 330 core

layout (location = 0) in vec2 pos;

uniform mat4 matrix;

out vec2 vert_pos;

void main() {
    vert_pos = pos;
    gl_Position = matrix * vec4(pos.xy, 0.0, 1.0);
}

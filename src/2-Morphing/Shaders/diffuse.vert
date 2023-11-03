#version 330 core

layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

uniform mat4 mvp;

out vec3 vert_col;
out vec2 vert_tex;

void main() {
	vert_col = vec3(0.5f, 0.5f, 0.5f);
	vert_tex = in_texcoord;
	gl_Position = mvp * vec4(in_vertex, 1);
}

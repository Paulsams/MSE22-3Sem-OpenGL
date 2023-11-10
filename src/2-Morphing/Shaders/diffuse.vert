#version 330 core

layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

uniform mat4 mvp;
uniform mat4 model;

out vec2 vert_tex;
out vec3 vert_normal;
out vec3 frag_position;

void main() {
	gl_Position = mvp * vec4(in_vertex, 1.0f);
	frag_position = vec3(model * vec4(in_vertex, 1.0f));
	vert_normal = mat3(transpose(inverse(model))) * in_normal;
	vert_tex = in_texcoord;
}

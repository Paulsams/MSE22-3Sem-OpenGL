#version 330 core

layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

uniform mat4 mvp;
uniform mat4 model;
uniform mat3 transpose_inverse_model;

out vec3 frag_position;
out vec3 vert_normal;
out vec2 vert_tex;

void main() {
	gl_Position = mvp * vec4(in_vertex, 1.0);

	frag_position = (model * vec4(in_vertex, 1.0f)).xyz;
	vert_normal = transpose_inverse_model * in_normal;
	vert_tex = in_texcoord;
}

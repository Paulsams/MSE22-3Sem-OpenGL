#version 330 core

layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

uniform float time;

uniform mat4 mvp;
uniform mat4 model;
uniform mat3 transpose_inverse_model;

out vec2 vert_tex;
out vec3 vert_normal;
out vec3 frag_position;

const float PI = 3.14159;
const float TWO_PI = PI * 2.0;

uniform bool is_morphing;
uniform float radius;
uniform float blend;

vec3 sphere(vec2 domain)
{
	vec3 range;
	range.x = radius * cos(domain.y) * sin(domain.x);
	range.y = radius * sin(domain.y) * sin(domain.x);
	range.z = radius * cos(domain.x);
	return range;
}

void main() {
	vec3 normal;
	vec3 vertex;
	if (is_morphing) {
		vec2 p0 = in_vertex.xy * TWO_PI;
		normal = sphere(p0);
		vec3 r0 = radius * normal;
		vertex = r0;

		normal = mix(in_normal, normal, blend);
		vertex = mix(in_vertex.xyz, vertex, blend);
	} else {
		normal = in_normal;
		vertex = in_vertex;
	}

	gl_Position = mvp * vec4(vertex, 1.0f);
	frag_position = vec3(model * vec4(vertex, 1.0f));
	vert_normal = transpose_inverse_model * normal;
	vert_tex = in_texcoord;
}

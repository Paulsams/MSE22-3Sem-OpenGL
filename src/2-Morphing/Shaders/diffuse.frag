#version 330 core

uniform float ambient_strength;
uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 view_position;
uniform float specular_strength;
uniform int power_specular;
//uniform vec3 object_color;

in vec2 vert_tex;
in vec3 vert_normal;
in vec3 frag_position;

out vec4 out_col;

void main() {
    vec3 object_color = vec3(1.0f, 1.0f, 1.0f);

    vec3 normal = normalize(vert_normal);
    vec3 light_dir = normalize(light_position - frag_position);
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    vec3 view_dir = normalize(view_position - frag_position);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(
        max(dot(view_dir, reflect_dir), 0.0),
        power_specular);
    vec3 specular = specular_strength * spec * light_color;

    vec3 ambient = ambient_strength * light_color;

    vec3 result = specular + diffuse + ambient * object_color;
    out_col = vec4(result, 1.0f);
}
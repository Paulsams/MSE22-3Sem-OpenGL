#version 330 core

#define MAX_NUM_TOTAL_LIGHTS 2

struct PointLight
{
    vec3 position;
    float distance;

    vec3 ambient;
//    float constant;

    vec3 diffuse;
//    float linear;

    vec3 specular;
//    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
};

layout (std140) uniform PointsLights {
    PointLight points[MAX_NUM_TOTAL_LIGHTS];
    int numPointsLights;
};

layout (std140) uniform DirectionalLight {
    vec3 direction;
    vec3 color;
    float ambient_strength;
} directional;

//uniform SpotLight
//{
//    vec3 light_color;
//    vec3 light_position;
//    float ambient_strength;
//} spots[16];

uniform vec3 view_position;
uniform float specular_strength;
uniform int power_specular;

in vec2 vert_tex;
in vec3 vert_normal;
in vec3 frag_position;

out vec4 out_color;

vec3 calculatePhong(vec3 normal, vec3 lightDirection, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    vec3 reflect_dir = reflect(-lightDirection, normal);

    float difference = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = difference * diffuseColor;

    float spec = pow(max(dot(-lightDirection, reflect_dir), 0.0), power_specular);
    vec3 specular = specularColor * spec;

    return ambientColor + diffuse/* + specular*/;
}

void main() {
    vec3 object_color = vec3(1.0f, 1.0f, 1.0f);

    vec3 normal = normalize(vert_normal);
    vec3 view_dir = normalize(view_position - frag_position);

    vec3 result;
    //    #pragma unroll numPointsLights
    for (int i = 0; i < 0; i++) {
        vec3 vector_to_light = points[i].position - frag_position;

        if (length(vector_to_light) <= points[i].distance)
            continue;

        vec3 light_dir = normalize(vector_to_light);
        result += calculatePhong(normal, light_dir, points[i].ambient, points[i].diffuse, points[i].specular);
    }

    result += calculatePhong(normal, directional.direction, vec3(1, 1, 1) * directional.ambient_strength,
                             directional.color, directional.color);

    result *= object_color;
    out_color = vec4(result, 1.0);
}

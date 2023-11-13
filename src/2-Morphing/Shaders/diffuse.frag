#version 330 core

#define MAX_NUM_TOTAL_LIGHTS 10

struct PointLight
{
    vec3 position;
    float distance;

    vec3 ambient;
    float constant;

    vec3 diffuse;
    float linear;

    vec3 specular;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    float distance;

    vec3 direction;
    float innerOutOffInRadians;

    vec3 ambient;
    float outerCutOffInRadians;

    vec3 diffuse;

    vec3 specular;
};

layout (std140) uniform DirectionalLight {
    vec3 direction;
    vec3 color;
    float ambient_strength;
} directional;

layout (std140) uniform PointLights {
    PointLight points[MAX_NUM_TOTAL_LIGHTS];
    int numPointsLights;
};

layout (std140) uniform SpotLights
{
    SpotLight spots[MAX_NUM_TOTAL_LIGHTS];
    int numSpotsLights;
};

uniform sampler2D duffuse_tex;
uniform vec3 duffuse_color;

uniform vec3 view_position;
uniform int power_specular;

in vec2 vert_tex;
in vec3 vert_normal;
in vec3 frag_position;

out vec4 out_color;

vec3 calculatePhong(vec3 normal, vec3 lightDirection, vec3 viewDirection, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    vec3 reflect_dir = reflect(-lightDirection, normal);

    float difference = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = difference * diffuseColor;

    float spec = pow(max(dot(viewDirection, reflect_dir), 0.0), power_specular);
    vec3 specular = specularColor * spec;

    return ambientColor + diffuse + specular;
}

void main() {
    vec3 texturePixel = vec3(texture(duffuse_tex, vert_tex).xyz);
    vec3 objectColor = (texturePixel == vec3(0.0f) ? vec3(1.0f) : texturePixel) * duffuse_color;

    vec3 normal = normalize(vert_normal);
    vec3 viewDirection = normalize(view_position - frag_position);

    vec3 result;
    for (int i = 0; i < numPointsLights; ++i) {
        PointLight pointLight = points[i];
        vec3 vectorToLight = pointLight.position - frag_position;
        float distance = length(vectorToLight);

        if (distance > pointLight.distance)
            continue;

        vec3 light_dir = normalize(vectorToLight);
        float attenuation =
            pointLight.constant
            + pointLight.linear * distance
            + pointLight.quadratic * (distance * distance);

        result += calculatePhong(normal, light_dir, viewDirection,
            pointLight.ambient, pointLight.diffuse, pointLight.specular) * (1.0f / attenuation);
    }

    for (int i = 0; i < numSpotsLights; ++i) {
        SpotLight light = spots[i];
        vec3 vectorToLight = light.position - frag_position;
        // TODO: сделать дистанцию
//        float distance = length(vectorToLight);

//        if (distance > light.distance)
//            continue;

        vec3 light_dir = normalize(vectorToLight);
        float theta = dot(light_dir, normalize(-light.direction));
        float epsilon = light.innerOutOffInRadians - light.outerCutOffInRadians;
        float intensity = clamp((theta - light.outerCutOffInRadians) / epsilon, 0.0, 1.0);

        result += calculatePhong(normal, light_dir, viewDirection,
            light.ambient, light.diffuse * intensity, light.specular * intensity);
    }

    result += calculatePhong(normal, directional.direction, viewDirection, vec3(1, 1, 1) * directional.ambient_strength,
        directional.color, directional.color);

    result *= objectColor;
    out_color = vec4(result, 1.0);
}

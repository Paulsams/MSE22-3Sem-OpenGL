#version 330 core

uniform float power_specular;
uniform vec3 view_position;
uniform vec2 screen_size;
uniform vec3 global_position;

//uniform LightInfo {
    uniform float constant;
    uniform float linear;
    uniform float quadratic;
    uniform vec3 ambient;
    uniform vec3 diffuse;
    uniform vec3 specular;
//};

uniform sampler2D g_buffer_position;
uniform sampler2D g_buffer_color;
uniform sampler2D g_buffer_normal;

out vec3 out_color;

vec3 calculatePhong(vec3 normal, vec3 lightDirection, vec3 viewDirection, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    vec3 reflect_dir = reflect(-lightDirection, normal);

    float difference = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = difference * diffuseColor;

    float spec = pow(max(dot(viewDirection, reflect_dir), 0.0), power_specular);
    vec3 specular = specularColor * spec;

    return ambientColor + diffuse + specular;
}

vec3 calcPointLight(vec3 fragPosition, vec3 normal) {
    vec3 vectorToLight = global_position - fragPosition;
    float distance = length(vectorToLight);

    vec3 lightDirection = normalize(vectorToLight);
    float attenuation =
        constant
        + linear * distance
        + quadratic * (distance * distance);

    vec3 viewDirection = normalize(view_position - fragPosition);
    return calculatePhong(normal, lightDirection, viewDirection,
        ambient, diffuse, specular) * (1.0 / attenuation);
}

void main() {
    // Calc texture coordinates
    vec2 texCoord = gl_FragCoord.xy / screen_size;

    vec3 fragPosition = texture(g_buffer_position, texCoord).xyz;
    vec3 color = texture(g_buffer_color, texCoord).xyz;
    vec3 normal = texture(g_buffer_normal, texCoord).xyz;

    out_color = color * calcPointLight(fragPosition, normal);
}

#version 330 core

uniform vec3 view_position;
uniform vec2 screen_size;

uniform sampler2D g_buffer_position;
uniform sampler2D g_buffer_color;

in vec2 vert_tex;

out vec4 out_color;

vec2 CalcTexCoord() {
    return gl_FragCoord.xy / screen_size;
}

//vec3 calculatePhong(vec3 normal, vec3 lightDirection, vec3 viewDirection, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
//    vec3 reflect_dir = reflect(-lightDirection, normal);
//
//    float difference = max(dot(normal, lightDirection), 0.0);
//    vec3 diffuse = difference * diffuseColor;
//
//    float spec = pow(max(dot(viewDirection, reflect_dir), 0.0), power_specular);
//    vec3 specular = specularColor * spec;
//
//    return ambientColor + diffuse + specular;
//}
//
//vec3 CalcPointLight(vec3 position, vec3 normal) {
//    vec3 vectorToLight = position - frag_position;
//    float distance = length(vectorToLight);
//
//    vec3 lightDirection = normalize(vectorToLight);
//    float attenuation =
//        light.constant
//        + light.linear * distance
//        + light.quadratic * (distance * distance);
//
//    return calculatePhong(normal, lightDirection, viewDirection,
//        light.ambient, light.diffuse, light.specular) * (1.0f / attenuation);
//}

void main() {
    vec2 texCoord = CalcTexCoord();
    vec4 susa = texture(g_buffer_position, texCoord);
    out_color = texture(g_buffer_color, texCoord) + vec4(0.1f);
//    vec3 WorldPos = texture(gPositionMap, texCoord).xyz;
//    vec3 Color = texture(gBuffer, texCoord).xyz;
//    vec3 Normal = texture(gNormalMap, texCoord).xyz;
//    Normal = normalize(Normal);
//
//    FragColor = Color * CalcPointLight(WorldPos, Normal);

//    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
//    vec4 gbuffer_0 = texture(tex_gbuffer_0, gl_FragCoord.xy / buffers_size);
//    vec4 gbuffer_1 = texture(tex_gbuffer_1, gl_FragCoord.xy / buffers_size);
}

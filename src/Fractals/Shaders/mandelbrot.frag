#version 330 core

uniform vec2 viewPosition;
uniform float zoom;
uniform int maxIterations;

in vec2 vert_pos;
out vec4 out_col;

void main() {
    float x0 = vert_pos.x / zoom + viewPosition.x;
    float y0 = vert_pos.y / zoom + viewPosition.y;
    float x = 0;
    float y = 0;
    float xTemp = 0;
    int iterations = 0;

    while (((x * x + y * y) <= 4) && iterations <= maxIterations) {
        xTemp = x * x - y * y + x0;
        y = 2 * x * y + y0;
        x = xTemp;
        ++iterations;
    }

    vec3 outputColor;
    if (iterations >= maxIterations) {
        outputColor = vec3(1.0, 1.0, 1.0);
    } else {
        float iterationsF = iterations;

        float r = (int)(iterationsF * sin(iterationsF)) % 256 / 255.0;
    float g = (int)(iterationsF * iterationsF) % 256 / 255.0;
    float b = iterations % 256 / 255.0;

    outputColor = vec3(r, g, b);
    }

    out_col = vec4(outputColor, 1.0);
}
#pragma once

#include "vec3.h"
#include "interval.h"

using color = vec3;

struct color32 {
    char r;
    char g;
    char b;

    color32(char r, char g, char b)
        : r(r), g(g), b(b) { }
};

inline double linear_to_gamma(double linear_component)
{
    return sqrt(linear_component);
}

inline color32 get_color_in_bytes(color pixel_color, int samples_per_pixel) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color by the number of samples.
    auto scale = 1.0 / samples_per_pixel;
    r *= scale;
    g *= scale;
    b *= scale;

    // Apply the linear to gamma transform.
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Write the translated [0,255] value of each color component.
    static const interval intensity(0.000, 0.999);
    return {
            static_cast<char>(256 * intensity.clamp(r)),
            static_cast<char>(256 * intensity.clamp(g)),
            static_cast<char>(256 * intensity.clamp(b))
    };
}

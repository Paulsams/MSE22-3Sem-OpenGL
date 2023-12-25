#pragma once

#include "vec3.h"

struct triangle3 {
    vec3 a;
    vec3 b;
    vec3 c;
    vec3 normal;

    triangle3(vec3 a, vec3 b, vec3 c, vec3 normal)
        : a(a), b(b), c(c), normal(normal) { }
};

#ifndef PROJECTION_H
#define PROJECTION_H

#include "vector.h"

typedef struct {
    float fov;
    float aspect;
    float near;
    float far;
} camera;

vec2 project(vec3 point, camera cam);

#endif
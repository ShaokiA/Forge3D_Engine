#include "projection.h"
#include <math.h>

vec2 project(vec3 v, camera cam) {

    vec2 out;

    float fov_scale = 1.0f / tanf(cam.fov * 0.5f);

    // if (v.z <= 0.1f)
    //     v.z = 0.1f;

    out.x = (v.x * fov_scale) / v.z;
    out.y = (v.y * fov_scale) / v.z;

    return out;
}
#include "vector.h"
#include "matrix.h"
#include <math.h>

vec3 vec3_add(vec3 a, vec3 b) {
    return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3 vec3_sub(vec3 a, vec3 b) {
    return (vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

vec3 vec3_scale(vec3 v, float s) {
    return (vec3){v.x * s, v.y * s, v.z * s};
}

float vec3_dot(vec3 a, vec3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

vec3 vec3_cross(vec3 a, vec3 b) {
    return (vec3){
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

float vec3_length(vec3 v) {
    return sqrtf(vec3_dot(v, v));
}

vec3 vec3_normalize(vec3 v) {
    float len = vec3_length(v);
    if (len == 0) return v;
    return vec3_scale(v, 1.0f / len);
}

vec3 vec3_transform(vec3 v, mat4 m) {

    vec3 result;

    result.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2] + m.m[0][3];
    result.y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2] + m.m[1][3];
    result.z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2] + m.m[2][3];

    return result;
}

// vec3 world_to_view(vec3 v, vec3 cam_pos) {
//     return vec3_sub(v, cam_pos);
// }
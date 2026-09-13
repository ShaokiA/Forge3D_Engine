#include "vector.h"                 // Vector types and function declarations
#include "matrix.h"                 // Matrix type mat4 er jonno
#include <math.h>                   // sqrtf() use korar jonno

vec3 vec3_add(vec3 a, vec3 b) {     // Duita 3D vector jog kore
    return (vec3){a.x + b.x, a.y + b.y, a.z + b.z}; // X, Y, Z separately jog kore result return kore
}

vec3 vec3_sub(vec3 a, vec3 b) {     // Duita 3D vector biyog kore
    return (vec3){a.x - b.x, a.y - b.y, a.z - b.z}; // X, Y, Z separately biyog kore result return kore
}

vec3 vec3_scale(vec3 v, float s) {  // Vector ke ekta number diye scale kore
    return (vec3){v.x * s, v.y * s, v.z * s}; // Prottek component ke s diye gun kore
}

float vec3_dot(vec3 a, vec3 b) {    // Duita vector er dot product calculate kore
    return a.x*b.x + a.y*b.y + a.z*b.z; // X, Y, Z gun kore jog kore scalar result dey
}

vec3 vec3_cross(vec3 a, vec3 b) {   // Duita vector er cross product calculate kore
    return (vec3){                    // Notun vec3 result return kore
        a.y*b.z - a.z*b.y,            // Result er X component
        a.z*b.x - a.x*b.z,            // Result er Y component
        a.x*b.y - a.y*b.x             // Result er Z component
    };
}

float vec3_length(vec3 v) {          // Vector er length calculate kore
    return sqrtf(vec3_dot(v, v));    // Dot product diye magnitude ber kore square root ney
}

vec3 vec3_normalize(vec3 v) {        // Vector ke unit vector banay
    float len = vec3_length(v);       // Age vector er length ber kore
    if (len == 0) return v;           // Length zero hole division avoid kore original vector return kore
    return vec3_scale(v, 1.0f / len); // Length diye divide kore normalized vector return kore
}

vec3 vec3_transform(vec3 v, mat4 m) { // Vector ke matrix diye transform kore

    vec3 result;                       // Transformation result rakhar jonno vector

    result.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2] + m.m[0][3]; // X coordinate transform kore
    result.y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2] + m.m[1][3]; // Y coordinate transform kore
    result.z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2] + m.m[2][3]; // Z coordinate transform kore

    return result;                     // Transformed vector return kore
}

// vec3 world_to_view(vec3 v, vec3 cam_pos) { // World coordinate ke view coordinate e convert korar old function
//     return vec3_sub(v, cam_pos);            // Camera position biyog kore view coordinate ber korto
// }                                          // Function er sesh
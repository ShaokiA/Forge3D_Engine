#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

typedef struct {
    float m[4][4];
} mat4;

mat4 mat4_identity(void);
mat4 mat4_mul(mat4 a, mat4 b);

mat4 mat4_translate(float x, float y, float z);
mat4 mat4_scale(float x, float y, float z);
mat4 mat4_rotate_x(float angle);
mat4 mat4_rotate_y(float angle);
mat4 mat4_rotate_z(float angle);

vec3 vec3_transform(vec3 v, mat4 m);

mat4 mat4_perspective(float fov, float aspect, float near, float far);
vec4 mat4_mul_vec4(mat4 m, vec4 v);

#endif
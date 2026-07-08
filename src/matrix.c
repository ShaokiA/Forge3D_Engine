#include "matrix.h"
#include <math.h>

mat4 mat4_identity(void) {
    mat4 m = {0};

    m.m[0][0] = 1;
    m.m[1][1] = 1;
    m.m[2][2] = 1;
    m.m[3][3] = 1;

    return m;
}

mat4 mat4_mul(mat4 a, mat4 b) {
    mat4 result = {0};

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            for (int k = 0; k < 4; k++) {
                result.m[row][col] += a.m[row][k] * b.m[k][col];
            }
        }
    }

    return result;
}

mat4 mat4_translate(float x, float y, float z) {
    mat4 m = mat4_identity();

    m.m[0][3] = x;
    m.m[1][3] = y;
    m.m[2][3] = z;

    return m;
}

mat4 mat4_scale(float x, float y, float z) {
    mat4 m = mat4_identity();

    m.m[0][0] = x;
    m.m[1][1] = y;
    m.m[2][2] = z;

    return m;
}

mat4 mat4_rotate_x(float angle) {
    mat4 m = mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);

    m.m[1][1] = c;
    m.m[1][2] = -s;
    m.m[2][1] = s;
    m.m[2][2] = c;

    return m;
}

mat4 mat4_rotate_y(float angle) {
    mat4 m = mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);

    m.m[0][0] = c;
    m.m[0][2] = s;
    m.m[2][0] = -s;
    m.m[2][2] = c;

    return m;
}

mat4 mat4_rotate_z(float angle) {
    mat4 m = mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);

    m.m[0][0] = c;
    m.m[0][1] = -s;
    m.m[1][0] = s;
    m.m[1][1] = c;

    return m;
}

mat4 mat4_perspective(float fov, float aspect, float near, float far) {

    mat4 r = {0};

    float f = 1.0f / tanf(fov * 0.5f);

    r.m[0][0] = f / aspect;
    r.m[1][1] = f;
    r.m[2][2] = (far + near) / (near - far);
    r.m[2][3] = (2 * far * near) / (near - far);
    r.m[3][2] = -1.0f;

    return r;
}

vec4 mat4_mul_vec4(mat4 m, vec4 v) {

    vec4 r;

    r.x = m.m[0][0]*v.x + m.m[0][1]*v.y + m.m[0][2]*v.z + m.m[0][3]*v.w;
    r.y = m.m[1][0]*v.x + m.m[1][1]*v.y + m.m[1][2]*v.z + m.m[1][3]*v.w;
    r.z = m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z + m.m[2][3]*v.w;
    r.w = m.m[3][0]*v.x + m.m[3][1]*v.y + m.m[3][2]*v.z + m.m[3][3]*v.w;

    return r;
}


#ifndef VECTOR_H                    // Header age include na holei(if) shudhu abar include kore define korbo
#define VECTOR_H                    // Header include korse

typedef struct {                    // 2D vector er structure
    float x;                        // X coordinate
    float y;                        // Y coordinate
} vec2;                             // struct er nam vec2

typedef struct {                    // 3D vector er structure
    float x;                        // X coordinate
    float y;                        // Y coordinate
    float z;                        // Z coordinate
} vec3;                             // struct er nam vec3

typedef struct {                    // 4D vector er structure
    float x;                        // X coordinate
    float y;                        // Y coordinate
    float z;                        // Z coordinate
    float w;                        // Homogeneous coordinate: w=0 hole position ar w=1 hole coordinate vector
                                        transformation er jonno use kora hoy
} vec4;                             // struct er nam vec4

typedef struct {                    // Vertex er structure
    float x, y, z;                  // X, Y and Z values
} vertex2d;                         // struct er nam vertex2d, eta 3D jodio name dewa hoise 2D karon 2D theke 3D te newar jonno

vec3 vec3_add(vec3 a, vec3 b);      // Duita 3D vector jog kore
vec3 vec3_sub(vec3 a, vec3 b);      // Duita 3D vector biyog kore
vec3 vec3_scale(vec3 v, float s);   // Vector ke ekta number diye gun kore
float vec3_dot(vec3 a, vec3 b);     // Dot product calculate kore
vec3 vec3_cross(vec3 a, vec3 b);    // Cross product calculate kore
float vec3_length(vec3 v);          // Vector er length calculate kore
vec3 vec3_normalize(vec3 v);        // Vector ke unit vector banay

#endif                              // Header guard sesh
#ifndef MESH_H
#define MESH_H

#include "vector.h"

#define MAX_VERTICES 5000
#define MAX_FACES 10000

typedef struct
{
    vec3 vertices[MAX_VERTICES];

    int faces[MAX_FACES][3];

    int vertex_count;

    int face_count;

} Mesh;

extern Mesh mesh;

#endif
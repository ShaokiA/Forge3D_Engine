#include "mesh_loader.h"

#include <stdio.h>

#include <string.h>

#include <stdlib.h>

#include <stdbool.h>
bool load_obj(const char *filename, Mesh *mesh)
{

    FILE *file = fopen(filename, "r");

    if (!file)
    {
        return false;
    }
    printf("Loading: %s\n", filename);
    mesh->vertex_count = 0;
    mesh->face_count = 0;

    char line[256];

    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == 'v' && line[1] == ' ')
        {
            float x;
            float y;
            float z;

            sscanf(
                line,
                "v %f %f %f",
                &x,
                &y,
                &z);

            mesh->vertices[mesh->vertex_count] =
                (vec3){x, y, z};

            mesh->vertex_count++;
        }
        if (line[0] == 'f' && line[1] == ' ')
        {
            int a;
            int b;
            int c;

            sscanf(
                line,
                "f %d %d %d",
                &a,
                &b,
                &c);

            mesh->faces[mesh->face_count][0] = a - 1;
            mesh->faces[mesh->face_count][1] = b - 1;
            mesh->faces[mesh->face_count][2] = c - 1;

            mesh->face_count++;
        }
    }

    fclose(file);
    printf("Vertices : %d\n", mesh->vertex_count);
printf("Faces    : %d\n", mesh->face_count);

    return true;
}
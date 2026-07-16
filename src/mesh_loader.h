#ifndef MESH_LOADER_H
#define MESH_LOADER_H
#include <stdbool.h>   
#include "mesh.h"

bool load_obj(const char *filename, Mesh *mesh);

#endif

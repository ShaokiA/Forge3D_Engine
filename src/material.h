#ifndef MATERIAL_H
#define MATERIAL_H

#include "vector.h"

// Material structure - defines surface properties for lighting
typedef struct {
    vec3 base_color;  // Base surface color (albedo)
} Material;

// Initialize material with default values
void material_init(Material *material);

#endif // MATERIAL_H

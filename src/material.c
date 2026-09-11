#include "material.h"

void material_init(Material *material)
{
    if (!material) return;
    
    material->base_color.x = 0.7f;
    material->base_color.y = 0.7f;
    material->base_color.z = 0.7f;
}

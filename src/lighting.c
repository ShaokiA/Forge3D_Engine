#include "lighting.h"
#include <stdio.h>

void lighting_init(LightingSystem *sys)
{
    if (!sys) return;
    
    sys->light_count = 0;
    
    // Default ambient light
    sys->ambient_light.x = 0.1f;
    sys->ambient_light.y = 0.1f;
    sys->ambient_light.z = 0.1f;
    
    for (int i = 0; i < 2; i++) {
        sys->lights[i].enabled = false;
    }
}

int lighting_add_light(LightingSystem *sys, Light light)
{
    if (!sys) return -1;
    
    if (sys->light_count >= 2) {
        printf("Warning: Cannot add more lights (max 2 reached)\n");
        return -1;
    }
    
    sys->lights[sys->light_count] = light;
    int index = sys->light_count;
    sys->light_count++;
    
    return index;
}

vec3 compute_light_contribution(Light *light, Material *mat,
                               vec3 position, vec3 normal)
{
    if (!light || !light->enabled || !mat) {
        return (vec3){0.0f, 0.0f, 0.0f};
    }
    
    vec3 light_dir = vec3_sub(light->position, position);
    light_dir = vec3_normalize(light_dir);
    
    float diffuse_factor = vec3_dot(normal, light_dir);
    if (diffuse_factor < 0.0f) {
        diffuse_factor = 0.0f;
    }
    
    vec3 result;
    result.x = light->color.x * light->intensity * diffuse_factor * mat->base_color.x;
    result.y = light->color.y * light->intensity * diffuse_factor * mat->base_color.y;
    result.z = light->color.z * light->intensity * diffuse_factor * mat->base_color.z;
    
    return result;
}

vec3 compute_lighting(LightingSystem *sys, Material *mat,
                     vec3 position, vec3 normal)
{
    if (!sys || !mat) {
        return (vec3){0.0f, 0.0f, 0.0f};
    }
    
    // Start with ambient
    vec3 result;
    result.x = sys->ambient_light.x * mat->base_color.x;
    result.y = sys->ambient_light.y * mat->base_color.y;
    result.z = sys->ambient_light.z * mat->base_color.z;
    
    // Add diffuse from each light
    for (int i = 0; i < sys->light_count; i++) {
        if (sys->lights[i].enabled) {
            vec3 contribution = compute_light_contribution(&sys->lights[i], mat, position, normal);
            result = vec3_add(result, contribution);
        }
    }
    
    return result;
}

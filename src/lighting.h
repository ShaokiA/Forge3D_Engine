#ifndef LIGHTING_H
#define LIGHTING_H

#include "vector.h"
#include "material.h"
#include <stdbool.h>

// Light structure - represents a light source in the scene
typedef struct {
    vec3 position;     // Position in 3D world space
    vec3 color;        // RGB color (0-1 range)
    float intensity;   // Brightness multiplier
    bool enabled;      // Whether this light is active
} Light;

// Lighting System - manages multiple lights and ambient lighting
typedef struct {
    Light lights[2];      // Support for 2 lights
    int light_count;      // Number of lights currently added
    vec3 ambient_light;   // Global ambient illumination
} LightingSystem;

// Initialize lighting system with default ambient
void lighting_init(LightingSystem *sys);

// Add a light source (up to 2)
int lighting_add_light(LightingSystem *sys, Light light);

// Compute simple ambient + diffuse illumination for a surface point
vec3 compute_lighting(LightingSystem *sys, Material *mat,
                     vec3 position, vec3 normal);

// Helper: compute diffuse contribution from single light
vec3 compute_light_contribution(Light *light, Material *mat,
                               vec3 position, vec3 normal);

#endif // LIGHTING_H

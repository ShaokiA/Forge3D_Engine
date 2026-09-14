#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    int width;
    int height;
    uint32_t *pixels;
} Texture;

bool texture_load(Texture *texture, const char *filename);
void texture_free(Texture *texture);

uint32_t texture_sample(const Texture *texture, float u, float v);

#endif
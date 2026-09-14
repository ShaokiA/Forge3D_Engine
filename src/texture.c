#include "texture.h"

#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

bool texture_load(Texture *texture, const char *filename)
{
    SDL_Surface *surface = IMG_Load(filename);

    if (!surface)
    {
        printf("Texture load failed: %s\n", SDL_GetError());
        return false;
    }

    texture->width = surface->w;
    texture->height = surface->h;

    texture->pixels = malloc(
        texture->width * texture->height * sizeof(uint32_t)
    );

    if (!texture->pixels)
    {
        printf("Failed to allocate texture memory!\n");
        SDL_DestroySurface(surface);
        return false;
    }

    // Convert surface to ARGB8888 so our renderer uses one consistent format
    SDL_Surface *converted = SDL_ConvertSurface(
        surface,
        SDL_PIXELFORMAT_ARGB8888
    );

    SDL_DestroySurface(surface);

    if (!converted)
    {
        printf("Failed to convert texture: %s\n", SDL_GetError());
        free(texture->pixels);
        texture->pixels = NULL;
        return false;
    }

    memcpy(
        texture->pixels,
        converted->pixels,
        texture->width * texture->height * sizeof(uint32_t)
    );

    SDL_DestroySurface(converted);

    printf("Texture loaded: %s (%dx%d)\n",
           filename,
           texture->width,
           texture->height);

    return true;
}


void texture_free(Texture *texture)
{
    if (texture->pixels)
    {
        free(texture->pixels);
        texture->pixels = NULL;
    }

    texture->width = 0;
    texture->height = 0;
}


uint32_t texture_sample(const Texture *texture, float u, float v)
{
    if (!texture || !texture->pixels)
        return 0xFFFF00FF;

    // Repeat texture coordinates
    u = u - floorf(u);
    v = v - floorf(v);

    // OBJ UV v-coordinate is normally bottom-to-top,
    // while image pixels are top-to-bottom.
    v = 1.0f - v;

    int x = (int)(u * texture->width);
    int y = (int)(v * texture->height);

    // Clamp
    if (x < 0)
        x = 0;

    if (x >= texture->width)
        x = texture->width - 1;

    if (y < 0)
        y = 0;

    if (y >= texture->height)
        y = texture->height - 1;

    return texture->pixels[y * texture->width + x];
}
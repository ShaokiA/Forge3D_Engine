#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "matrix.h"
#include "projection.h"
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

bool is_running = true;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* color_buffer_texture = NULL;

// Framebuffer
uint32_t* color_buffer = NULL;

float* z_buffer = NULL;

camera cam = {
    .fov = 3.14159f / 3.0f,  // 60 degrees
    .aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT,
    .near = 0.1f,
    .far = 1000.0f
};

vec3 cube[] = {
    {-1, -1,  1},
    { 1, -1,  1},
    { 1,  1,  1},
    {-1,  1,  1},

    {-1, -1, -1},
    { 1, -1, -1},
    { 1,  1, -1},
    {-1,  1, -1}
};

int cube_edges[][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0}, // front face
    {4, 5}, {5, 6}, {6, 7}, {7, 4}, // back face
    {0, 4}, {1, 5}, {2, 6}, {3, 7}  // connections
};

int cube_faces[][3] = {
    {0, 1, 2}, {0, 2, 3},   // front
    {1, 5, 6}, {1, 6, 2},   // right
    {5, 4, 7}, {5, 7, 6},   // back
    {4, 0, 3}, {4, 3, 7},   // left
    {3, 2, 6}, {3, 6, 7},   // top
    {4, 5, 1}, {4, 1, 0}    // bottom
};

vec3 camera_pos = {0, 0, -5};

// vec3 light_dir = {0.0f, 0.0f, -1.0f};
vec3 light_dir = {0.3f, 0.5f, -1.0f};
// vec3 light_dir = vec3_normalize((vec3){0.3f, 0.5f, -1.0f});

// Function declarations
bool initialize_window(void);
void process_input(void);
void update(void);
void render(void);
void destroy_window(void);

void clear_color_buffer(uint32_t color);
void draw_pixel(int x, int y, float z, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle(vertex2d s0, vertex2d s1, vertex2d s2, vec3 v0, vec3 v1, vec3 v2, uint32_t color);
void render_color_buffer(void);

// vec3 world_to_view(vec3 v, vec3 cam_pos);

int main(int argc, char* argv[]) {

    if (!initialize_window()) {
        return 1;
    }

    while (is_running) {

        process_input();

        update();

        render();
    }

    destroy_window();

    return 0;
}

bool initialize_window(void) {

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(
        "Forge3D Engine",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, NULL);

    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Allocate framebuffer memory
    color_buffer = (uint32_t*)malloc(
        sizeof(uint32_t) * WINDOW_WIDTH * WINDOW_HEIGHT
    );

    // Allocate depth buffer (Z-buffer)
    z_buffer = (float*)malloc(
        sizeof(float) * WINDOW_WIDTH * WINDOW_HEIGHT
    );

    // Create texture used to display framebuffer
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );

    return true;
}

void process_input(void) {

    SDL_Event event;

    while (SDL_PollEvent(&event)) {

        switch (event.type) {

            case SDL_EVENT_QUIT:
                is_running = false;
                break;

            case SDL_EVENT_KEY_DOWN:

                if (event.key.key == SDLK_ESCAPE) {
                    is_running = false;
                }

                // Add camera controls
                float speed = 0.1f;
                if (event.key.scancode == SDL_SCANCODE_W) {
                    camera_pos.z += speed;
                }
                if (event.key.scancode == SDL_SCANCODE_S) {
                    camera_pos.z -= speed;
                }
                if (event.key.scancode == SDL_SCANCODE_A) {
                    camera_pos.x -= speed;
                }
                if (event.key.scancode == SDL_SCANCODE_D) {
                    camera_pos.x += speed;
                }
                if (event.key.scancode == SDL_SCANCODE_Q) {
                    camera_pos.y -= speed;
                }
                if (event.key.scancode == SDL_SCANCODE_E) {
                    camera_pos.y += speed;
                }

                break;
        }
    }
}

void update(void) {

    clear_color_buffer(0xFF000000);

    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++) {
        z_buffer[i] = -1e9f;
    }

    float time = SDL_GetTicks() * 0.001f;

    mat4 rot_y = mat4_rotate_y(time);
    mat4 rot_x = mat4_rotate_x(time * 0.7f);
    mat4 model = mat4_mul(rot_y, rot_x);

    mat4 proj = mat4_perspective(
        cam.fov,
        cam.aspect,
        cam.near,
        cam.far
    );

    mat4 view = mat4_translate(
        -camera_pos.x,
        -camera_pos.y,
        -camera_pos.z
    );

    vec3 world_pos[8];
    vertex2d screen_pos[8];

    for (int i = 0; i < 8; i++) {

        vec4 v = (vec4){
            cube[i].x,
            cube[i].y,
            cube[i].z,
            1.0f
        };

        // MODEL
        vec4 world = mat4_mul_vec4(model, v);
        world_pos[i] = (vec3){world.x, world.y, world.z};

        // VIEW
        vec4 view_pos = mat4_mul_vec4(view, world);

        // PROJECTION
        vec4 clip = mat4_mul_vec4(proj, view_pos);

        if (clip.w != 0.0f) {
            clip.x /= clip.w;
            clip.y /= clip.w;
            clip.z /= clip.w;
        }

        screen_pos[i].x = (clip.x + 1.0f) * 0.5f * WINDOW_WIDTH;
        screen_pos[i].y = (1.0f - (clip.y + 1.0f) * 0.5f) * WINDOW_HEIGHT;
        screen_pos[i].z = clip.z;
    }

    for (int i = 0; i < 12; i++) {

        int a = cube_faces[i][0];
        int b = cube_faces[i][1];
        int c = cube_faces[i][2];

        draw_triangle(
            screen_pos[a],
            screen_pos[b],
            screen_pos[c],
            world_pos[a],
            world_pos[b],
            world_pos[c],
            0xFFAAAAAA
        );
    }
}

void render(void) {

    // Copy framebuffer into SDL texture
    render_color_buffer();

    SDL_RenderClear(renderer);

    // Draw texture to screen
    SDL_RenderTexture(
        renderer,
        color_buffer_texture,
        NULL,
        NULL
    );

    SDL_RenderPresent(renderer);
}

void destroy_window(void) {

    free(color_buffer);
    free(z_buffer);

    SDL_DestroyTexture(color_buffer_texture);

    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);

    SDL_Quit();
}

void clear_color_buffer(uint32_t color) {

    for (int y = 0; y < WINDOW_HEIGHT; y++) {

        for (int x = 0; x < WINDOW_WIDTH; x++) {

            color_buffer[(WINDOW_WIDTH * y) + x] = color;
        }
    }
}

void draw_pixel(int x, int y, float z, uint32_t color) {

    if (x < 0 || x >= WINDOW_WIDTH ||
        y < 0 || y >= WINDOW_HEIGHT) {
        return;
    }

    int index = (WINDOW_WIDTH * y) + x;

    if (z > z_buffer[index]) {
        z_buffer[index] = z;
        color_buffer[index] = color;
    }
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {

    int delta_x = x1 - x0;
    int delta_y = y1 - y0;

    int longest_side_length = abs(delta_x);

    if (abs(delta_y) > longest_side_length) {
        longest_side_length = abs(delta_y);
    }

    float x_increment = delta_x / (float)longest_side_length;
    float y_increment = delta_y / (float)longest_side_length;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= longest_side_length; i++) {

        draw_pixel(
            (int)current_x,
            (int)current_y,
            0.0f,
            color
        );

        current_x += x_increment;
        current_y += y_increment;
    }
}

void draw_triangle(vertex2d s0, vertex2d s1, vertex2d s2, vec3 v0, vec3 v1, vec3 v2, uint32_t color) {

    int min_x = fminf(s0.x, fminf(s1.x, s2.x));
    int max_x = fmaxf(s0.x, fmaxf(s1.x, s2.x));
    int min_y = fminf(s0.y, fminf(s1.y, s2.y));
    int max_y = fmaxf(s0.y, fmaxf(s1.y, s2.y));

    float area = fabsf((s1.x - s0.x)*(s2.y - s0.y) - (s1.y - s0.y)*(s2.x - s0.x));

    if (area < 0.0001f) return;

    vec3 edge1 = vec3_sub(v1, v0);
    vec3 edge2 = vec3_sub(v2, v0);

    vec3 normal = vec3_cross(edge1, edge2);
    normal = vec3_normalize(normal);

    vec3 light = vec3_normalize(light_dir);
    float intensity = fmaxf(0.2f, vec3_dot(normal, light));

    vec3 view_dir = vec3_sub(camera_pos, v0);
    view_dir = vec3_normalize(view_dir);

    if (vec3_dot(normal, view_dir) <= 0)
        return;

    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {

            float w0 = ((s1.y - s2.y)*(x - s2.x) + (s2.x - s1.x)*(y - s2.y)) / ((s1.y - s2.y)*(s0.x - s2.x) + (s2.x - s1.x)*(s0.y - s2.y));
            float w1 = ((s2.y - s0.y)*(x - s2.x) + (s0.x - s2.x)*(y - s2.y)) / ((s1.y - s2.y)*(s0.x - s2.x) + (s2.x - s1.x)*(s0.y - s2.y));
            float w2 = 1.0f - w0 - w1;

            if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {

                float alpha = w1 / area;
                float beta  = w2 / area;
                float gamma = w0 / area;

                float z = alpha * v0.z + beta * v1.z + gamma * v2.z;

                // draw_pixel(x, y, z, color);
                uint8_t shade = (uint8_t)(255 * intensity);

                uint32_t lit_color =
                    (0xFF << 24) |
                    (shade << 16) |
                    (shade << 8) |
                    (shade);

                draw_pixel(x, y, z, lit_color);
            }
        }
    }
}

void render_color_buffer(void) {

    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        WINDOW_WIDTH * sizeof(uint32_t)
    );
}
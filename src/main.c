#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "matrix.h"
#include "projection.h"
#include "camera.h"
#include "material.h"
#include "lighting.h"
#include <math.h>
#include "mesh_loader.h"
#include "mesh.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

float *z_buffer = NULL;
uint32_t *color_buffer = NULL;

bool is_running = true;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *color_buffer_texture = NULL;

camera cam = {
    .fov = 3.14159f / 3.0f,  // 60 degrees
    .aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT,
    .near = 0.1f,
    .far = 1000.0f
};

vec3 camera_pos = {0, 0, -5};

// Orbit camera
orbit_camera orbit_cam;

// Lighting
LightingSystem lighting_sys;
Material default_material;

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

int main(int argc, char *argv[])
{
    if (!initialize_window())
    {
        return 1;
    }

    // Initialize orbit camera
    vec3 target = {0.0f, 0.0f, 0.0f};
    orbit_cam = orbit_camera_create(target, 10.0f);
    
    // Setup lighting
    lighting_init(&lighting_sys);
    
    Light main_light;
    main_light.position = (vec3){5.0f, 5.0f, -5.0f};
    main_light.color = (vec3){1.0f, 1.0f, 1.0f};
    main_light.intensity = 1.0f;
    main_light.enabled = true;
    lighting_add_light(&lighting_sys, main_light);
    
    material_init(&default_material);
    
    // Load mesh
    if (argc < 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    char filename[300];
    snprintf(filename, sizeof(filename), "models/%s.obj", argv[1]);

    printf("Loading mesh: %s.obj\n", filename);
    if (!load_obj(filename, &mesh))
    {
        printf("OBJ load failed!\n");
        return 1;
    }
    
    // Compute normals if needed
    if (!mesh.has_normals)
    {
        printf("Computing normals...\n");
        mesh_compute_normals(&mesh);
    }
    
    // Validate mesh
    if (!validate_mesh(&mesh))
    {
        printf("Mesh validation failed: %s\n", last_mesh_error.message);
        mesh_free(&mesh);
        return 1;
    }

    while (is_running)
    {
        process_input();
        update();
        render();
    }

    destroy_window();
    mesh_free(&mesh);

    return 0;
}

bool initialize_window(void)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(
        "Forge3D Engine",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    if (!window)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, NULL);

    if (!renderer)
    {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    color_buffer = (uint32_t *)malloc(sizeof(uint32_t) * WINDOW_WIDTH * WINDOW_HEIGHT);
    z_buffer = (float *)malloc(sizeof(float) * WINDOW_WIDTH * WINDOW_HEIGHT);

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );

    return true;
}

void process_input(void)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            is_running = false;
            break;

        case SDL_EVENT_KEY_DOWN:
            if (event.key.key == SDLK_ESCAPE)
            {
                is_running = false;
            }

            // WASD/QE camera movement
            float speed = 0.1f;
            if (event.key.scancode == SDL_SCANCODE_W)
                camera_pos.z += speed;
            if (event.key.scancode == SDL_SCANCODE_S)
                camera_pos.z -= speed;
            if (event.key.scancode == SDL_SCANCODE_A)
                camera_pos.x -= speed;
            if (event.key.scancode == SDL_SCANCODE_D)
                camera_pos.x += speed;
            if (event.key.scancode == SDL_SCANCODE_Q)
                camera_pos.y -= speed;
            if (event.key.scancode == SDL_SCANCODE_E)
                camera_pos.y += speed;
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            orbit_camera_mouse_button(&orbit_cam, true, event.button.button == SDL_BUTTON_LEFT, window);
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            orbit_camera_mouse_button(&orbit_cam, false, event.button.button == SDL_BUTTON_LEFT, window);
            break;

        case SDL_EVENT_MOUSE_MOTION:
            orbit_camera_mouse_motion(&orbit_cam, event.motion.xrel, event.motion.yrel);
            break;

        case SDL_EVENT_MOUSE_WHEEL:
            orbit_camera_mouse_wheel(&orbit_cam, event.wheel.y);
            break;
        }
    }
}

void update(void)
{
    clear_color_buffer(0xFF000000);

    // Reset depth buffer
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++)
    {
        z_buffer[i] = -1e9f;
    }

    float time = SDL_GetTicks() * 0.001f;

    // Model rotation
    mat4 rot_y = mat4_rotate_y(time);
    mat4 rot_x = mat4_rotate_x(time * 0.7f);
    mat4 model = mat4_mul(rot_y, rot_x);

    mat4 proj = mat4_perspective(cam.fov, cam.aspect, cam.near, cam.far);
    mat4 view = orbit_camera_view_matrix(&orbit_cam);

    camera_pos = orbit_camera_position(&orbit_cam);

    // Allocate temporary arrays for transformed vertices
    vec3 *world_pos = (vec3 *)malloc(mesh.vertex_count * sizeof(vec3));
    vertex2d *screen_pos = (vertex2d *)malloc(mesh.vertex_count * sizeof(vertex2d));

    // Transform vertices
    for (int i = 0; i < mesh.vertex_count; i++)
    {
        vec4 v = (vec4){
            mesh.vertices[i].x,
            mesh.vertices[i].y,
            mesh.vertices[i].z,
            1.0f
        };

        // Model -> World
        vec4 world = mat4_mul_vec4(model, v);
        world_pos[i] = (vec3){world.x, world.y, world.z};

        // World -> View
        vec4 view_pos = mat4_mul_vec4(view, world);

        // View -> Clip
        vec4 clip = mat4_mul_vec4(proj, view_pos);

        // Perspective divide
        if (clip.w != 0.0f)
        {
            clip.x /= clip.w;
            clip.y /= clip.w;
            clip.z /= clip.w;
        }

        // NDC to screen space
        screen_pos[i].x = (clip.x + 1.0f) * 0.5f * WINDOW_WIDTH;
        screen_pos[i].y = (1.0f - (clip.y + 1.0f) * 0.5f) * WINDOW_HEIGHT;
        screen_pos[i].z = clip.z;
    }

    // Draw triangles
    for (int i = 0; i < mesh.face_count; i++)
    {
        int a = mesh.faces[i][0];
        int b = mesh.faces[i][1];
        int c = mesh.faces[i][2];

        draw_triangle(
            screen_pos[a], screen_pos[b], screen_pos[c],
            world_pos[a], world_pos[b], world_pos[c],
            0xFFAAAAAA
        );
    }

    // Free temporary arrays
    free(world_pos);
    free(screen_pos);
}

void render(void)
{
    render_color_buffer();
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, color_buffer_texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void destroy_window(void)
{
    free(color_buffer);
    free(z_buffer);
    SDL_DestroyTexture(color_buffer_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void clear_color_buffer(uint32_t color)
{
    for (int y = 0; y < WINDOW_HEIGHT; y++)
    {
        for (int x = 0; x < WINDOW_WIDTH; x++)
        {
            color_buffer[(WINDOW_WIDTH * y) + x] = color;
        }
    }
}

void draw_pixel(int x, int y, float z, uint32_t color)
{
    if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT)
        return;

    int index = (WINDOW_WIDTH * y) + x;

    if (z > z_buffer[index])
    {
        z_buffer[index] = z;
        color_buffer[index] = color;
    }
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color)
{

    int delta_x = x1 - x0;
    int delta_y = y1 - y0;

    int longest_side_length = abs(delta_x);

    if (abs(delta_y) > longest_side_length)
    {
        longest_side_length = abs(delta_y);
    }

    float x_increment = delta_x / (float)longest_side_length;
    float y_increment = delta_y / (float)longest_side_length;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= longest_side_length; i++)
    {

        draw_pixel(
            (int)current_x,
            (int)current_y,
            0.0f,
            color);

        current_x += x_increment;
        current_y += y_increment;
    }
}

void draw_triangle(vertex2d s0, vertex2d s1, vertex2d s2, vec3 v0, vec3 v1, vec3 v2, uint32_t color)
{
    int min_x = fminf(s0.x, fminf(s1.x, s2.x));
    int max_x = fmaxf(s0.x, fmaxf(s1.x, s2.x));
    int min_y = fminf(s0.y, fminf(s1.y, s2.y));
    int max_y = fmaxf(s0.y, fmaxf(s1.y, s2.y));

    float area = fabsf((s1.x - s0.x) * (s2.y - s0.y) - (s1.y - s0.y) * (s2.x - s0.x));

    if (area < 0.0001f)
        return;

    // Calculate normal for lighting
    vec3 edge1 = vec3_sub(v1, v0);
    vec3 edge2 = vec3_sub(v2, v0);
    vec3 normal = vec3_normalize(vec3_cross(edge1, edge2));

    // Backface culling
    vec3 view_dir = vec3_normalize(vec3_sub(camera_pos, v0));
    if (vec3_dot(normal, view_dir) <= 0)
        return;

    // Compute lighting
    vec3 lit_color = compute_lighting(&lighting_sys, &default_material, v0, normal);
    
    // Clamp to [0, 1]
    lit_color.x = fminf(fmaxf(lit_color.x, 0.0f), 1.0f);
    lit_color.y = fminf(fmaxf(lit_color.y, 0.0f), 1.0f);
    lit_color.z = fminf(fmaxf(lit_color.z, 0.0f), 1.0f);
    
    uint8_t r = (uint8_t)(255 * lit_color.x);
    uint8_t g = (uint8_t)(255 * lit_color.y);
    uint8_t b = (uint8_t)(255 * lit_color.z);

    // Rasterize triangle
    for (int y = min_y; y <= max_y; y++)
    {
        for (int x = min_x; x <= max_x; x++)
        {
            float w0 = ((s1.y - s2.y) * (x - s2.x) + (s2.x - s1.x) * (y - s2.y)) / 
                       ((s1.y - s2.y) * (s0.x - s2.x) + (s2.x - s1.x) * (s0.y - s2.y));
            float w1 = ((s2.y - s0.y) * (x - s2.x) + (s0.x - s2.x) * (y - s2.y)) / 
                       ((s1.y - s2.y) * (s0.x - s2.x) + (s2.x - s1.x) * (s0.y - s2.y));
            float w2 = 1.0f - w0 - w1;

            if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0))
            {
                float alpha = w1 / area;
                float beta = w2 / area;
                float gamma = w0 / area;
                float z = alpha * v0.z + beta * v1.z + gamma * v2.z;

                uint32_t final_color = (0xFF << 24) | (r << 16) | (g << 8) | b;
                draw_pixel(x, y, z, final_color);
            }
        }
    }
}

void render_color_buffer(void)
{
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        WINDOW_WIDTH * sizeof(uint32_t)
    );
}
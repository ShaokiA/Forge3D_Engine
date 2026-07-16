#include <SDL3/SDL.h> //cross-platform library; handle input, output and rendering
#include <stdbool.h>  //easy true false identification
#include <stdint.h>   //for fixed size integer data types
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"     //user-defined
#include "matrix.h"     //user-defined
#include "projection.h" //user-defined
#include "camera.h"     //user-defined
#include <math.h>
#include "mesh_loader.h"
#include "mesh.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
// pre-processor, total screen=600*800 pixels
float *z_buffer = NULL;        // depth of every pixel and 1D array structure similar to color_buffer
uint32_t *color_buffer = NULL; // Its a framebuffer (1D array) where all pixel colors are stored in 32 bit unsigned int before final screen display
                               // later used malloc can convert it into a 2D array of pixels, each pixel represented by a 32-bit unsigned integer (uint32_t) in ARGB format (Alpha, Red, Green, Blue).
                               // alpha means transparency,0x000000FF(last 2 digits alpha)
                               //(color_buffer[index])'s index = y * WINDOW_WIDTH + x;

bool is_running = true;        // variable is_running declaration
SDL_Window *window = NULL;     // pointer of SDL_Window(the screen in which program runs) data type is set to NULL
SDL_Renderer *renderer = NULL; // pointer of SDL_Renderer(component/pen that draws in window) data type is set to NULL
SDL_Texture *color_buffer_texture = NULL;
// pointer of SDL_Texture(image that GPU can render/draw when user calls Graphics API(OpenGL used here) to connect GPU) data type is set to NULL

camera cam = {
    .fov = 3.14159f / 3.0f, // 60 degrees
    .aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT,
    .near = 0.1f,
    .far = 1000.0f};

#define PHI 1.618034f
// 20/6 face 12/8 vertice 30/12 edge






vec3 camera_pos = {0, 0, -5};

// Orbit camera for mouse controls
orbit_camera orbit_cam;

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

int main(void)
{

    if (!initialize_window())
    {             // jumps to line 141
        return 1; // main function exits if return 1, program ends with error code 1
    }

    // Initialize orbit camera
    vec3 target = {0.0f, 0.0f, 0.0f};
    orbit_cam = orbit_camera_create(target, 10.0f);
    if (!load_obj("model.obj", &mesh))
{
    printf("OBJ load failed!\n");
    return 1;
}

    while (is_running)
    {
  
        process_input();

        update();

        render();
    }

    destroy_window();

    return 0;
}

bool initialize_window(void)
{

    if (!SDL_Init(SDL_INIT_VIDEO))
    {                                                                        // initialization function of SDL library init & video is a flag,a type of feature SDL can
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError()); // SDL function finding error feterror()
        return false;
    }

    window = SDL_CreateWindow( // line 17  struct of SDL_Window* window = SDL_CreateWindow(
        "Forge3D Engine",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0 // since not 1 in line 120
    );
    // SDL_Window *SDL_CreateWindow(
    //  const char *title,
    //  int width,
    //  int height,
    //  SDL_WindowFlags flags
    //);
    // This is SDL_CreateWindow function prototype

    if (!window)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, NULL); // NULL gives default renderer like OpenGL, Direct3D, etc. SDL chooses the best one available on the system
    //     SDL_Renderer* SDL_CreateRenderer(
    //     SDL_Window* window,
    //     const char* name
    // );SDL standard renderer function

    if (!renderer)
    {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    color_buffer = (uint32_t *)malloc(
        sizeof(uint32_t) * WINDOW_WIDTH * WINDOW_HEIGHT);
    // because every pixel takes 32 bit or 4 byte

    z_buffer = (float *)malloc( // float used to preserve precision
        sizeof(float) * WINDOW_WIDTH * WINDOW_HEIGHT);

    color_buffer_texture = SDL_CreateTexture(
        renderer, // line 183
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH,
        WINDOW_HEIGHT);
    //         color_buffer_texture = SDL_CreateTexture(
    //     renderer,
    //     SDL_PIXELFORMAT_ARGB8888,
    //     SDL_TEXTUREACCESS_STREAMING,
    //     WINDOW_WIDTH,
    //     WINDOW_HEIGHT
    // );
    // SDL library function

    return true;
}

void process_input(void)
{

    SDL_Event event; // another empty struct

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

            // Keep old WASD/QE camera controls for fallback
            float speed = 0.1f;
            if (event.key.scancode == SDL_SCANCODE_W)
            {
                camera_pos.z += speed;
            }
            if (event.key.scancode == SDL_SCANCODE_S)
            {
                camera_pos.z -= speed;
            }
            if (event.key.scancode == SDL_SCANCODE_A)
            {
                camera_pos.x -= speed;
            }
            if (event.key.scancode == SDL_SCANCODE_D)
            {
                camera_pos.x += speed;
            }
            if (event.key.scancode == SDL_SCANCODE_Q)
            {
                camera_pos.y -= speed;
            }
            if (event.key.scancode == SDL_SCANCODE_E)
            {
                camera_pos.y += speed;
            }

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

    clear_color_buffer(0xFF000000); // A=FF means 100% visible resulting in solid black so new frame drawn

    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++)
    {
        z_buffer[i] = -1e9f;
    }
    // enough small z buffer results it to undermine it below new frame after rotation

    float time = SDL_GetTicks() * 0.001f; // get clicks store milli second time from starting so multiplication results in 1 sec, so it rotates per second

    mat4 rot_y = mat4_rotate_y(time); // time used as angle of rotation here
    mat4 rot_x = mat4_rotate_x(time * 0.7f);
    mat4 model = mat4_mul(rot_y, rot_x);

    mat4 proj = mat4_perspective(
        cam.fov,
        cam.aspect,
        cam.near,
        cam.far);

    // Use orbit camera view matrix
    mat4 view = orbit_camera_view_matrix(&orbit_cam);

    // Update camera_pos for lighting calculations
    camera_pos = orbit_camera_position(&orbit_cam);

    vec3 world_pos[MAX_VERTICES];      
    vertex2d screen_pos[MAX_VERTICES]; 
    // array of 12 vec3 and vertex2d

    for(int i=0;i<mesh.vertex_count;i++)
    {

        vec4 v = (vec4){
            mesh.vertices[i].x,
            mesh.vertices[i].y,
            mesh.vertices[i].z,
            1.0f};
        

        // MODEL
        vec4 world = mat4_mul_vec4(model, v); // model is matrix multiplication of x and y axis rotation
        world_pos[i] = (vec3){world.x, world.y, world.z};

        // VIEW
        vec4 view_pos = mat4_mul_vec4(view, world); // its just adding camera calculation with world

        // PROJECTION
        vec4 clip = mat4_mul_vec4(proj, view_pos); // near object bigger and farther object smaller

        if (clip.w != 0.0f) // perspective divide
        {
            clip.x /= clip.w;
            clip.y /= clip.w;
            clip.z /= clip.w;
        }
        // range from -1 to 1 but we need scale from 0 to 1 o multiply with pixel
        screen_pos[i].x = (clip.x + 1.0f) * 0.5f * WINDOW_WIDTH;
        screen_pos[i].y = (1.0f - (clip.y + 1.0f) * 0.5f) * WINDOW_HEIGHT; // extra 1 minus cuz 0,0 is at top left corner so y increases downward
        screen_pos[i].z = clip.z;
        // converting -1,0,1 scale to 0-800 pixel form
    }

    for(int i=0;i<mesh.face_count;i++)
    {

        int a = mesh.faces[i][0];
        int b = mesh.faces[i][1];
        int c = mesh.faces[i][2];

        draw_triangle(
            screen_pos[a],
            screen_pos[b],
            screen_pos[c],
            world_pos[a],
            world_pos[b],
            world_pos[c],
            0xFFAAAAAA);
    }
}

void render(void)
{

    // Copy framebuffer into SDL texture
    render_color_buffer();

    SDL_RenderClear(renderer);

    // Draw texture to screen
    SDL_RenderTexture(
        renderer,
        color_buffer_texture,
        NULL,
        NULL);

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

    if (x < 0 || x >= WINDOW_WIDTH ||
        y < 0 || y >= WINDOW_HEIGHT)
    {
        return;
    }

    int index = (WINDOW_WIDTH * y) + x;

    if (z > z_buffer[index])
    {
        z_buffer[index] = z;
        color_buffer[index] = color;
    }
}
// 1st 2 00 means transparent and FF means completely visible

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

    if (area < 0.0001f) // if shifting is negligible,no change
        return;

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

    for (int y = min_y; y <= max_y; y++)
    {
        for (int x = min_x; x <= max_x; x++)
        {

            float w0 = ((s1.y - s2.y) * (x - s2.x) + (s2.x - s1.x) * (y - s2.y)) / ((s1.y - s2.y) * (s0.x - s2.x) + (s2.x - s1.x) * (s0.y - s2.y));
            float w1 = ((s2.y - s0.y) * (x - s2.x) + (s0.x - s2.x) * (y - s2.y)) / ((s1.y - s2.y) * (s0.x - s2.x) + (s2.x - s1.x) * (s0.y - s2.y));
            float w2 = 1.0f - w0 - w1;

            if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0))
            {

                float alpha = w1 / area;
                float beta = w2 / area;
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

void render_color_buffer(void)
{

    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        WINDOW_WIDTH * sizeof(uint32_t));
}
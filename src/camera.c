#include "camera.h"
#include <SDL3/SDL.h>
#include <math.h>

#define PI 3.14159265359f

orbit_camera orbit_camera_create(vec3 target, float distance) {
    orbit_camera cam = {0};
    cam.target = target;
    cam.distance = distance;
    cam.yaw = 0.0f;
    cam.pitch = 0.0f;
    cam.min_distance = 1.0f;
    cam.max_distance = 50.0f;
    cam.sensitivity = 0.005f;  // Radians per pixel
    cam.zoom_speed = 1.0f;
    cam.is_dragging = false;
    return cam;
}

vec3 orbit_camera_position(orbit_camera* cam) {
    // Convert spherical coordinates to cartesian
    float cos_pitch = cosf(cam->pitch);
    float sin_pitch = sinf(cam->pitch);
    float cos_yaw = cosf(cam->yaw);
    float sin_yaw = sinf(cam->yaw);
    
    vec3 offset = {
        cam->distance * cos_pitch * sin_yaw,
        cam->distance * sin_pitch,
        cam->distance * cos_pitch * cos_yaw
    };
    
    return vec3_add(cam->target, offset);
}

mat4 orbit_camera_view_matrix(orbit_camera* cam) {
    vec3 cam_pos = orbit_camera_position(cam);
    
    // Compute forward vector (from camera to target)
    vec3 forward = vec3_sub(cam->target, cam_pos);
    forward = vec3_normalize(forward);
    
    // Compute right vector
    vec3 world_up = {0.0f, 1.0f, 0.0f};
    vec3 right = vec3_cross(forward, world_up);
    
    // Handle gimbal lock when looking straight up/down
    float right_len = vec3_length(right);
    if (right_len < 0.0001f) {
        // Use a different up vector
        world_up = (vec3){0.0f, 0.0f, 1.0f};
        right = vec3_cross(forward, world_up);
    }
    
    right = vec3_normalize(right);
    
    // Compute true up vector
    vec3 up = vec3_cross(right, forward);
    up = vec3_normalize(up);
    
    // Build view matrix (inverse of camera transform)
    mat4 view = {0};
    
    view.m[0][0] = right.x;
    view.m[0][1] = right.y;
    view.m[0][2] = right.z;
    view.m[0][3] = -vec3_dot(right, cam_pos);
    
    view.m[1][0] = up.x;
    view.m[1][1] = up.y;
    view.m[1][2] = up.z;
    view.m[1][3] = -vec3_dot(up, cam_pos);
    
    view.m[2][0] = -forward.x;
    view.m[2][1] = -forward.y;
    view.m[2][2] = -forward.z;
    view.m[2][3] = vec3_dot(forward, cam_pos);
    
    view.m[3][0] = 0.0f;
    view.m[3][1] = 0.0f;
    view.m[3][2] = 0.0f;
    view.m[3][3] = 1.0f;
    
    return view;
}

bool orbit_camera_mouse_button(orbit_camera* cam, bool is_down, bool is_left_button, void* window) {
    if (!is_left_button) {
        return false;
    }
    
    SDL_Window* sdl_window = (SDL_Window*)window;
    
    if (is_down) {
        // Start dragging
        cam->is_dragging = true;
        SDL_SetWindowRelativeMouseMode(sdl_window, true);
    } else {
        // Stop dragging
        cam->is_dragging = false;
        SDL_SetWindowRelativeMouseMode(sdl_window, false);
    }
    
    return true;
}

bool orbit_camera_mouse_motion(orbit_camera* cam, float dx, float dy) {
    if (!cam->is_dragging) {
        return false;
    }
    
    // Update yaw and pitch
    cam->yaw -= dx * cam->sensitivity;
    cam->pitch += dy * cam->sensitivity;
    
    // Clamp pitch to avoid flipping at poles (~89 degrees)
    float max_pitch = PI * 0.49f;  // ~88.2 degrees
    if (cam->pitch > max_pitch) {
        cam->pitch = max_pitch;
    }
    if (cam->pitch < -max_pitch) {
        cam->pitch = -max_pitch;
    }
    
    return true;
}

bool orbit_camera_mouse_wheel(orbit_camera* cam, float wheel_y) {
    // Zoom in/out by adjusting distance
    cam->distance -= wheel_y * cam->zoom_speed;
    
    // Clamp to min/max range
    if (cam->distance < cam->min_distance) {
        cam->distance = cam->min_distance;
    }
    if (cam->distance > cam->max_distance) {
        cam->distance = cam->max_distance;
    }
    
    return true;
}

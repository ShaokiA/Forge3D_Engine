#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"
#include "matrix.h"
#include <stdbool.h>

typedef struct {
    vec3 target;        // Point the camera orbits around
    float distance;     // Distance from target
    float yaw;          // Horizontal rotation (radians)
    float pitch;        // Vertical rotation (radians)
    float min_distance; // Minimum zoom distance
    float max_distance; // Maximum zoom distance
    float sensitivity;  // Mouse rotation sensitivity
    float zoom_speed;   // Scroll wheel zoom speed
    bool is_dragging;   // Track if mouse is currently dragging
} orbit_camera;

// Initialize an orbit camera with default values
orbit_camera orbit_camera_create(vec3 target, float distance);

// Get the view matrix for the camera
mat4 orbit_camera_view_matrix(orbit_camera* cam);

// Get the camera position in world space
vec3 orbit_camera_position(orbit_camera* cam);

// Handle mouse button events (returns true if camera state changed)
bool orbit_camera_mouse_button(orbit_camera* cam, bool is_down, bool is_left_button, void* window);

// Handle mouse motion events (returns true if camera state changed)
bool orbit_camera_mouse_motion(orbit_camera* cam, float dx, float dy);

// Handle mouse wheel events (returns true if camera state changed)
bool orbit_camera_mouse_wheel(orbit_camera* cam, float wheel_y);

#endif

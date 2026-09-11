#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include <stdbool.h>

// Error handling structures
typedef enum {
    MESH_ERROR_NONE,
    MESH_ERROR_FILE_NOT_FOUND,
    MESH_ERROR_INVALID_FORMAT,
    MESH_ERROR_PARSE_ERROR,
    MESH_ERROR_VALIDATION_FAILED,
    MESH_ERROR_OUT_OF_MEMORY
} MeshErrorCode;

typedef struct {
    MeshErrorCode code;
    char message[256];
    int line_number;  // -1 if not applicable
} MeshError;

typedef struct
{
    // Geometric data
    vec3 *vertices;           // Dynamically allocated vertex positions
    vec3 *normals;            // Dynamically allocated normal vectors
    vec2 *tex_coords;         // Dynamically allocated UV coordinates
    
    // Topology data
    int (*faces)[3];          // Dynamically allocated triangle indices
    int (*normal_indices)[3]; // Per-face normal indices (for non-smooth shading)
    int (*uv_indices)[3];     // Per-face texture coordinate indices
    
    // Counts and capacities
    int vertex_count;
    int face_count;
    int normal_count;
    int uv_count;
    
    // Metadata
    bool has_normals;
    bool has_tex_coords;
} Mesh;

extern Mesh mesh;

// Global error state
extern MeshError last_mesh_error;

// Error reporting function
void mesh_set_error(MeshErrorCode code, const char *format, ...);

// Memory management functions
bool mesh_allocate(Mesh *mesh, int vertex_capacity, int face_capacity,
                  int normal_capacity, int uv_capacity);
void mesh_free(Mesh *mesh);

// Normal computation function
void mesh_compute_normals(Mesh *mesh);

// Validation function
bool validate_mesh(const Mesh *mesh);

#endif
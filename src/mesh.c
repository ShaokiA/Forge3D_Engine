#include "mesh.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

Mesh mesh;

// Global error state
MeshError last_mesh_error = {MESH_ERROR_NONE, "", -1};

void mesh_set_error(MeshErrorCode code, const char *format, ...)
{
    va_list args;
    
    last_mesh_error.code = code;
    
    va_start(args, format);
    vsnprintf(last_mesh_error.message, sizeof(last_mesh_error.message), format, args);
    va_end(args);
    
    last_mesh_error.line_number = -1;
}

bool mesh_allocate(Mesh *mesh, int vertex_capacity, int face_capacity,
                  int normal_capacity, int uv_capacity)
{
    mesh->vertices = NULL;
    mesh->normals = NULL;
    mesh->tex_coords = NULL;
    mesh->faces = NULL;
    mesh->normal_indices = NULL;
    mesh->uv_indices = NULL;
    
    mesh->vertex_count = 0;
    mesh->face_count = 0;
    mesh->normal_count = 0;
    mesh->uv_count = 0;
    
    mesh->has_normals = false;
    mesh->has_tex_coords = false;
    
    // Allocate vertices array
    if (vertex_capacity > 0) {
        mesh->vertices = (vec3*)malloc(vertex_capacity * sizeof(vec3));
        if (!mesh->vertices) {
            mesh_set_error(MESH_ERROR_OUT_OF_MEMORY, 
                          "Failed to allocate memory for %d vertices", vertex_capacity);
            mesh_free(mesh);
            return false;
        }
        memset(mesh->vertices, 0, vertex_capacity * sizeof(vec3));
    }
    
    // Allocate normals array
    if (normal_capacity > 0) {
        mesh->normals = (vec3*)malloc(normal_capacity * sizeof(vec3));
        if (!mesh->normals) {
            mesh_set_error(MESH_ERROR_OUT_OF_MEMORY, 
                          "Failed to allocate memory for %d normals", normal_capacity);
            mesh_free(mesh);
            return false;
        }
        memset(mesh->normals, 0, normal_capacity * sizeof(vec3));
    }
    
    // Allocate texture coordinates array
    if (uv_capacity > 0) {
        mesh->tex_coords = (vec2*)malloc(uv_capacity * sizeof(vec2));
        if (!mesh->tex_coords) {
            mesh_set_error(MESH_ERROR_OUT_OF_MEMORY, 
                          "Failed to allocate memory for %d texture coordinates", uv_capacity);
            mesh_free(mesh);
            return false;
        }
        memset(mesh->tex_coords, 0, uv_capacity * sizeof(vec2));
    }
    
    // Allocate faces array
    if (face_capacity > 0) {
        mesh->faces = (int(*)[3])malloc(face_capacity * sizeof(int[3]));
        if (!mesh->faces) {
            mesh_set_error(MESH_ERROR_OUT_OF_MEMORY, 
                          "Failed to allocate memory for %d faces", face_capacity);
            mesh_free(mesh);
            return false;
        }
        memset(mesh->faces, 0, face_capacity * sizeof(int[3]));
    }
    
    // Allocate normal indices array
    if (face_capacity > 0 && normal_capacity > 0) {
        mesh->normal_indices = (int(*)[3])malloc(face_capacity * sizeof(int[3]));
        if (!mesh->normal_indices) {
            mesh_set_error(MESH_ERROR_OUT_OF_MEMORY, 
                          "Failed to allocate memory for %d normal indices", face_capacity);
            mesh_free(mesh);
            return false;
        }
        memset(mesh->normal_indices, 0, face_capacity * sizeof(int[3]));
    }
    
    // Allocate UV indices array
    if (face_capacity > 0 && uv_capacity > 0) {
        mesh->uv_indices = (int(*)[3])malloc(face_capacity * sizeof(int[3]));
        if (!mesh->uv_indices) {
            mesh_set_error(MESH_ERROR_OUT_OF_MEMORY, 
                          "Failed to allocate memory for %d UV indices", face_capacity);
            mesh_free(mesh);
            return false;
        }
        memset(mesh->uv_indices, 0, face_capacity * sizeof(int[3]));
    }
    
    return true;
}

void mesh_free(Mesh *mesh)
{
    if (!mesh) return;
    
    if (mesh->vertices) {
        free(mesh->vertices);
        mesh->vertices = NULL;
    }
    
    if (mesh->normals) {
        free(mesh->normals);
        mesh->normals = NULL;
    }
    
    if (mesh->tex_coords) {
        free(mesh->tex_coords);
        mesh->tex_coords = NULL;
    }
    
    if (mesh->faces) {
        free(mesh->faces);
        mesh->faces = NULL;
    }
    
    if (mesh->normal_indices) {
        free(mesh->normal_indices);
        mesh->normal_indices = NULL;
    }
    
    if (mesh->uv_indices) {
        free(mesh->uv_indices);
        mesh->uv_indices = NULL;
    }
    
    mesh->vertex_count = 0;
    mesh->face_count = 0;
    mesh->normal_count = 0;
    mesh->uv_count = 0;
    
    mesh->has_normals = false;
    mesh->has_tex_coords = false;
}

void mesh_compute_normals(Mesh *mesh)
{
    if (!mesh || !mesh->vertices) return;
    
    if (!mesh->normals) {
        mesh->normals = (vec3*)malloc(mesh->vertex_count * sizeof(vec3));
        if (!mesh->normals) {
            mesh_set_error(MESH_ERROR_OUT_OF_MEMORY, 
                          "Failed to allocate memory for normals");
            return;
        }
    }
    
    if (!mesh->faces || mesh->face_count == 0) {
        for (int i = 0; i < mesh->vertex_count; i++) {
            mesh->normals[i].x = 0.0f;
            mesh->normals[i].y = 0.0f;
            mesh->normals[i].z = 0.0f;
        }
        mesh->normal_count = mesh->vertex_count;
        mesh->has_normals = true;
        return;
    }
    
    // Initialize normals to zero
    for (int i = 0; i < mesh->vertex_count; i++) {
        mesh->normals[i].x = 0.0f;
        mesh->normals[i].y = 0.0f;
        mesh->normals[i].z = 0.0f;
    }
    
    // Compute and accumulate face normals
    for (int i = 0; i < mesh->face_count; i++) {
        int v0_idx = mesh->faces[i][0];
        int v1_idx = mesh->faces[i][1];
        int v2_idx = mesh->faces[i][2];
        
        vec3 v0 = mesh->vertices[v0_idx];
        vec3 v1 = mesh->vertices[v1_idx];
        vec3 v2 = mesh->vertices[v2_idx];
        
        vec3 edge1 = vec3_sub(v1, v0);
        vec3 edge2 = vec3_sub(v2, v0);
        vec3 face_normal = vec3_cross(edge1, edge2);
        
        mesh->normals[v0_idx] = vec3_add(mesh->normals[v0_idx], face_normal);
        mesh->normals[v1_idx] = vec3_add(mesh->normals[v1_idx], face_normal);
        mesh->normals[v2_idx] = vec3_add(mesh->normals[v2_idx], face_normal);
    }
    
    // Normalize
    for (int i = 0; i < mesh->vertex_count; i++) {
        mesh->normals[i] = vec3_normalize(mesh->normals[i]);
    }
    
    mesh->normal_count = mesh->vertex_count;
    mesh->has_normals = true;
}

bool validate_mesh(const Mesh *mesh)
{
    if (!mesh) {
        mesh_set_error(MESH_ERROR_VALIDATION_FAILED, "Mesh pointer is NULL");
        return false;
    }
    
    if (mesh->vertex_count <= 0) {
        mesh_set_error(MESH_ERROR_VALIDATION_FAILED, "Mesh has no vertices (vertex_count = %d)", mesh->vertex_count);
        return false;
    }
    
    if (mesh->face_count <= 0) {
        mesh_set_error(MESH_ERROR_VALIDATION_FAILED, "Mesh has no faces (face_count = %d)", mesh->face_count);
        return false;
    }
    
    // Validate face indices
    for (int i = 0; i < mesh->face_count; i++) {
        for (int j = 0; j < 3; j++) {
            int vertex_idx = mesh->faces[i][j];
            if (vertex_idx < 0 || vertex_idx >= mesh->vertex_count) {
                mesh_set_error(MESH_ERROR_VALIDATION_FAILED, 
                              "Face %d has invalid vertex index %d at position %d (vertex_count = %d)",
                              i, vertex_idx, j, mesh->vertex_count);
                return false;
            }
        }
    }
    
    // Validate normal indices
    if (mesh->has_normals && mesh->normal_indices && mesh->normal_count > 0) {
        for (int i = 0; i < mesh->face_count; i++) {
            for (int j = 0; j < 3; j++) {
                int normal_idx = mesh->normal_indices[i][j];
                if (normal_idx < 0 || normal_idx >= mesh->normal_count) {
                    mesh_set_error(MESH_ERROR_VALIDATION_FAILED,
                                  "Face %d has invalid normal index %d at position %d (normal_count = %d)",
                                  i, normal_idx, j, mesh->normal_count);
                    return false;
                }
            }
        }
    }
    
    // Validate UV indices
    if (mesh->has_tex_coords && mesh->uv_indices && mesh->uv_count > 0) {
        for (int i = 0; i < mesh->face_count; i++) {
            for (int j = 0; j < 3; j++) {
                int uv_idx = mesh->uv_indices[i][j];
                if (uv_idx < 0 || uv_idx >= mesh->uv_count) {
                    mesh_set_error(MESH_ERROR_VALIDATION_FAILED,
                                  "Face %d has invalid UV index %d at position %d (uv_count = %d)",
                                  i, uv_idx, j, mesh->uv_count);
                    return false;
                }
            }
        }
    }
    
    // Verify normals are unit length
    const float EPSILON = 1e-5f;
    
    if (mesh->has_normals && mesh->normals && mesh->normal_count > 0) {
        for (int i = 0; i < mesh->normal_count; i++) {
            vec3 normal = mesh->normals[i];
            float length = vec3_length(normal);
            float length_diff = length - 1.0f;
            if (length_diff < -EPSILON || length_diff > EPSILON) {
                mesh_set_error(MESH_ERROR_VALIDATION_FAILED,
                              "Normal %d does not have unit length (length = %f, expected 1.0)",
                              i, length);
                return false;
            }
        }
    }
    
    return true;
}

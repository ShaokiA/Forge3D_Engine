#include "mesh_loader.h"

#include <stdio.h>

#include <string.h>

#include <stdlib.h>

#include <stdbool.h>
bool load_obj(const char *filename, Mesh *mesh)
{
    FILE *file = fopen(filename, "r");

    if (!file)
    {
        mesh_set_error(MESH_ERROR_FILE_NOT_FOUND, 
                      "Failed to open mesh file '%s'", filename);
        return false;
    }
    
    printf("Loading: %s\n", filename);
    
    // First pass: count elements
    int vertex_count = 0;
    int normal_count = 0;
    int uv_count = 0;
    int face_count = 0;
    
    char line[256];
    
    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == 'v' && line[1] == ' ')
        {
            vertex_count++;
        }
        else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
        {
            normal_count++;
        }
        else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')
        {
            uv_count++;
        }
        else if (line[0] == 'f' && line[1] == ' ')
        {
            face_count++;
        }
    }
    
    // Allocate mesh
    if (!mesh_allocate(mesh, vertex_count, face_count, normal_count, uv_count))
    {
        fclose(file);
        return false;
    }
    
    // Reset file for second pass
    rewind(file);
    
    // Second pass: parse data
    mesh->vertex_count = 0;
    mesh->normal_count = 0;
    mesh->uv_count = 0;
    mesh->face_count = 0;
    
    while (fgets(line, sizeof(line), file))
    {
        // Vertex positions
        if (line[0] == 'v' && line[1] == ' ')
        {
            float x, y, z;
            if (sscanf(line, "v %f %f %f", &x, &y, &z) == 3)
            {
                mesh->vertices[mesh->vertex_count] = (vec3){x, y, z};
                mesh->vertex_count++;
            }
        }
        // Vertex normals
        else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
        {
            float x, y, z;
            if (sscanf(line, "vn %f %f %f", &x, &y, &z) == 3)
            {
                mesh->normals[mesh->normal_count] = (vec3){x, y, z};
                mesh->normal_count++;
                mesh->has_normals = true;
            }
        }
        // Texture coordinates
        else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')
        {
            float u, v;
            if (sscanf(line, "vt %f %f", &u, &v) == 2)
            {
                mesh->tex_coords[mesh->uv_count] = (vec2){u, v};
                mesh->uv_count++;
                mesh->has_tex_coords = true;
            }
        }
        // Faces (multiple formats supported)
        else if (line[0] == 'f' && line[1] == ' ')
        {
            int v1, v2, v3;
            int vt1, vt2, vt3;
            int vn1, vn2, vn3;
            
            // Try format: f v/vt/vn v/vt/vn v/vt/vn
            if (sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                      &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3) == 9)
            {
                mesh->faces[mesh->face_count][0] = v1 - 1;
                mesh->faces[mesh->face_count][1] = v2 - 1;
                mesh->faces[mesh->face_count][2] = v3 - 1;
                
                if (mesh->has_tex_coords)
                {
                    mesh->uv_indices[mesh->face_count][0] = vt1 - 1;
                    mesh->uv_indices[mesh->face_count][1] = vt2 - 1;
                    mesh->uv_indices[mesh->face_count][2] = vt3 - 1;
                }
                
                if (mesh->has_normals)
                {
                    mesh->normal_indices[mesh->face_count][0] = vn1 - 1;
                    mesh->normal_indices[mesh->face_count][1] = vn2 - 1;
                    mesh->normal_indices[mesh->face_count][2] = vn3 - 1;
                }
                
                mesh->face_count++;
            }
            // Try format: f v//vn v//vn v//vn
            else if (sscanf(line, "f %d//%d %d//%d %d//%d",
                           &v1, &vn1, &v2, &vn2, &v3, &vn3) == 6)
            {
                mesh->faces[mesh->face_count][0] = v1 - 1;
                mesh->faces[mesh->face_count][1] = v2 - 1;
                mesh->faces[mesh->face_count][2] = v3 - 1;
                
                if (mesh->has_normals)
                {
                    mesh->normal_indices[mesh->face_count][0] = vn1 - 1;
                    mesh->normal_indices[mesh->face_count][1] = vn2 - 1;
                    mesh->normal_indices[mesh->face_count][2] = vn3 - 1;
                }
                
                mesh->face_count++;
            }
            // Try format: f v/vt v/vt v/vt
            else if (sscanf(line, "f %d/%d %d/%d %d/%d",
                           &v1, &vt1, &v2, &vt2, &v3, &vt3) == 6)
            {
                mesh->faces[mesh->face_count][0] = v1 - 1;
                mesh->faces[mesh->face_count][1] = v2 - 1;
                mesh->faces[mesh->face_count][2] = v3 - 1;
                
                if (mesh->has_tex_coords)
                {
                    mesh->uv_indices[mesh->face_count][0] = vt1 - 1;
                    mesh->uv_indices[mesh->face_count][1] = vt2 - 1;
                    mesh->uv_indices[mesh->face_count][2] = vt3 - 1;
                }
                
                mesh->face_count++;
            }
            // f v v v
            else if (sscanf(line, "f %d %d %d", &v1, &v2, &v3) == 3)
            {
                mesh->faces[mesh->face_count][0] = v1 - 1;
                mesh->faces[mesh->face_count][1] = v2 - 1;
                mesh->faces[mesh->face_count][2] = v3 - 1;
                mesh->face_count++;
            }
        }
    }

    fclose(file);
    
    printf("Vertices : %d\n", mesh->vertex_count);
    printf("Normals  : %d\n", mesh->normal_count);
    printf("UVs      : %d\n", mesh->uv_count);
    printf("Faces    : %d\n", mesh->face_count);

    return true;
}
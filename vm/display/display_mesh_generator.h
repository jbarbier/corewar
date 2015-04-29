#ifndef DISPLAY_MESH_GENERATOR_H
#define DISPLAY_MESH_GENERATOR_H

#include "../../common/types.h"

struct s_v3;
typedef struct s_v3 t_v3;

void display_generate_sphere_count(int32 subDiv, int32* vertex_count, int32* index_count);
void display_generate_sphere(int subDiv, t_v3* center, float sphere_radius, uint8* vb, t_mesh_definition* def, uint16* ib, uint16 start);


void display_generate_rect_count(int32 subDiv, int32* vertex_count, int32* index_count);
void display_generate_rect(int subDiv, t_v3* min, t_v3* max, float sphere_radius, uint8* vb, t_mesh_definition* def, uint16* ib, uint16 start);

#endif
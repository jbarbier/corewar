#include <GL/glew.h>
#include <stdlib.h>

#include "display_math.h"
#include "display_mesh.h"
#include "display_gl.h"

void display_generate_rect_count(int32 subDiv, int32* vertex_count, int32* index_count)
{
	*vertex_count = 4;
	*index_count = 6;
}

void display_generate_rect(int subDiv, t_v3* min, t_v3* max, float sphere_radius, uint8* vb, t_mesh_definition* def, uint16* ib, uint16 start)
{
	float  vs[] = {
		min->x, min->y, min->z,
		max->x, min->y, min->z,
		min->x, max->y, min->z,
		max->x, max->y, min->z
	};
	float* vsp = vs;
	int32 i;

	for (i = 0; i < 4; ++i)
	{
		t_v3* v = (t_v3*)(vb + def->vertex_offset);
		t_v3* n = (t_v3*)(vb + def->normal_offset);
		v3_set(v, *vsp++, *vsp++, *vsp++);
		if (def->normal_offset != -1)
			v3_set(n, 0.0f, 0.0f, 1.0f);
		vb += def->stride;
	}

	*ib++ = start + 0;
	*ib++ = start + 1;
	*ib++ = start + 3;

	*ib++ = start + 0;
	*ib++ = start + 3;
	*ib++ = start + 2;
}



void display_generate_sphere_count(int32 subDiv, int32* vertex_count, int32* index_count)
{
	*vertex_count = subDiv * subDiv;
	*index_count = subDiv * (subDiv - 1) * 6;
}

void display_generate_sphere(int subDiv, t_v3* center, float sphere_radius, uint8* vb, t_mesh_definition* def, uint16* ib, uint16 start)
{
	float pi = DISPLAY_M_PI;
	int32 i, j;
	for (i = 0; i < subDiv; ++i)
	{
		float theta = (float)i / (subDiv - 1);
		for (j = 0; j < subDiv; ++j)
		{
			float phi = (float)j / subDiv;
			float radius = sinf(theta * pi) * sphere_radius;
			
			t_v3*	v = (t_v3*)(vb + def->vertex_offset);
			t_v3*	n = (t_v3*)(vb + def->normal_offset);
			
			v->x = sinf(phi * 2.0f * pi) * radius;
			v->y = cosf(phi * 2.0f * pi) * radius;
			v->z = cosf(theta * pi) * sphere_radius;
			if (def->normal_offset != -1)
				v3_norm(v, n);
			v3_add(v, center, v);
			vb += def->stride;
		}
	}

	for (i = 0; i < subDiv - 1; ++i)
	{
		for (j = 0; j < subDiv; ++j)
		{
			int32 i0 = start + (i + 0) * subDiv + j;
			int32 i1 = start + (i + 1) * subDiv + j;
			int32 i2 = start + (i + 1) * subDiv + ((j + 1) % subDiv);
			int32 i3 = start + (i + 0) * subDiv + ((j + 1) % subDiv);

			*ib++ = i0, *ib++ = i1, *ib++ = i2;
			*ib++ = i0, *ib++ = i2, *ib++ = i3;
		}
	}
}



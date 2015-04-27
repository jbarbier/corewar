#ifndef DISPLAY_MATH_H
#define DISPLAY_MATH_H

typedef struct s_v3
{
	float x;
	float y;
	float z;
} t_v3;

typedef struct s_mat4
{
	union
	{
		float v[16];
		float m[4][4];
	} mat;
} t_mat4;

void	mat4_ident(t_mat4* mat);
void	mat4_ortho(t_mat4* mat, float l, float r, float b, float t, float n, float f);
t_v3*	v3_set(t_v3* v, float x, float y, float z);


#endif
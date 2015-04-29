#ifndef DISPLAY_MATH_H
#define DISPLAY_MATH_H

typedef struct s_v3
{
	float x;
	float y;
	float z;
} t_v3;

typedef struct s_v2
{
	float x;
	float y;
} t_v2;

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
void	mat4_x_v3(t_mat4* mat, t_v3* src, t_v3* dst);

t_v3*	v3_set(t_v3* v, float x, float y, float z);
t_v2*	v2_set(t_v2* v, float x, float y);
t_v2*	v2_add(t_v2* vin, t_v2* add, t_v2* vout);
t_v2*	v2_sub(t_v2* vin, t_v2* sub, t_v2* vout);
t_v2*	v2_mul(t_v2* vin, float sx, float sy, t_v2* vout);


#endif
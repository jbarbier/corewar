#ifndef DISPLAY_MATH_H
#define DISPLAY_MATH_H

#include <math.h>


struct s_v4
{
	float x;
	float y;
	float z;
	float w;
};


struct s_v3
{
	float x;
	float y;
	float z;
};

struct s_v2
{
	float x;
	float y;
};

struct s_mat4
{
	union
	{
		float v[16];
		float m[4][4];
	} mat;
};


typedef struct s_v4 t_v4;
typedef struct s_v4 t_quat;

typedef struct s_v3 t_v3;
typedef struct s_v2 t_v2;
typedef struct s_mat4 t_mat4;

void	quat_from_euler(t_quat* out, float yaw, float pitch, float rool);
void	quat_normalize(t_quat* out);
void	quat_to_mat4(t_quat* in, t_mat4* out);

float	mat4_det(t_mat4* in);
int		mat4_invert(t_mat4* in, t_mat4* out);
void	mat4_ident(t_mat4* mat);
void	mat4_ortho(t_mat4* mat, float l, float r, float b, float t, float n, float f);
void	mat4_x_v3(t_mat4* mat, t_v3* src, t_v3* dst);
void	mat4_translate(t_mat4* mat, float x, float y, float z);
t_mat4*	mat4_mul(t_mat4* mat, t_mat4* with, t_mat4* out);

t_v3*	v3_set(t_v3* v, float x, float y, float z);
t_v2*	v2_set(t_v2* v, float x, float y);
t_v2*	v2_add(t_v2* vin, t_v2* add, t_v2* vout);
t_v2*	v2_sub(t_v2* vin, t_v2* sub, t_v2* vout);
t_v2*	v2_mul(t_v2* vin, float sx, float sy, t_v2* vout);
t_v3*	v3_norm(t_v3* vin, t_v3* vout);
t_v3*	v3_add(t_v3* vin, t_v3* add, t_v3* vout);
t_v4*	v4_set(t_v4* v, float x, float y, float z, float w);
#define DISPLAY_M_PI       3.14159265358979f

#endif
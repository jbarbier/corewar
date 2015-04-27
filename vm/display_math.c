#include "display_math.h"

void mat4_ident(t_mat4* mat)
{
	mat->mat.m[0][0] = 1; mat->mat.m[1][0] = 0; mat->mat.m[2][0] = 0; mat->mat.m[3][0] = 0;
	mat->mat.m[0][1] = 0; mat->mat.m[1][1] = 1; mat->mat.m[2][1] = 0; mat->mat.m[3][1] = 0;
	mat->mat.m[0][2] = 0; mat->mat.m[1][2] = 0; mat->mat.m[2][2] = 1; mat->mat.m[3][2] = 0;
	mat->mat.m[0][3] = 0; mat->mat.m[1][3] = 0; mat->mat.m[2][3] = 0; mat->mat.m[3][3] = 1;
}

void  mat4_ortho(t_mat4* mat, float l, float r, float b, float t, float n, float f)
{
	mat->mat.v[0] = 2 / (r - l);
	mat->mat.v[5] = 2 / (t - b);
	mat->mat.v[10] = -2 / (f - n);
	mat->mat.v[12] = -(r + l) / (r - l);
	mat->mat.v[13] = -(t + b) / (t - b);
	mat->mat.v[14] = -(f + n) / (f - n);
}

t_v3* v3_set(t_v3* v, float x, float y, float z)
{
	v->x = x;
	v->y = y;
	v->z = z;

	return v;
}

void mat4_x_v3(t_mat4* mat, t_v3* src, t_v3* dst)
{
	float x = src->x * mat->mat.m[0][0] + src->y * mat->mat.m[1][0] + src->z * mat->mat.m[2][0] + mat->mat.m[3][0];
	float y = src->x * mat->mat.m[0][1] + src->y * mat->mat.m[1][1] + src->z * mat->mat.m[2][1] + mat->mat.m[3][1];
	float z = src->x * mat->mat.m[0][2] + src->y * mat->mat.m[1][2] + src->z * mat->mat.m[2][2] + mat->mat.m[3][2];
	dst->x = x;
	dst->y = x;
	dst->z = x;
}
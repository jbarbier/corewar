#include <float.h>
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

void mat4_translate(t_mat4* mat, float x, float y, float z)
{
	mat->mat.m[3][0] = x;
	mat->mat.m[3][1] = y;
	mat->mat.m[3][2] = z;
}


t_mat4*	mat4_mul(t_mat4* mat, t_mat4* with, t_mat4* out)
{
	out->mat.v[0] = mat->mat.v[0] * with->mat.v[0] + mat->mat.v[4] * with->mat.v[1] + mat->mat.v[8] * with->mat.v[2] + mat->mat.v[12] * with->mat.v[3];
	out->mat.v[1] = mat->mat.v[1] * with->mat.v[0] + mat->mat.v[5] * with->mat.v[1] + mat->mat.v[9] * with->mat.v[2] + mat->mat.v[13] * with->mat.v[3];
	out->mat.v[2] = mat->mat.v[2] * with->mat.v[0] + mat->mat.v[6] * with->mat.v[1] + mat->mat.v[10] * with->mat.v[2] + mat->mat.v[14] * with->mat.v[3];
	out->mat.v[3] = mat->mat.v[3] * with->mat.v[0] + mat->mat.v[7] * with->mat.v[1] + mat->mat.v[11] * with->mat.v[2] + mat->mat.v[15] * with->mat.v[3];

	out->mat.v[4] = mat->mat.v[0] * with->mat.v[4] + mat->mat.v[4] * with->mat.v[5] + mat->mat.v[8] * with->mat.v[6] + mat->mat.v[12] * with->mat.v[7];
	out->mat.v[5] = mat->mat.v[1] * with->mat.v[4] + mat->mat.v[5] * with->mat.v[5] + mat->mat.v[9] * with->mat.v[6] + mat->mat.v[13] * with->mat.v[7];
	out->mat.v[6] = mat->mat.v[2] * with->mat.v[4] + mat->mat.v[6] * with->mat.v[5] + mat->mat.v[10] * with->mat.v[6] + mat->mat.v[14] * with->mat.v[7];
	out->mat.v[7] = mat->mat.v[3] * with->mat.v[4] + mat->mat.v[7] * with->mat.v[5] + mat->mat.v[11] * with->mat.v[6] + mat->mat.v[15] * with->mat.v[7];

	out->mat.v[8] = mat->mat.v[0] * with->mat.v[8] + mat->mat.v[4] * with->mat.v[9] + mat->mat.v[8] * with->mat.v[10] + mat->mat.v[12] * with->mat.v[11];
	out->mat.v[9] = mat->mat.v[1] * with->mat.v[8] + mat->mat.v[5] * with->mat.v[9] + mat->mat.v[9] * with->mat.v[10] + mat->mat.v[13] * with->mat.v[11];
	out->mat.v[10] = mat->mat.v[2] * with->mat.v[8] + mat->mat.v[6] * with->mat.v[9] + mat->mat.v[10] * with->mat.v[10] + mat->mat.v[14] * with->mat.v[11];
	out->mat.v[11] = mat->mat.v[3] * with->mat.v[8] + mat->mat.v[7] * with->mat.v[9] + mat->mat.v[11] * with->mat.v[10] + mat->mat.v[15] * with->mat.v[11];

	out->mat.v[12] = mat->mat.v[0] * with->mat.v[12] + mat->mat.v[4] * with->mat.v[13] + mat->mat.v[8] * with->mat.v[14] + mat->mat.v[12] * with->mat.v[15];
	out->mat.v[13] = mat->mat.v[1] * with->mat.v[12] + mat->mat.v[5] * with->mat.v[13] + mat->mat.v[9] * with->mat.v[14] + mat->mat.v[13] * with->mat.v[15];
	out->mat.v[14] = mat->mat.v[2] * with->mat.v[12] + mat->mat.v[6] * with->mat.v[13] + mat->mat.v[10] * with->mat.v[14] + mat->mat.v[14] * with->mat.v[15];
	out->mat.v[15] = mat->mat.v[3] * with->mat.v[12] + mat->mat.v[7] * with->mat.v[13] + mat->mat.v[11] * with->mat.v[14] + mat->mat.v[15] * with->mat.v[15];	
	return out;
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


t_v2*	v2_set(t_v2* v, float x, float y)
{
	v->x = x;
	v->y = y;
	return v;
}

t_v2*	v2_add(t_v2* vin, t_v2* add, t_v2* vout)
{
	vout->x = vin->x + add->x;
	vout->y = vin->y + add->y;
	return vout;
}

t_v2*	v2_sub(t_v2* vin, t_v2* sub, t_v2* vout)
{
	vout->x = vin->x - sub->x;
	vout->y = vin->y - sub->y;
	return vout;
}

t_v2*	v2_mul(t_v2* vin, float sx, float sy, t_v2* vout)
{
	vout->x = vin->x * sx;
	vout->y = vin->y * sy;
	return vout;
}

t_v3*	v3_norm(t_v3* vin, t_v3* vout)
{
	float v = sqrtf(vin->x * vin->x + vin->y * vin->y + vin->z * vin->z);
	float d = 1.0f;
	if (v > FLT_EPSILON)
		d = v;

	vout->x = vin->x / d;
	vout->y = vin->y / d;
	vout->z = vin->z / d;
	
	return vout;
}

t_v3*	v3_add(t_v3* vin, t_v3* add, t_v3* vout)
{
	vout->x = vin->x + add->x;
	vout->y = vin->y + add->y;
	vout->z = vin->z + add->z;
	
	return vout;
}

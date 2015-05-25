#include <GL/glew.h>
#include <stdlib.h>
#include "display_math.h"
#include "display_mesh.h"
#include "display_gl.h"

typedef struct s_mesh_vn
{
	t_v3	v;
	t_v3	n;
}	t_mesh_vn;


typedef struct s_mesh_v
{
	t_v3	v;
}	t_mesh_v;

typedef struct s_mesh_vc
{
	t_v3	v;
	uint32	argb;
}	t_mesh_vc;


typedef struct s_mesh
{
	int32	vao;
	int32	vb;
	int32	ib;

	int32	count;
	int32	type;
} t_mesh;

typedef struct s_display_mesh_shader
{
	t_shader	shader;
	int32		uniform_projection_matrix;
	int32		uniform_diffuse;
	int32		uniform_light_direction;
	int32		uniform_local_matrix;
	int32		uniform_ambient;
} t_display_mesh_shader;


typedef struct s_display_mesh_renderer
{
	t_display_mesh_shader	type[MESH_TYPE_COUNT];
	t_display_mesh_shader*	current;
} t_display_mesh_renderer;

static t_mesh_definition	s_definition_table[] =
{
	{ 0, 3 * sizeof(float), -1, 6 * sizeof(float)} ,
	{ 0, -1, -1, 3 * sizeof(float) },
	{ 0, -1, 3 * sizeof(float), 3 * sizeof(float) + sizeof(uint32)},

};


t_mesh_definition* display_mesh_get_definiton(int type)
{
	return &(s_definition_table[type]);
}

t_display_mesh_renderer* display_mesh_renderer_initialize()
{
	t_display_mesh_renderer* renderer = (t_display_mesh_renderer*)malloc(sizeof(t_display_mesh_renderer));
	display_gl_load_shader(&renderer->type[MESH_TYPE_VN].shader, "shaders/mesh_vn.vert", "shaders/mesh_vn.frag");
	int32 id = renderer->type[MESH_TYPE_VN].shader.id;
	renderer->type[MESH_TYPE_VN].uniform_projection_matrix = glGetUniformLocation(id, "uni_ProjectionMatrix");
	renderer->type[MESH_TYPE_VN].uniform_light_direction = glGetUniformLocation(id, "uni_LightDirection");
	renderer->type[MESH_TYPE_VN].uniform_diffuse = glGetUniformLocation(id, "uni_Diffuse");
	renderer->type[MESH_TYPE_VN].uniform_ambient = glGetUniformLocation(id, "uni_Ambient");
	renderer->type[MESH_TYPE_VN].uniform_local_matrix = glGetUniformLocation(id, "uni_LocalMatrix");

	display_gl_load_shader(&renderer->type[MESH_TYPE_V].shader, "shaders/mesh_v.vert", "shaders/mesh_v.frag");
	id = renderer->type[MESH_TYPE_V].shader.id;
	renderer->type[MESH_TYPE_V].uniform_projection_matrix = glGetUniformLocation(id, "uni_ProjectionMatrix");
	renderer->type[MESH_TYPE_V].uniform_light_direction = glGetUniformLocation(id, "uni_LightDirection");
	renderer->type[MESH_TYPE_V].uniform_diffuse = glGetUniformLocation(id, "uni_Diffuse");
	renderer->type[MESH_TYPE_V].uniform_ambient = glGetUniformLocation(id, "uni_Ambient");
	renderer->type[MESH_TYPE_V].uniform_local_matrix = glGetUniformLocation(id, "uni_LocalMatrix");

	display_gl_load_shader(&renderer->type[MESH_TYPE_VC].shader, "shaders/mesh_vc.vert", "shaders/mesh_vc.frag");
	id = renderer->type[MESH_TYPE_VC].shader.id;
	renderer->type[MESH_TYPE_VC].uniform_projection_matrix = glGetUniformLocation(id, "uni_ProjectionMatrix");
	renderer->type[MESH_TYPE_VC].uniform_light_direction = glGetUniformLocation(id, "uni_LightDirection");
	renderer->type[MESH_TYPE_VC].uniform_diffuse = glGetUniformLocation(id, "uni_Diffuse");
	renderer->type[MESH_TYPE_VC].uniform_ambient = glGetUniformLocation(id, "uni_Ambient");
	renderer->type[MESH_TYPE_VC].uniform_local_matrix = glGetUniformLocation(id, "uni_LocalMatrix");

	return renderer;
}

void	display_mesh_renderer_destroy(t_display_mesh_renderer* renderer)
{
	int i;
	for (i = 0; i < MESH_TYPE_COUNT; ++i)
	{
		display_gl_destroy_shader(&renderer->type[i].shader);
	}

	free(renderer);
}

t_mesh* display_mesh_vn_create(void* vertices, int32 vertex_count, uint16* indices, int32 index_count)
{
	t_mesh* mesh = malloc(sizeof(t_mesh));
	mesh->count = index_count;
	mesh->type = MESH_TYPE_VN;
	mesh->vb = display_gl_create_buffer(GL_ARRAY_BUFFER, vertex_count * sizeof(t_mesh_vn), GL_STATIC_DRAW, vertices);
	mesh->ib = display_gl_create_buffer(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(uint16), GL_STATIC_DRAW, indices);
	mesh->vao = display_gl_create_vao();
	display_gl_bind_vao(mesh->vao);
	display_gl_bind_buffer(GL_ARRAY_BUFFER, mesh->vb);
	t_mesh_definition* def = display_mesh_get_definiton(MESH_TYPE_VN);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, def->stride, (const void*)def->vertex_offset);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, def->stride, (const void*)def->normal_offset);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	return mesh;
}

t_mesh* display_mesh_vc_create(void* vertices, int32 vertex_count, uint16* indices, int32 index_count)
{
	t_mesh* mesh = malloc(sizeof(t_mesh));
	mesh->count = index_count;
	mesh->type = MESH_TYPE_VC;
	mesh->vb = display_gl_create_buffer(GL_ARRAY_BUFFER, vertex_count * sizeof(t_mesh_vc), GL_STATIC_DRAW, vertices);
	mesh->ib = display_gl_create_buffer(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(uint16), GL_STATIC_DRAW, indices);
	mesh->vao = display_gl_create_vao();
	display_gl_bind_vao(mesh->vao);
	display_gl_bind_buffer(GL_ARRAY_BUFFER, mesh->vb);
	t_mesh_definition* def = display_mesh_get_definiton(MESH_TYPE_VC);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, def->stride, (const void*)def->vertex_offset);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, def->stride, (const void*)def->color_offset);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	return mesh;
}

t_mesh* display_mesh_v_create(void* vertices, int32 vertex_count, uint16* indices, int32 index_count)
{
	t_mesh* mesh = malloc(sizeof(t_mesh));
	mesh->count = index_count;
	mesh->type = MESH_TYPE_V;
	mesh->vb = display_gl_create_buffer(GL_ARRAY_BUFFER, vertex_count * sizeof(t_mesh_v), GL_STATIC_DRAW, vertices);
	mesh->ib = display_gl_create_buffer(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(uint16), GL_STATIC_DRAW, indices);
	mesh->vao = display_gl_create_vao();
	display_gl_bind_vao(mesh->vao);
	display_gl_bind_buffer(GL_ARRAY_BUFFER, mesh->vb);
	t_mesh_definition* def = display_mesh_get_definiton(MESH_TYPE_V);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, def->stride, (const void*)def->vertex_offset);
	glEnableVertexAttribArray(0);
	return mesh;
}


void	display_mesh_destroy(t_mesh* mesh)
{
	display_gl_destroy_buffer(mesh->ib);
	display_gl_destroy_buffer(mesh->vb);
	display_gl_destroy_vao(mesh->vao);
	free(mesh);
}


void display_mesh_render_start(t_display_mesh_renderer* renderer, int type)
{
	renderer->current = &(renderer->type[type]);
	glUseProgram(renderer->current->shader.id);
}

void display_mesh_render(t_mesh* mesh)
{
	display_gl_bind_vao(mesh->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ib);
	glDrawElements(GL_TRIANGLES, mesh->count, GL_UNSIGNED_SHORT, 0);
}

void display_mesh_render_count(t_mesh* mesh, int32 count)
{
	display_gl_bind_vao(mesh->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ib);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);
}

void display_mesh_set_ambient(t_display_mesh_renderer* renderer, t_v4* color)
{
	glUniform4fv(renderer->current->uniform_ambient, 1, (float*) color);
}

void display_mesh_set_diffuse(t_display_mesh_renderer* renderer, t_v4* color)
{
	glUniform4fv(renderer->current->uniform_diffuse, 1, (float*)color);
}

void display_mesh_set_projection(t_display_mesh_renderer* renderer, t_mat4* projection)
{
	glUniformMatrix4fv(renderer->current->uniform_projection_matrix, 1, GL_FALSE, projection->mat.v);
}

int32 display_mesh_get_ib(t_mesh* mesh)
{
	return mesh->ib;
}

int32 display_mesh_get_vb(t_mesh* mesh)
{
	return mesh->vb;
}

void display_mesh_set_local(t_display_mesh_renderer* renderer, t_mat4* local)
{
	glUniformMatrix4fv(renderer->current->uniform_local_matrix, 1, GL_FALSE, local->mat.v);
}

void display_mesh_set_light_direction(t_display_mesh_renderer* renderer, t_v3* direction)
{
	glUniform3fv(renderer->current->uniform_light_direction, 1, (float*) direction);
}

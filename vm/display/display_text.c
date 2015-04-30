#include <GL/glew.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <stb_easy_font.h>

#include "display.h"
#include "display_gl.h"
#include "display_math.h"
#include "display_text.h"
#include "display_mesh.h"

#define MAX_TEXT		100
#define MAX_TEXT_LEN	256

typedef struct s_text
{
	t_v2	position;
	int32	rgba;
	char*	text;

} t_text;

typedef struct s_display_text
{
	t_text						texts[MAX_TEXT];
	int32						text_count;

	t_display_mesh_renderer*	text_renderer;
	t_mesh*						text_mesh;
	uint8*						text_mesh_vb;
	uint32						text_mesh_vb_size;
} t_display_text;


t_display_text* display_text_intialize()
{
	t_display_text* text = malloc(sizeof(t_display_text));
	uint16*			ib;
	uint16*			temp;
	int32			size;
	int32			i, start;

	text->text_count = 0;
	text->text_renderer = display_mesh_renderer_initialize();
	
	size = MAX_TEXT * MAX_TEXT_LEN * 6;
	temp = ib = malloc(size * sizeof(uint16));
	start = 0;
	for (i = 0; i < MAX_TEXT * MAX_TEXT_LEN; i += 6)
	{
		*temp++ = start + 0;
		*temp++ = start + 1;
		*temp++ = start + 3;
		*temp++ = start + 1;
		*temp++ = start + 3;
		*temp++ = start + 2;
		start += 4;
	}


	t_mesh_definition* def = display_mesh_get_definiton(MESH_TYPE_VC);
	text->text_mesh_vb_size = MAX_TEXT * MAX_TEXT_LEN * 4 * def->stride;
	text->text_mesh = display_mesh_vc_create(NULL, text->text_mesh_vb_size, ib, size);
	free(ib);

	text->text_mesh_vb = malloc(text->text_mesh_vb_size);
	return text;
}

void			display_text_clear(t_display_text* texts)
{
	int32 i;
	for (i = 0; i < texts->text_count; ++i)
	{
		free(texts->texts[i % MAX_TEXT].text);
	}

	texts->text_count = 0;
}

void			display_text_destroy(t_display_text* texts)
{
	int32 i;
	for (i = 0; i < texts->text_count; ++i)
		free(texts->texts[i % MAX_TEXT].text);
	free(texts->text_mesh_vb);
	free(texts);
}

void			display_text_add(t_display_text* texts, float x, float y, int32 rgba, char* format, ...)
{
	va_list args;
	char*	buffer = malloc(MAX_TEXT_LEN);
	int32	size;

	va_start(args, format);
	size = vsnprintf(buffer, MAX_TEXT_LEN, format, args);
	va_end(args);
	
	texts->texts[texts->text_count % MAX_TEXT].text = buffer;
	texts->texts[texts->text_count % MAX_TEXT].rgba = rgba;
	texts->texts[texts->text_count % MAX_TEXT].position.x = x;
	texts->texts[texts->text_count % MAX_TEXT].position.y = y;
	texts->text_count++;

}

void			display_text_render(t_display_text* texts, t_mat4* projection_view)
{
	int32 i;
	int32 vb_index = 0;
	t_v4	color_diffuse;
	t_v4	color_ambient;
	t_mat4	local;
	v4_set(&color_diffuse, 1.f, 1.f, 1.0f, 1.0f);
	v4_set(&color_ambient, 0.0f, 0.0f, 0.0f, 0.0f);
	mat4_ident(&local);

	for (i = 0; i < texts->text_count; ++i)
	{
		t_text* text = &(texts->texts[i]);
		vb_index += stb_easy_font_print(text->position.x, text->position.y, text->text, (uint8*)&text->rgba, texts->text_mesh_vb + vb_index, texts->text_mesh_vb_size) * 64;
	}
	display_gl_bind_buffer(GL_ARRAY_BUFFER, display_mesh_get_vb(texts->text_mesh));
	glBufferSubData(GL_ARRAY_BUFFER, 0, vb_index, texts->text_mesh_vb);

	display_mesh_render_start(texts->text_renderer, MESH_TYPE_VC);
	display_mesh_set_ambient(texts->text_renderer, &color_ambient);
	display_mesh_set_diffuse(texts->text_renderer, &color_diffuse);
	display_mesh_set_projection(texts->text_renderer, projection_view);
	display_mesh_set_local(texts->text_renderer, &local);
	display_mesh_render_count(texts->text_mesh, (vb_index / 64) * 6);
}

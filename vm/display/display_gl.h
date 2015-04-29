#ifndef DISPLAY_GL_H
#define DISPLAY_GL_H

#include "../../common/types.h"

typedef struct s_shader
{
	int id;
	int vertex_id;
	int fragment_id;
} t_shader;

#define DISPLAY_GL_ERROR_SHADER_VERT_FILE -1
#define DISPLAY_GL_ERROR_SHADER_FRAG_FILE -2
#define DISPLAY_GL_OK					   1

void	display_gl_log(int id, const char* desc, int is_shader);
void	display_gl_destroy_shader(t_shader* shader);
int32	display_gl_compile_shader(char* name, char* src, int32 type);
int32	display_gl_load_shader(t_shader* shader, char* vert_file, char* frag_file);
int32	display_gl_create_buffer(int32 type, int32 size, int32 flags, void* data);
int32	display_gl_load_texture(char* file_name);
void	display_gl_destroy_texture(int32 id);
void	display_gl_destroy_buffer(int32 id);

#endif
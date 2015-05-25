#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "display_gl.h"

#include "../../common/utils.h"

void display_gl_log(int id, const char* desc, int is_shader)
{
	int log_length;
	if (is_shader)
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_length);
	else
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &log_length);
	if (log_length > 1)
	{
		char* log = malloc(log_length);
		if (is_shader)
			glGetShaderInfoLog(id, log_length, &log_length, &log[0]);
		else
			glGetProgramInfoLog(id, log_length, &log_length, &log[0]);
		printf("%s: %s", desc, log);
		free(log);
	}
}

void display_gl_destroy_shader(t_shader* shader)
{
	glDeleteShader(shader->vertex_id);
	glDeleteShader(shader->fragment_id);
	glDeleteProgram(shader->id);
}

int32 display_gl_compile_shader(char* name, char* src, int32 type)
{
	int32 shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);
	int32 compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		display_gl_log(shader, name, 1);
		return -1;
	}
	return shader;
}

int32 display_gl_load_shader(t_shader* shader, char* vert_file, char* frag_file)
{
	shader->id = glCreateProgram();

	char* vert_code = file_to_memory(vert_file, NULL);
	char* frag_code = file_to_memory(frag_file, NULL);

	if (vert_code == NULL)
		return DISPLAY_GL_ERROR_SHADER_VERT_FILE;
	if (frag_code == NULL)
	{
		free(vert_code);
		return DISPLAY_GL_ERROR_SHADER_FRAG_FILE;
	}

	shader->vertex_id = display_gl_compile_shader(vert_file, vert_code, GL_VERTEX_SHADER);
	shader->fragment_id = display_gl_compile_shader(frag_file, frag_code, GL_FRAGMENT_SHADER);

	glAttachShader(shader->id, shader->vertex_id);
	glAttachShader(shader->id, shader->fragment_id);

	glValidateProgram(shader->id);
	glLinkProgram(shader->id);

	display_gl_log(shader->id, "LINK", 0);

	return DISPLAY_GL_OK;
}
int32 display_gl_create_buffer(int32 type, int32 size, int32 flags, void* data)
{
	int32 id;
	glGenBuffers(1, &id);
	glBindBuffer(type, id);
	glBufferData(type, size, data, flags);
	return id;
}
int32 display_gl_load_texture(char* file_name)
{
	int32 size;
	int32 width, height, channels;
	int32 id = -1;

	char* data = file_to_memory(file_name, &size);
	if (data)
	{
		char* result = stbi_load_from_memory(data, size, &width, &height, &channels, 4);
		if (result)
		{
			int32 internals;
			switch (channels)
			{
			case 1: internals = GL_RED; break;
			case 2: internals = GL_RG; break;
			case 3: internals = GL_RGB; break;
			case 4: internals = GL_RGBA; break;
			}

			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, internals, width, height, 0, internals, GL_UNSIGNED_BYTE, result);
			stbi_image_free(result);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		}
		free(data);
	}
	return id;
}

void display_gl_bind_buffer(int32 type, int32 id)
{
	glBindBuffer(type, id);
}

void display_gl_destroy_texture(int32 id)
{
	glDeleteTextures(1, &id);
}

void display_gl_destroy_buffer(int32 id)
{
	glDeleteBuffers(1, &id);
}

void display_gl_bind_vao(int32 id)
{
	glBindVertexArray(id);
}

void display_gl_destroy_vao(int32 id)
{
	glDeleteVertexArrays(1, &id);
}

int32 display_gl_create_vao()
{
	int32 id;
	glGenVertexArrays(1, &id);
	return id;
}

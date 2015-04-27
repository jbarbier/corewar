#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <math.h>
#include <stdlib.h>
#include "display.h"
#include "op.h"
#include "memory.h"
#include "vm.h"
#include "../common/utils.h"

#define DISPLAY_CELL_SIZE	1.f
#define DISPLAY_OK			1

#define DISPLAY_ERROR_SHADER_VERT_FILE -1
#define DISPLAY_ERROR_SHADER_FRAG_FILE -2

typedef struct s_shader
{
	int id;
	int vertex_id;
	int fragment_id;
} t_shader;

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

typedef struct s_display
{
	GLFWwindow*	window;

	int32		memory_vertex_buffer;
	int32		grid_vertex_buffer;
	int32		hex_texture;
	t_shader	memory_shader;
	int32		uniform_projection_matrix;
	int32		uniform_coord;
	int32		uniform_texture;
	int32		memory_vao;
	int32		memory_vertex_count;
} t_display;


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
		return DISPLAY_ERROR_SHADER_VERT_FILE;
	if (frag_code == NULL)
	{
		free(vert_code);
		return DISPLAY_ERROR_SHADER_FRAG_FILE;
	}

	shader->vertex_id = display_gl_compile_shader(vert_file, vert_code, GL_VERTEX_SHADER);
	shader->fragment_id = display_gl_compile_shader(frag_file, frag_code, GL_FRAGMENT_SHADER);

	glAttachShader(shader->id, shader->vertex_id);
	glAttachShader(shader->id, shader->fragment_id);

	glValidateProgram(shader->id);
	glLinkProgram(shader->id);

	return DISPLAY_OK;
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

void display_gl_destroy_texture(int32 id)
{
	glDeleteTextures(1, &id);
}

void display_gl_destroy_buffer(int32 id)
{
	glDeleteBuffers(1, &id);
}

typedef struct s_grid_vertex
{
	t_v3	v;
	float	i;
} t_grid_vertex;

void display_generate_grid(t_display* display, int memory_size)
{
	int32				width = (int32)roundf(sqrtf(MEM_SIZE));
	int32				height = width;
	int32				size = width * height;
	int32				x, y;
	t_grid_vertex*		vb;
	t_grid_vertex*		temp_vb;
	int32				vb_size = (size + height) * 6 * sizeof(t_grid_vertex);

	temp_vb = vb = malloc(vb_size);
	for (y = 0; y < height + 1; ++y)
	{
		float yc = DISPLAY_CELL_SIZE * y;
		float yn = yc + DISPLAY_CELL_SIZE;
		for (x = 0; x < width; ++x)
		{
			float xc = DISPLAY_CELL_SIZE * x;
			float xn = xc + DISPLAY_CELL_SIZE;
			v3_set(&vb->v, xc, yc, 0.1f), vb->i = 0, vb++;
			v3_set(&vb->v, xn, yc, 0.1f), vb->i = 1, vb++;
			v3_set(&vb->v, xn, yn, 0.1f), vb->i = 3, vb++;

			v3_set(&vb->v, xc, yc, 0.1f), vb->i = 0, vb++;
			v3_set(&vb->v, xn, yn, 0.1f), vb->i = 3, vb++;
			v3_set(&vb->v, xc, yn, 0.1f), vb->i = 2, vb++;
		}
	}

	display->grid_vertex_buffer = display_gl_create_buffer(GL_ARRAY_BUFFER, vb_size, GL_STATIC_DRAW, temp_vb);
	display->memory_vertex_buffer = display_gl_create_buffer(GL_ARRAY_BUFFER, (size + height) * 4, GL_STREAM_DRAW, NULL);
	display->memory_vertex_count = (size + height);
	free(temp_vb);

}

t_display* display_initialize()
{
	t_display*			display = (t_display*)malloc(sizeof(t_display));

	if (!glfwInit())
		return NULL;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	display->window = glfwCreateWindow(640, 480, "Corewar", NULL, NULL);

	glfwMakeContextCurrent(display->window);
	glewExperimental = GL_TRUE;
	glewInit();

	display_generate_grid(display, MEM_SIZE);
	display->hex_texture = display_gl_load_texture("data/hex.png");
	display_gl_load_shader(&display->memory_shader, "shaders/memory.vert", "shaders/memory.frag");
	
	display->uniform_projection_matrix = glGetUniformLocation(display->memory_shader.id, "uni_ProjectionMatrix");
	display->uniform_coord = glGetUniformLocation(display->memory_shader.id, "uni_Coord");
	display->uniform_texture = glGetUniformLocation(display->memory_shader.id, "uni_Texture");
	float delta = 1.f / 255.f;
	float uv[] = {
		0.f, 0.f,
		delta, 0.f,
		0.f, 1.0f,
		delta, 1.0f
	};
	glUseProgram(display->memory_shader.id);
	glUniform2fv(display->uniform_coord, 4, uv);
	int id = 0;
	glUniform1iv(display->uniform_texture, 1, &id);

	glGenVertexArrays(1, &display->memory_vao);
	glBindVertexArray(display->memory_vao);
	glBindBuffer(GL_ARRAY_BUFFER, display->grid_vertex_buffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);	
	glBindBuffer(GL_ARRAY_BUFFER, display->memory_vertex_buffer);
	glVertexAttribPointer(1, 1, GL_UNSIGNED_BYTE, GL_FALSE, 0, NULL);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	return display;
}

int	 display_should_exit(t_display* display)
{
	return glfwWindowShouldClose(display->window);
}

void display_destroy(t_display* display)
{
	display_gl_destroy_buffer(display->memory_vertex_buffer);
	display_gl_destroy_buffer(display->grid_vertex_buffer);
	display_gl_destroy_texture(display->hex_texture);
	display_gl_destroy_shader(&display->memory_shader);
	glfwDestroyWindow(display->window);
	glfwTerminate();
}

void display_step(t_vm* vm, t_display* display)
{
	uint32* dst;
	uint8*	src = (uint8*) vm->memory->data;
	int		size = vm->memory->size;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE); 

	glBindBuffer(GL_ARRAY_BUFFER, display->memory_vertex_buffer);
	dst = (int*) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	while (size--)
	{
		uint32 v = ((uint32) *src++);
		*dst++ = v << 24 | v << 16 | v << 8 | v;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	t_mat4 mat;
	mat4_ident(&mat);
	mat4_ortho(&mat, 0, sqrtf(MEM_SIZE) * 10, 0, sqrtf(MEM_SIZE) * 10, 0.0, 10);

	glUseProgram(display->memory_shader.id);
	glUniformMatrix4fv(display->uniform_projection_matrix, 1, GL_FALSE, mat.mat.v);
	glBindTexture(GL_TEXTURE_2D, display->hex_texture);
	glBindVertexArray(display->memory_vao);
	glDrawArrays(GL_TRIANGLES, 0, display->memory_vertex_count);

	glfwSwapBuffers(display->window);
	glfwPollEvents();
}

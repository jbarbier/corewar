#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdlib.h>
#include "display.h"
#include "op.h"
#include "memory.h"
#include "vm.h"

#define DISPLAY_CELL_SIZE	10.f

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
} t_display;


int32 display_gl_create_buffer(int32 type, int32 size, int32 flags, void* data)
{
	int32 id;
	glGenBuffers(1, &id);
	glBindBuffer(type, id);
	glBufferData(type, size, data, flags);
	return id;
}

void display_gl_destroy_buffer(int32 id)
{
	glDeleteBuffers(1, &id);
}

typedef struct s_grid_vertex
{
	t_v3	v;
	int32	i;
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
			v3_set(&vb->v, xc, yc, 0), vb->i = 0, vb++;
			v3_set(&vb->v, xn, yc, 0), vb->i = 1, vb++;
			v3_set(&vb->v, xn, yn, 0), vb->i = 3, vb++;

			v3_set(&vb->v, xc, yc, 0), vb->i = 0, vb++;
			v3_set(&vb->v, xn, yn, 0), vb->i = 3, vb++;
			v3_set(&vb->v, xc, yn, 0), vb->i = 2, vb++;
		}
	}

	display->grid_vertex_buffer = display_gl_create_buffer(GL_ARRAY_BUFFER, vb_size, GL_STATIC_DRAW, temp_vb);
	display->memory_vertex_buffer = display_gl_create_buffer(GL_ARRAY_BUFFER, (size + height) * 4, GL_STREAM_DRAW, NULL);

	free(temp_vb);

}

t_display* display_initialize()
{
	t_display*			display = (t_display*)malloc(sizeof(t_display));

	if (!glfwInit())
		return NULL;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	display->window = glfwCreateWindow(640, 480, "Corewar", NULL, NULL);

	glfwMakeContextCurrent(display->window);
	glewExperimental = GL_TRUE;
	glewInit();

	display_generate_grid(display, MEM_SIZE);
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
	glfwDestroyWindow(display->window);
	glfwTerminate();
}

void display_step(t_vm* vm, t_display* display)
{	
	uint32* dst;
	uint8*	src = (uint8*) vm->memory->data;
	int		size = vm->memory->size;

	glBindBuffer(GL_ARRAY_BUFFER, display->grid_vertex_buffer);
	dst = (int*) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	while (size--)
	{
		uint32 v = ((uint32) *src++);
		*dst++ = v << 24 | v << 16 | v << 8 | v;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glfwSwapBuffers(display->window);
	glfwPollEvents();
}

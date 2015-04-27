#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <math.h>
#include <stdlib.h>
#include "display.h"
#include "op.h"
#include "memory.h"
#include "vm.h"
#include "../common/utils.h"
#include "display_math.h"
#include "display_gl.h"



typedef struct s_display
{
	GLFWwindow*	window;

	int32		memory_vertex_buffer;
	int32		grid_vertex_buffer;

	int32		hex_texture;
	t_shader	memory_shader;
	
	int32		uniform_projection_matrix;
	int32		uniform_coord;
	int32		uniform_color;
	int32		uniform_texture;
	
	int32		memory_vao;
	int32		memory_vertex_count;
	int32		frame_buffer_width;
	int32		frame_buffer_height;
	float		frame_buffer_ratio;

	double		mouse_prev_x;
	double		mouse_prev_y;

	double		display_zoom;
	double		display_offset_x;
	double		display_offset_y;

	double		frame_last_time;
} t_display;



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
			v3_set(&vb->v, xc, yc, 0.f), vb->i = 0, vb++;
			v3_set(&vb->v, xn, yc, 0.f), vb->i = 1, vb++;
			v3_set(&vb->v, xn, yn, 0.f), vb->i = 3, vb++;

			v3_set(&vb->v, xc, yc, 0.f), vb->i = 0, vb++;
			v3_set(&vb->v, xn, yn, 0.f), vb->i = 3, vb++;
			v3_set(&vb->v, xc, yn, 0.f), vb->i = 2, vb++;
		}
	}

	display->grid_vertex_buffer = display_gl_create_buffer(GL_ARRAY_BUFFER, vb_size, GL_STATIC_DRAW, temp_vb);
	display->memory_vertex_buffer = display_gl_create_buffer(GL_ARRAY_BUFFER, (size + height) * 12, GL_STREAM_DRAW, NULL);	
	display->memory_vertex_count = (size + height) * 6;
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
	display->uniform_color = glGetUniformLocation(display->memory_shader.id, "uni_Color");
	display->uniform_texture = glGetUniformLocation(display->memory_shader.id, "uni_Texture");

	float delta = 1.f / 256.f;
	float uv[] = {
		0.f, 0.f, delta, 0.f, 0.f, 1.0f, delta, 1.0f
	};

	float color[] = {
		1.f, 1.f, 1.f, 1.f,
		1.f, 0.f, 0.f, 1.f,
		0.f, 1.f, 0.f, 1.f,
		0.f, 0.f, 1.f, 1.f
	};

	glUseProgram(display->memory_shader.id);
	glUniform2fv(display->uniform_coord, 4, uv);
	glUniform4fv(display->uniform_color, 4, color);
	int id = 0;
	glUniform1iv(display->uniform_texture, 1, &id);

	glGenVertexArrays(1, &display->memory_vao);
	glBindVertexArray(display->memory_vao);
	glBindBuffer(GL_ARRAY_BUFFER, display->grid_vertex_buffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const void*)0);
	
	glBindBuffer(GL_ARRAY_BUFFER, display->memory_vertex_buffer);
	glVertexAttribPointer(1, 1, GL_UNSIGNED_BYTE, GL_FALSE, 2, (const void*)0);
	glVertexAttribPointer(2, 1, GL_UNSIGNED_BYTE, GL_FALSE, 2, (const void*)1);


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glfwGetCursorPos(display->window, &display->mouse_prev_x, &display->mouse_prev_y);
	display->display_zoom = 1.0;
	display->display_offset_x = 0;
	display->display_offset_y = 0;
	display->frame_last_time = glfwGetTime();

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
	glDeleteVertexArrays(1, &display->memory_vao);
	glfwDestroyWindow(display->window);
	glfwTerminate();
}

void display_update_memory(struct s_vm* vm, t_display* display)
{
	uint8* dst;
	uint8*	src = (uint8*)vm->memory->data;
	uint8*	flag = (uint8*)vm->memory->flag;
	int		size = vm->memory->size;
	
	glBindBuffer(GL_ARRAY_BUFFER, display->memory_vertex_buffer);
	dst = (uint8*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);	
	while (size--)
	{
		uint8 v = *src++;
		uint8 f = *flag++;

		*dst++ = v;
		*dst++ = f;

		*dst++ = v;
		*dst++ = f;

		*dst++ = v;
		*dst++ = f;

		*dst++ = v;
		*dst++ = f;

		*dst++ = v;
		*dst++ = f;

		*dst++ = v;
		*dst++ = f;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	
	/*value = (value + 1) % 256;*/
}


void display_step(struct s_vm* vm, t_display* display)
{
	double mouse_cur_x, mouse_cur_y;
	double mouse_delta_x, mouse_delta_y;
	
	double current_time = glfwGetTime();
	double delta = display->frame_last_time - current_time;	
	display->frame_last_time = current_time;

	t_mat4 mat;

	glfwGetFramebufferSize(display->window, &display->frame_buffer_width, &display->frame_buffer_height);
	glfwGetCursorPos(display->window, &mouse_cur_x, &mouse_cur_y);
	
	mouse_delta_x = display->mouse_prev_x - mouse_cur_x;
	mouse_delta_y = display->mouse_prev_y - mouse_cur_y;

	display->mouse_prev_x = mouse_cur_x;
	display->mouse_prev_y = mouse_cur_y;

	if (glfwGetMouseButton(display->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{ 
		display->display_offset_x += mouse_delta_x * display->display_zoom;
		display->display_offset_y += mouse_delta_y * display->display_zoom;
	}

	if (glfwGetKey(display->window, GLFW_KEY_Q) == GLFW_PRESS)
		display->display_zoom += delta;
	if (glfwGetKey(display->window, GLFW_KEY_A) == GLFW_PRESS)
		display->display_zoom -= delta;

	display_update_memory(vm, display);
	
	mat4_ident(&mat);
	mat4_ortho(&mat, display->display_offset_x, display->display_offset_x + display->frame_buffer_width  * display->display_zoom,
		display->display_offset_y + display->frame_buffer_height * display->display_zoom, display->display_offset_y, 0, 10);


	display->frame_buffer_ratio = (float)display->frame_buffer_width / (float)display->frame_buffer_height;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, display->frame_buffer_width, display->frame_buffer_height);

	
	
	glUseProgram(display->memory_shader.id);
	glUniformMatrix4fv(display->uniform_projection_matrix, 1, GL_FALSE, mat.mat.v);
	glBindTexture(GL_TEXTURE_2D, display->hex_texture);
	glBindVertexArray(display->memory_vao);
	glDrawArrays(GL_TRIANGLES, 0, display->memory_vertex_count);

	glfwSwapBuffers(display->window);
	glfwPollEvents();
	
}

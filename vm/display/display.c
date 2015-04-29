#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory.h>
#include <stb_image.h>
#include <math.h>
#include <stdlib.h>

#include "../../common/utils.h"
#include "../memory.h"
#include "../vm.h"


#include "display.h"
#include "display_math.h"
#include "display_gl.h"
#include "display_mesh.h"
#include "display_mesh_generator.h"


typedef struct s_display
{
	GLFWwindow*	window;


	int32		hex_texture;

	t_shader	memory_shader;	
	int32		memory_grid_vertex_buffer;
	int32		memory_grid_index_buffer;
	int32		memory_vertex_buffer;
	uint8*		memory_temp_buffer;

	int32		memory_uniform_projection_matrix;
	int32		memory_uniform_coord;
	int32		memory_uniform_color;
	int32		memory_uniform_texture;
	int32		memory_vao;
	int32		memory_vertex_count;
	int32		memory_index_count;
	int32		memory_size;
	float		memory_width;
	float		memory_height;
	int32		memory_stride;
	t_shader	io_shader;
	int32		io_uniform_projection_matrix;
	int32		io_uniform_color;

	int32		frame_buffer_width;
	int32		frame_buffer_height;
	float		frame_buffer_ratio;

	double		mouse_prev_x;
	double		mouse_prev_y;

	float		display_zoom;
	float		display_center_x;
	float		display_center_y;

	double		frame_last_time;


	t_display_mesh_renderer* mesh_renderer;
	t_mesh*					 process_mesh;
} t_display;



typedef struct s_grid_vertex
{
	t_v3	v;
	float	i;
}	t_grid_vertex;


void display_generate_process_mesh(t_display* display)
{

}

void display_generate_grid(t_display* display, int memory_size)
{
	int32				width = (int32)roundf(sqrtf(MEM_SIZE));
	int32				height = width;
	int32				size = width * height;
	int32				x, y;
	t_grid_vertex*		vb;
	t_grid_vertex*		temp_vb;
	uint16*				ib;
	uint16*				temp_ib;
	int32				vb_size = (size + height) * 4 * sizeof(t_grid_vertex);
	int32				ib_size = (size + height) * 6 * sizeof(int16);
	int32				cell_index = 0;
	temp_vb = vb = malloc(vb_size);
	temp_ib = ib = malloc(ib_size);
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
			v3_set(&vb->v, xc, yn, 0.f), vb->i = 2, vb++;
			v3_set(&vb->v, xn, yn, 0.f), vb->i = 3, vb++;

			*ib++ = cell_index + 0;
			*ib++ = cell_index + 1;
			*ib++ = cell_index + 3;

			*ib++ = cell_index + 0;
			*ib++ = cell_index + 3;
			*ib++ = cell_index + 2;
			
			cell_index += 4;
		}
	}
	display->memory_grid_vertex_buffer = display_gl_create_buffer(GL_ARRAY_BUFFER, vb_size, GL_STATIC_DRAW, temp_vb);
	display->memory_grid_index_buffer = display_gl_create_buffer(GL_ELEMENT_ARRAY_BUFFER, ib_size, GL_STATIC_DRAW, temp_ib);
	display->memory_vertex_buffer = display_gl_create_buffer(GL_ARRAY_BUFFER, (size + height) * 4, GL_STREAM_DRAW, NULL);		
	display->memory_temp_buffer = (uint8*)malloc((size + height) * 4);	
	display->memory_vertex_count = (size + height) * 4;
	display->memory_index_count = (size + height) * 6;
	display->memory_size = (size + height);
	display->memory_width = DISPLAY_CELL_SIZE * width;
	display->memory_height = DISPLAY_CELL_SIZE * height + 1;
	display->memory_stride = width;
	free(temp_vb);
	free(temp_ib);

}

t_display* display_initialize(int width, int height)
{
	t_display*			display = (t_display*)malloc(sizeof(t_display));

	if (!glfwInit())
		return NULL;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	display->window = glfwCreateWindow(width, height, "corewar", NULL, NULL);

	glfwMakeContextCurrent(display->window);
	glewExperimental = GL_TRUE;
	glewInit();

	display_generate_grid(display, MEM_SIZE);
	display->hex_texture = display_gl_load_texture("data/hex.png");

	display_gl_load_shader(&display->memory_shader, "shaders/memory.vert", "shaders/memory.frag");
	display->memory_uniform_projection_matrix = glGetUniformLocation(display->memory_shader.id, "uni_ProjectionMatrix");
	display->memory_uniform_coord = glGetUniformLocation(display->memory_shader.id, "uni_Coord");
	display->memory_uniform_color = glGetUniformLocation(display->memory_shader.id, "uni_Color");
	display->memory_uniform_texture = glGetUniformLocation(display->memory_shader.id, "uni_Texture");

	{
		float delta = 1.f / 256.f;
		float uv[] = {
			0.f, 0.f, delta, 0.f, 0.f, 1.0f, delta, 1.0f
		};
		int id = 0;
		glUseProgram(display->memory_shader.id);
		glUniform2fv(display->memory_uniform_coord, 4, uv);
		glUniform1iv(display->memory_uniform_texture, 1, &id);
		glUseProgram(0);
	}

	display_gl_load_shader(&display->io_shader, "shaders/io.vert", "shaders/io.frag");
	display->io_uniform_projection_matrix = glGetUniformLocation(display->io_shader.id, "uni_ProjectionMatrix");
	display->io_uniform_color = glGetUniformLocation(display->io_shader.id, "uni_Color");



	display->memory_vao = display_gl_create_vao();
	display_gl_bind_vao(display->memory_vao);
	display_gl_bind_buffer(GL_ARRAY_BUFFER, display->memory_grid_vertex_buffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const void*)0);
	display_gl_bind_buffer(GL_ARRAY_BUFFER, display->memory_vertex_buffer);
	glVertexAttribPointer(1, 1, GL_UNSIGNED_BYTE, GL_FALSE, 1, (const void*)0);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glfwGetCursorPos(display->window, &display->mouse_prev_x, &display->mouse_prev_y);
	glfwGetFramebufferSize(display->window, &display->frame_buffer_width, &display->frame_buffer_height);

	float screen_memory_ratio = display->memory_width / (float) display->frame_buffer_width;
	float screen_memory_ratio_height = display->memory_height / (float) display->frame_buffer_height;
	if (screen_memory_ratio_height > screen_memory_ratio)
		screen_memory_ratio = screen_memory_ratio_height;

	display->display_zoom = screen_memory_ratio;
	display->display_center_x = ((float)display->frame_buffer_width * 0.5f) * screen_memory_ratio;
	display->display_center_y = ((float)display->frame_buffer_height * 0.5f) * screen_memory_ratio;

	display->frame_last_time = glfwGetTime();

	display->mesh_renderer = display_mesh_renderer_initialize();
	int32 vb_count;
	int32 ib_count;
	display_generate_sphere_count(16, &vb_count, &ib_count);
	t_mesh_definition* def = display_mesh_get_definiton(MESH_TYPE_VN);
	uint8*	vb = malloc(vb_count * def->stride);
	uint16* ib = malloc(ib_count * sizeof(uint16));
	t_v3 center = { 0, 0, 0};
	display_generate_sphere(16, &center, DISPLAY_CELL_SIZE * 0.5f, vb, def, ib);
	display->process_mesh = display_mesh_vn_create(vb, vb_count, ib, ib_count);
	free(vb);
	free(ib);
	return display;
}

int	 display_should_exit(t_display* display)
{
	return glfwWindowShouldClose(display->window);
}

void display_destroy(t_display* display)
{
	display_mesh_renderer_destroy(display->mesh_renderer);
	display_gl_destroy_buffer(display->memory_vertex_buffer);
	display_gl_destroy_buffer(display->memory_grid_vertex_buffer);
	display_gl_destroy_buffer(display->memory_grid_index_buffer);
	display_gl_destroy_texture(display->hex_texture);
	display_gl_destroy_shader(&display->memory_shader);
	display_gl_destroy_shader(&display->io_shader);	
	display_gl_destroy_vao(display->memory_vao);
	glfwDestroyWindow(display->window);
	free(display->memory_temp_buffer);
	glfwTerminate();
}

void display_update_memory(struct s_vm* vm, t_display* display)
{
	uint8* dst;
	uint8*	src = (uint8*)vm->memory->data;
	int		size = vm->memory->size;
	
	
	dst = (uint8*)display->memory_temp_buffer; 
	memset(dst, 0, display->memory_size * 4);

	while (size--)
	{
		uint8 v = *src++;

		*dst++ = v;
		*dst++ = v;
		*dst++ = v;
		*dst++ = v;
	}
	
	display_gl_bind_buffer(GL_ARRAY_BUFFER, display->memory_vertex_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, display->memory_size * 4, display->memory_temp_buffer);
}

int			display_key_pressed(t_display* display, int key)
{
	return glfwGetKey(display->window, key) == GLFW_PRESS;
}


void display_render_memory(struct s_vm* vm, t_display* display, t_mat4* projection)
{
	float color_mem[] = { 0.4, 0.4, 0.8, 1 };

	display_update_memory(vm, display);
	glUseProgram(display->memory_shader.id);
	glUniformMatrix4fv(display->memory_uniform_projection_matrix, 1, GL_FALSE, projection->mat.v);
	glUniform4fv(display->memory_uniform_color, 1, color_mem);

	glBindTexture(GL_TEXTURE_2D, display->hex_texture);
	display_gl_bind_vao(display->memory_vao);	
	display_gl_bind_buffer(GL_ELEMENT_ARRAY_BUFFER, display->memory_grid_index_buffer);
	glDrawElements(GL_TRIANGLES, display->memory_index_count, GL_UNSIGNED_SHORT, 0);
}

void display_render_io_read(struct s_vm* vm, t_display* display, t_mat4* projection)
{
	float	color_io_read[] = { 1.0f, 0.4f, 0.4f, 0.0f };
	int		i, j;
	uint8*  dst;
	int		size = vm->memory->size;

	dst = (uint8*)display->memory_temp_buffer;
	memset(dst, 0, display->memory_size * 4);
	for (i = 0; i < vm->process_count; ++i)
	{
		t_process* process = vm->processes[i];
		for (j = 0; j < process->memory_read_op_count; ++j)
		{
			int index = process->memory_read_op[j] * 4;
			dst[index + 0]++;
			dst[index + 1]++;
			dst[index + 2]++;
			dst[index + 3]++;
		}
	}

	display_gl_bind_buffer(GL_ARRAY_BUFFER, display->memory_vertex_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, display->memory_size * 4, display->memory_temp_buffer);

	glUseProgram(display->io_shader.id);
	glUniformMatrix4fv(display->io_uniform_projection_matrix, 1, GL_FALSE, projection->mat.v);
	glUniform4fv(display->io_uniform_color, 1, color_io_read);
	display_gl_bind_vao(display->memory_vao);
	display_gl_bind_buffer(GL_ELEMENT_ARRAY_BUFFER, display->memory_grid_index_buffer);
	glDrawElements(GL_TRIANGLES, display->memory_index_count, GL_UNSIGNED_SHORT, 0);
}

void display_render_io_write(struct s_vm* vm, t_display* display, t_mat4* projection)
{
	float	color_io_write[] = { 0.4f, 1.0f, 0.4f, 0.0f };
	uint8*	dst;
	int		size = vm->memory->size;
	int		i, j;		

	dst = (uint8*)display->memory_temp_buffer;
	memset(dst, 0, display->memory_size * 4);

	for (i = 0; i < vm->process_count; ++i)
	{
		t_process* process = vm->processes[i];
		for (j = 0; j < process->memory_write_op_count; ++j)
		{
			int index = process->memory_write_op[j] * 4;
			dst[index + 0]++;
			dst[index + 1]++;
			dst[index + 2]++;
			dst[index + 3]++;
		}
	}

	display_gl_bind_buffer(GL_ARRAY_BUFFER, display->memory_vertex_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, display->memory_size * 4, display->memory_temp_buffer);

	glUseProgram(display->io_shader.id);
	glUniformMatrix4fv(display->io_uniform_projection_matrix, 1, GL_FALSE, projection->mat.v);
	glUniform4fv(display->io_uniform_color, 1, color_io_write);
	display_gl_bind_vao(display->memory_vao);
	display_gl_bind_buffer(GL_ELEMENT_ARRAY_BUFFER, display->memory_grid_index_buffer);
	glDrawElements(GL_TRIANGLES, display->memory_index_count, GL_UNSIGNED_SHORT, 0);
}

void display_render_io_process(struct s_vm* vm, t_display* display, t_mat4* projection)
{
	t_v4	color_io_process = { 0.4f, 0.4f, 1.0f, 0.0f };
	t_v4	color_ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
	t_v3	light_direction = { 0, 0, -1.0f };
	t_mat4	local;
	int		i;

	display_mesh_render_start(display->mesh_renderer, MESH_TYPE_VN);
	display_mesh_set_ambient(display->mesh_renderer, &color_ambient);
	display_mesh_set_light_direction(display->mesh_renderer, &light_direction);
	display_mesh_set_diffuse(display->mesh_renderer, &color_io_process);
	display_mesh_set_projection(display->mesh_renderer, projection);
	mat4_ident(&local);
	for (i = 0; i < vm->process_count; ++i)
	{
		t_process* process = vm->processes[i];
		int index = process->pc;
		float x = index % display->memory_stride;
		float y = index / display->memory_stride;
		
		x = x * DISPLAY_CELL_SIZE + DISPLAY_CELL_SIZE * 0.5f;
		y = y * DISPLAY_CELL_SIZE + DISPLAY_CELL_SIZE * 0.5f;
		mat4_translate(&local, x, y, DISPLAY_CELL_SIZE * 0.5f);
		display_mesh_set_local(display->mesh_renderer, &local);
		display_mesh_render(display->process_mesh);
	}
}

int32 display_update_input(t_display* display)
{
	int32 moved = 0;
	double mouse_cur_x, mouse_cur_y;
	double mouse_delta_x, mouse_delta_y;
	
	double current_time = glfwGetTime();
	double delta = display->frame_last_time - current_time;

	double framebuffer_center_x = (double)display->frame_buffer_width * 0.5f;
	double framebuffer_center_y = (double)display->frame_buffer_height * 0.5f;

	display->frame_last_time = current_time;

	glfwGetCursorPos(display->window, &mouse_cur_x, &mouse_cur_y);

	mouse_delta_x = display->mouse_prev_x - mouse_cur_x;
	mouse_delta_y = display->mouse_prev_y - mouse_cur_y;

	display->mouse_prev_x = mouse_cur_x;
	display->mouse_prev_y = mouse_cur_y;


	if (glfwGetKey(display->window, GLFW_KEY_Q) == GLFW_PRESS)
		display->display_zoom += (float)delta, moved = 1;

	if (glfwGetKey(display->window, GLFW_KEY_A) == GLFW_PRESS)	
		display->display_zoom -= (float)delta, moved = 1;

	if (display->display_zoom < 0.01f)
		display->display_zoom = 0.01f;

	if (moved)
	{
		double mouse_target_center_x, mouse_target_center_y;

		mouse_target_center_x = ((mouse_cur_x - framebuffer_center_x) / (double)display->frame_buffer_width) * 1.0f / display->display_zoom;
		mouse_target_center_y = ((mouse_cur_y - framebuffer_center_y) / (double)display->frame_buffer_height) * 1.0f / display->display_zoom;
		printf("=> %f %f\n", mouse_target_center_x, mouse_target_center_y);
		display->display_center_x += (float)(mouse_target_center_x);
		display->display_center_y += (float)(mouse_target_center_y);
	}


	if (glfwGetMouseButton(display->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		display->display_center_x += (float) mouse_delta_x * display->display_zoom;
		display->display_center_y += (float) mouse_delta_y * display->display_zoom;
		moved = 1;
	}
	return moved;
}

void display_update_camera(t_display* display, t_mat4* view)
{
	float width = display->frame_buffer_width  * display->display_zoom * 0.5f;
	float height = display->frame_buffer_height * display->display_zoom * 0.5f;

	mat4_ident(view);

	mat4_ortho(view,
		display->display_center_x - width,
		display->display_center_x + width,
		display->display_center_y + height,
		display->display_center_y - height, 
		0.0f, 10.0f);
}

void display_step(struct s_vm* vm, t_display* display)
{
	t_mat4 mat;

	glfwGetFramebufferSize(display->window, &display->frame_buffer_width, &display->frame_buffer_height);
	display->frame_buffer_ratio = (float)display->frame_buffer_width / (float)display->frame_buffer_height;	

	display_update_camera(display, &mat);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, display->frame_buffer_width, display->frame_buffer_height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	display_render_memory(vm, display, &mat);

	display_render_io_read(vm, display, &mat);
	display_render_io_write(vm, display, &mat);
	display_render_io_process(vm, display, &mat);

	glfwSwapBuffers(display->window);
	glfwPollEvents();
	
}

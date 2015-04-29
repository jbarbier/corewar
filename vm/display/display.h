#ifndef DISPLAY_H
#define DISPLAY_H


#include	"../../common/types.h"


#define DISPLAY_CELL_SIZE				10.f
#define DISPLAY_OK						1


struct s_display;
struct s_vm;
typedef struct s_vm	t_vm;
typedef struct s_display t_display;

t_display*	display_initialize(int width, int height);
int32		display_should_exit(t_display* display);
void		display_destroy(t_display* display);
void		display_step(struct s_vm* vm, t_display* display);
int			display_key_pressed(t_display* display, int key);
int32		display_update_input(t_display* display);
#endif
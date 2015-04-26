#ifndef DISPLAY_H
#define DISPLAY_H

#include "types.h"

struct s_display;
typedef struct s_display t_display;

t_display*	display_initialize();
int32		display_should_exit(t_display* display);
void		display_destroy(t_display* display);
void		display_step(struct s_vm* vm, t_display* display);

#endif
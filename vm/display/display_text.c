#include <stdlib.h>
#include "display.h"
#include "display_gl.h"
#include "display_math.h"
#include "display_text.h"

#define MAX_TEXT	100

typedef struct s_text
{
	int32	x;
	int32	y;
	char*	text;
} t_text;

typedef struct s_display_text
{
	t_text	texts[MAX_TEXT];
	int32	text_count;
} t_display_text;


t_display_text* display_text_intialize()
{
	t_display_text* text = malloc(sizeof(t_display_text));
	text->text_count = 0;
	return text;
}

void			display_text__destroy(t_display_text* display)
{
	free(display);
}

void			display_text_add(t_display_text* display, int x, int y, char* format, ...)
{

}

void			display_text_render(t_display_text* display)
{

}

#ifndef DISPLAY_TEXT_H
#define DISPLAY_TEXT_H


struct s_display_text;
typedef struct s_display_text t_display_text;

t_display_text* display_text_intialize();
void			display_text_destroy(t_display_text* texts);
void			display_text_add(t_display_text* texts, float x, float y, int32 rgba, char* format, ...);
void			display_text_render(t_display_text* texts, t_mat4* projection_view);
void			display_text_clear(t_display_text* texts);
#endif
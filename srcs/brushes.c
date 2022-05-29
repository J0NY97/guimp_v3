#include "guimp.h"

void	draw_brush(t_guimp *guimp, t_layer *active_layer,
	t_vec2i actual_pos, t_vec2i hidden_pos)
{
	ui_surface_circle_draw(guimp->hidden_surface, hidden_pos, guimp->size,
		guimp->combined_color);
	if (guimp->win_main->mouse_down != SDL_BUTTON_LEFT)
	{
		guimp->first_set = 0;
		return ;
	}
	if (guimp->first_set)
	{
		ui_surface_circle_draw_filled(active_layer->surface,
			guimp->first_pos_converted, guimp->size, guimp->combined_color);
		ui_surface_line_draw_thicc(active_layer->surface,
			(t_vec2i []){guimp->first_pos_converted, actual_pos}, guimp->size,
			guimp->combined_color);
		ui_surface_circle_draw_filled(active_layer->surface,
			actual_pos, guimp->size, guimp->combined_color);
	}
	guimp->first_pos_converted = actual_pos;
	guimp->first_set = 1;
}

void	text_brush(t_guimp *guimp, t_layer *active_layer,
	t_vec2i actual_pos, t_vec2i hidden_pos)
{
	SDL_Surface		*surface;
	char			*font_path;
	char			*full_font;

	font_path = ui_dropdown_active_text(guimp->font_dropdown);
	if (!font_path)
		return ;
	guimp->text_input_str = ui_input_get_text(guimp->text_input);
	if (!guimp->text_input_str || ft_strequ(guimp->text_input_str, ""))
		return ;
	full_font = ft_strjoiner("fonts/", font_path, ".ttf", NULL);
	surface = ui_surface_text_new(guimp->text_input_str,
			full_font, guimp->size, guimp->combined_color);
	ft_strdel(&full_font);
	if (!surface)
	{
		ft_printf("[%s] Surface couldn\'t be created.\n", __FUNCTION__);
		return ;
	}
	SDL_BlitScaled(surface, NULL, guimp->hidden_surface,
		&(SDL_Rect){hidden_pos.x, hidden_pos.y, surface->w, surface->h});
	if (guimp->win_main->mouse_down == SDL_BUTTON_LEFT)
		SDL_BlitSurface(surface, NULL, active_layer->surface,
			&(SDL_Rect){actual_pos.x, actual_pos.y, surface->w, surface->h});
	SDL_FreeSurface(surface);
}

void	erase_brush(t_guimp *guimp, t_layer *active_layer,
	t_vec2i actual_pos, t_vec2i hidden_pos)
{
	ui_surface_circle_draw(guimp->hidden_surface,
		hidden_pos, guimp->size, 0xffb0b0b0);
	if (guimp->win_main->mouse_down != SDL_BUTTON_LEFT)
		return ;
	ui_surface_circle_draw_filled(active_layer->surface,
		actual_pos, guimp->size, 0x00000000);
}

void	flood_brush(t_guimp *guimp, t_layer *active_layer, t_vec2i actual_pos)
{
	if (guimp->win_main->mouse_down == SDL_BUTTON_LEFT)
		flood_fill(active_layer->surface,
			guimp->combined_color, actual_pos.x, actual_pos.y);
}

void	move_brush(t_guimp *guimp, t_layer *active_layer)
{
	t_vec2i	mouse_pos;

	if (guimp->win_main->mouse_down != SDL_BUTTON_LEFT)
		return ;
	mouse_pos.x = guimp->win_main->mouse_pos.x
		- guimp->win_main->mouse_pos_prev.x;
	mouse_pos.y = guimp->win_main->mouse_pos.y
		- guimp->win_main->mouse_pos_prev.y;
	active_layer->pos.x += mouse_pos.x / guimp->zoom;
	active_layer->pos.y += mouse_pos.y / guimp->zoom;
}

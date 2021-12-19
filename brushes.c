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
			guimp->first_pos_converted, actual_pos, guimp->size,
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

void	sticker_brush(t_guimp *guimp, t_layer *active_layer,
	t_vec2i actual_pos, t_vec2i hidden_pos)
{
	SDL_Surface		*surface;
	char			*sticker_text;
	char			*full_path;
	char			*including_type;

	sticker_text = ui_dropdown_active_text(guimp->sticker_dropdown);
	if (!sticker_text)
		return ;
	full_path = ft_strjoin("stickers/", sticker_text);
	// Checking png
	including_type = ft_strjoin(full_path, ".png");
	// Checking jpg
	if (access(including_type, F_OK))
	{
		ft_strdel(&including_type);
		including_type = ft_strjoin(full_path, ".jpg");
	}
	// Checking bmp
	if (access(including_type, F_OK))
	{
		ft_strdel(&including_type);
		including_type = ft_strjoin(full_path, ".bmp");
	}
	surface = ui_surface_image_new(including_type);
	ft_strdel(&including_type);
	ft_strdel(&full_path);
	if (!surface)
	{
		ft_printf("[%s] Sticker image doesnt exist.\n", __FUNCTION__);
		return ;
	}
	SDL_BlitScaled(surface, NULL, guimp->hidden_surface,
		&(SDL_Rect){hidden_pos.x, hidden_pos.y, surface->w, surface->h});
	if (guimp->win_main->mouse_down == SDL_BUTTON_LEFT)
		SDL_BlitSurface(surface, NULL, active_layer->surface,
			&(SDL_Rect){actual_pos.x, actual_pos.y, surface->w, surface->h});
	SDL_FreeSurface(surface);
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

void	line_tool_drawer(t_guimp *guimp, SDL_Surface *surface,
	t_vec2i pos1, t_vec2i pos2)
{
	if (guimp->size > 1)
	{
		ui_surface_circle_draw_filled(surface,
			pos1, guimp->size, guimp->combined_color);
		ui_surface_line_draw_thicc(surface, pos1,
			pos2, guimp->size, guimp->combined_color);
		ui_surface_circle_draw_filled(surface,
			pos2, guimp->size, guimp->combined_color);
	}
	else
	{
		ui_surface_line_draw(surface, pos1, pos2, guimp->combined_color);
	}
}

void	line_tool_brush(t_guimp *guimp, t_layer *active_layer,
	t_vec2i actual_pos, t_vec2i hidden_pos)
{
	if (guimp->first_set)
		line_tool_drawer(guimp, guimp->hidden_surface,
			guimp->first_pos, hidden_pos);
	if (guimp->win_main->mouse_down_last_frame != SDL_BUTTON_LEFT)
		return ;
	if (!guimp->first_set)
	{
		guimp->first_pos_converted = actual_pos;
		guimp->first_pos = hidden_pos;
		guimp->first_set = 1;
	}
	else
	{
		line_tool_drawer(guimp, active_layer->surface,
			guimp->first_pos_converted, actual_pos);
		guimp->first_set = 0;
	}
}

void	square_tool_brush(t_guimp *guimp, t_layer *active_layer,
	t_vec2i actual_pos, t_vec2i hidden_pos)
{
	if (guimp->first_set)
		ui_surface_rect_draw(guimp->hidden_surface,
			guimp->first_pos, hidden_pos, guimp->combined_color);
	if (guimp->win_main->mouse_down_last_frame != SDL_BUTTON_LEFT)
		return ;
	if (!guimp->first_set)
	{
		guimp->first_pos_converted = actual_pos;
		guimp->first_pos = hidden_pos;
		guimp->first_set = 1;
	}
	else
	{
		ui_surface_rect_draw(active_layer->surface, guimp->first_pos_converted,
			actual_pos, guimp->combined_color);
		guimp->first_set = 0;
	}
}

void	circle_tool_brush(
	t_guimp *guimp, t_layer *active_layer,
	t_vec2i actual_pos, t_vec2i hidden_pos)
{
	if (guimp->first_set)
	{
		ui_surface_circle_draw(guimp->hidden_surface, guimp->first_pos,
			dist(guimp->first_pos, hidden_pos), guimp->combined_color);
	}
	if (guimp->win_main->mouse_down_last_frame != SDL_BUTTON_LEFT)
		return ;
	if (!guimp->first_set)
	{
		guimp->first_pos_converted = actual_pos;
		guimp->first_pos = hidden_pos;
		guimp->first_set = 1;
	}
	else
	{
		ui_surface_circle_draw(active_layer->surface,
			guimp->first_pos_converted,
			dist(guimp->first_pos_converted, actual_pos),
			guimp->combined_color);
		guimp->first_set = 0;
	}
}

/*
 * unselect on right click;
 *	done here so that it applies to all the shape button tools;
 */
void	shape_brush(
	t_guimp *guimp, t_layer *active_layer,
	t_vec2i actual_pos, t_vec2i hidden_pos)
{
	if (guimp->win_main->mouse_down_last_frame == SDL_BUTTON_RIGHT)
	{
		guimp->first_set = 0;
		return ;
	}
	if (guimp->line_button->state == UI_STATE_CLICK)
		line_tool_brush(guimp, active_layer, actual_pos, hidden_pos);
	else if (guimp->square_button->state == UI_STATE_CLICK)
		square_tool_brush(guimp, active_layer, actual_pos, hidden_pos);
	else if (guimp->circle_button->state == UI_STATE_CLICK)
		circle_tool_brush(guimp, active_layer, actual_pos, hidden_pos);
}

/*
 * NOTE:
 *	actual_pos is the final_image position,
 *		instead of layer position like all the other brushes;
*/
void	pipette_brush(t_guimp *guimp, t_vec2i actual_pos)
{
	if (guimp->win_main->mouse_down_last_frame != SDL_BUTTON_LEFT)
		return ;
	set_sliders_to_color(guimp,
		ui_surface_pixel_get(guimp->final_image.surface,
			actual_pos.x, actual_pos.y));
}

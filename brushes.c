#include "guimp.h"

void	draw_brush(t_guimp *guimp, t_layer *active_layer, t_vec2i actual_pos, t_vec2i hidden_pos)
{
	ui_surface_circle_draw(guimp->hidden_surface, hidden_pos, guimp->size, guimp->combined_color);
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
			guimp->first_pos_converted, actual_pos, guimp->size, guimp->combined_color);
		ui_surface_circle_draw_filled(active_layer->surface,
			actual_pos, guimp->size, guimp->combined_color);
	}
	guimp->first_pos_converted = actual_pos;
	guimp->first_set = 1;
}

void	text_brush(t_guimp *guimp, t_layer *active_layer, t_vec2i actual_pos, t_vec2i hidden_pos)
{
	SDL_Surface		*surface;
	t_ui_label		*label;
	t_ui_dropdown	*drop;
	char			*full_font;

	drop = ui_dropdown_get_dropdown(guimp->font_dropdown);
	if (!drop->active)
		return ;
	label = ui_button_get_label(drop->active);
	guimp->text_input_str = ui_input_get_label(guimp->text_input)->text;
	full_font = ft_strjoiner("fonts/", label->text, ".ttf", NULL);
	surface = ui_surface_text_new(guimp->text_input_str, full_font, guimp->size, guimp->combined_color);
	ft_strdel(&full_font);
	if (!surface)
	{
		ft_printf("[%s] Font doesnt exist.\n", __FUNCTION__);
		return ;
	}
	SDL_BlitScaled(surface, NULL, guimp->hidden_surface, &(SDL_Rect){hidden_pos.x, hidden_pos.y, surface->w, surface->h});
	if (guimp->win_main->mouse_down == SDL_BUTTON_LEFT)
		SDL_BlitSurface(surface, NULL, active_layer->surface, &(SDL_Rect){actual_pos.x, actual_pos.y, surface->w, surface->h});
	SDL_FreeSurface(surface);
}

void	erase_brush(t_guimp *guimp, t_layer *active_layer, t_vec2i actual_pos, t_vec2i hidden_pos)
{
	ui_surface_circle_draw(guimp->hidden_surface, hidden_pos, guimp->size, 0xffb0b0b0);
	if (guimp->win_main->mouse_down != SDL_BUTTON_LEFT)
		return ;
	ui_surface_circle_draw_filled(active_layer->surface,
		actual_pos, guimp->size, 0x00000000);
}

void	flood_brush(t_guimp *guimp, t_layer *active_layer, t_vec2i actual_pos)
{
	if (guimp->win_main->mouse_down == SDL_BUTTON_LEFT)
		flood_fill(active_layer->surface, guimp->combined_color, actual_pos.x , actual_pos.y);
}

void	sticker_brush(t_guimp *guimp, t_layer *active_layer, t_vec2i actual_pos, t_vec2i hidden_pos)
{
	SDL_Surface		*surface;
	t_ui_label		*label;
	t_ui_dropdown	*drop;
	char			*full_path;
	char			*including_type;

	drop = ui_dropdown_get_dropdown(guimp->sticker_dropdown);
	if (!drop->active)
		return ;
	label = ui_button_get_label(drop->active);
	full_path = ft_strjoin("stickers/", label->text);
	// Checking png
	including_type = ft_strjoin(full_path, ".png");
	surface = ui_surface_image_new(including_type);
	// Checking jpg
	if (!surface)
	{
		ft_strdel(&including_type);
		including_type = ft_strjoin(full_path, ".jpg");
		surface = ui_surface_image_new(including_type);
	}
	// Checking bmp
	if (!surface)
	{
		ft_strdel(&including_type);
		including_type = ft_strjoin(full_path, ".bmp");
		surface = ui_surface_image_new(including_type);
	}
	ft_strdel(&including_type);
	ft_strdel(&full_path);
	if (!surface)
	{
		ft_printf("[%s] Sticker image doesnt exist.\n", __FUNCTION__);
		return ;
	}
	SDL_BlitScaled(surface, NULL, guimp->hidden_surface, &(SDL_Rect){hidden_pos.x, hidden_pos.y, surface->w, surface->h});
	if (guimp->win_main->mouse_down == SDL_BUTTON_LEFT)
		SDL_BlitSurface(surface, NULL, active_layer->surface, &(SDL_Rect){actual_pos.x, actual_pos.y, surface->w, surface->h});
	SDL_FreeSurface(surface);
}

void	move_brush(t_guimp *guimp, t_layer *active_layer)
{
	t_vec2i	mouse_pos;

	if (guimp->win_main->mouse_down != SDL_BUTTON_LEFT)
		return ;
	mouse_pos.x = guimp->win_main->mouse_pos.x - guimp->win_main->mouse_pos_prev.x;
	mouse_pos.y = guimp->win_main->mouse_pos.y - guimp->win_main->mouse_pos_prev.y;
	active_layer->pos.x += mouse_pos.x / guimp->zoom;
	active_layer->pos.y += mouse_pos.y / guimp->zoom;
}

void	line_tool_brush(t_guimp *guimp, t_layer *active_layer, t_vec2i actual_pos, t_vec2i hidden_pos)
{
	if (guimp->first_set)
	{
		if (guimp->size > 1)
		{
			ui_surface_circle_draw_filled(guimp->hidden_surface, guimp->first_pos, guimp->size, guimp->combined_color);
			ui_surface_line_draw_thicc(guimp->hidden_surface, guimp->first_pos, hidden_pos, guimp->size, guimp->combined_color);
			ui_surface_circle_draw_filled(guimp->hidden_surface, hidden_pos, guimp->size, guimp->combined_color);
		}
		else
		{
			ui_surface_line_draw(guimp->hidden_surface, guimp->first_pos, hidden_pos, guimp->combined_color);
		}
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
		if (guimp->size > 1)
		{
			ui_surface_circle_draw_filled(active_layer->surface,
				guimp->first_pos_converted, guimp->size, guimp->combined_color);
			ui_surface_line_draw_thicc(active_layer->surface,
				guimp->first_pos_converted, actual_pos, guimp->size, guimp->combined_color);
			ui_surface_circle_draw_filled(active_layer->surface,
				actual_pos, guimp->size, guimp->combined_color);
		}
		else
		{
			ui_surface_line_draw(active_layer->surface,
				guimp->first_pos_converted, actual_pos, guimp->combined_color);
		}
		guimp->first_set = 0;
	}
}

void	square_tool_brush(t_guimp *guimp, t_layer *active_layer, t_vec2i actual_pos, t_vec2i hidden_pos)
{
	if (guimp->first_set)
		ui_surface_rect_draw(guimp->hidden_surface, guimp->first_pos, hidden_pos, guimp->combined_color);
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

void	circle_tool_brush(t_guimp *guimp, t_layer *active_layer, t_vec2i actual_pos, t_vec2i hidden_pos)
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
		ui_surface_circle_draw(active_layer->surface, guimp->first_pos_converted,
			dist(guimp->first_pos_converted, actual_pos), guimp->combined_color);
		guimp->first_set = 0;
	}
}

void	shape_brush(t_guimp *guimp, t_layer *active_layer, t_vec2i actual_pos, t_vec2i hidden_pos)
{
	if (guimp->win_main->mouse_down_last_frame == SDL_BUTTON_RIGHT) // unselect on right click; done here so that it applies to all the shape button tools;
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
 * actual_pos is the final_image position, instead of layer position like all the other brushes;
*/
void	pipette_brush(t_guimp *guimp, t_vec2i actual_pos)
{
	if (guimp->win_main->mouse_down_last_frame != SDL_BUTTON_LEFT)
		return ;
	set_sliders_to_color(guimp, ui_surface_pixel_get(guimp->final_image.surface, actual_pos.x, actual_pos.y));
}
#include "guimp.h"

/*
 * Not to confuse with the elements,
 * these are the actual drawing layers.
*/
void	layer_new(t_layer *layer, char *name, t_vec4i pos, bool *show)
{
	layer->name = ft_strdup(name);
	layer->id = -1;
	layer->pos = pos;
	layer->surface = ui_surface_new(pos.w, pos.h);
	layer->show = show;
	SDL_FillRect(layer->surface, NULL, 0x00000000);
}

/*
 * takes in a non malloced pointer, so dont free actual 'layer';
*/
void	layer_free(t_layer *layer)
{
	ft_strdel(&layer->name);	
	SDL_FreeSurface(layer->surface);
	layer->show = NULL;
}

void	resize_layer(t_layer *layer, t_vec2i wh)
{
	SDL_Surface	*new_surface;

	if (!layer)
	{
		layer_new(layer, "no_name_given", vec4i(0, 0, wh.x, wh.y), NULL);
		return ;
	}
	new_surface = ui_surface_new(wh.x, wh.y);
	SDL_BlitScaled(layer->surface, NULL, new_surface, NULL);
	layer->pos.w = wh.x;
	layer->pos.h = wh.y;
	SDL_FreeSurface(layer->surface);
	layer->surface = new_surface;
}

/*
 * Selecting the layer and stuff.
*/
void	layer_event(t_guimp *guimp)
{
	// Element event
	int				ii;
	t_ui_element	*button;

	ii = -1;
	while (++ii < guimp->layer_amount)
	{
		button = ui_list_get_element_by_id(guimp->layer_elems[ii]->children, "layer_select_button");
		if (ui_button(button))
		{
			guimp->selected_layer = ii;
			button->is_click = 1;
			button->state = UI_STATE_CLICK;
		}
	}
	// Other layer events
	if (SDL_GetModState() & KMOD_LCTRL && guimp->win_main->scroll)
	{
		int	prev_w = guimp->final_image.pos.w * guimp->zoom;
		int	prev_h = guimp->final_image.pos.h * guimp->zoom;
		guimp->zoom = ft_fclamp(guimp->zoom + (float)guimp->win_main->scroll / 10.0f, 0.1f, 100.0f);
		int	next_w = guimp->final_image.pos.w * guimp->zoom;
		int	next_h = guimp->final_image.pos.h * guimp->zoom;
		guimp->final_image.pos.x += (prev_w - next_w) / 2;
		guimp->final_image.pos.y += (prev_h - next_h) / 2;
	}
	else if (guimp->win_main->mouse_down == 2) // move whole image by holding middle mouse and mouse motioning
	{
		guimp->final_image.pos.x += guimp->win_main->mouse_pos.x - guimp->win_main->mouse_pos_prev.x;
		guimp->final_image.pos.y += guimp->win_main->mouse_pos.y - guimp->win_main->mouse_pos_prev.y;
	}
}

/*
 * NOTE:
 * When drawing you need to take into consideration the position of the image, the layer, and the zoom...
 * ...and maybe other things in the future.A
*/
void	layer_draw(t_guimp *guimp)
{
	t_vec2i	relative_mouse_pos;

	if (guimp->selected_layer < 0 || guimp->selected_layer >= guimp->layer_amount)
		return ;
	t_layer	*active_layer = &guimp->layers[guimp->selected_layer];
	relative_mouse_pos.x = guimp->win_main->mouse_pos.x - (guimp->final_image.pos.x + active_layer->pos.x);
	relative_mouse_pos.y = guimp->win_main->mouse_pos.y - (guimp->final_image.pos.y + active_layer->pos.y);
	float aspect_x = ((float)active_layer->pos.w / ((float)active_layer->pos.w * guimp->zoom));
	float aspect_y = ((float)active_layer->pos.h / ((float)active_layer->pos.h * guimp->zoom));
	t_vec2i	actual_pos = vec2i(
			relative_mouse_pos.x * aspect_x,
			relative_mouse_pos.y * aspect_y);
//	if (guimp->win_main->mouse_down == 1) // if left click on the win_main
	{
		if (guimp->draw_button->state == UI_STATE_CLICK) // basic draw
		{
			ui_surface_circle_draw(guimp->hidden_surface,
				guimp->win_main->mouse_pos, guimp->size * guimp->zoom, guimp->combined_color);
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
				guimp->first_set = 0;
			}
			guimp->first_pos_converted = actual_pos;
			guimp->first_set = 1;
		}
		else if (guimp->text_button->state == UI_STATE_CLICK) // text
		{
			SDL_Surface		*surface;
			t_ui_label		*label;
			t_ui_dropdown	*drop;
			char			*full_font;

			drop = ui_dropdown_get_dropdown(guimp->font_dropdown);
			if (!drop->active)
				return ;
			label = ui_button_get_label(drop->active);
			guimp->text_input_str = ui_input_label_get(guimp->text_input)->text;
			full_font = ft_strjoiner("fonts/", label->text, ".ttf", NULL);
			surface = ui_surface_text_new(guimp->text_input_str, full_font, guimp->size, guimp->combined_color);
			ft_strdel(&full_font);
			if (!surface)
			{
				ft_printf("[%s] Font doesnt exist.\n", __FUNCTION__);
				return ;
			}
			SDL_BlitScaled(surface, NULL, guimp->hidden_surface, &(SDL_Rect){guimp->win_main->mouse_pos.x, guimp->win_main->mouse_pos.y, surface->w * guimp->zoom, surface->h * guimp->zoom});
			if (guimp->win_main->mouse_down == SDL_BUTTON_LEFT)
				SDL_BlitSurface(surface, NULL, active_layer->surface, &(SDL_Rect){actual_pos.x, actual_pos.y, surface->w, surface->h});
			SDL_FreeSurface(surface);
		}
		else if (guimp->erase_button->state == UI_STATE_CLICK) // erase
		{
			ui_surface_circle_draw(guimp->hidden_surface,
				guimp->win_main->mouse_pos, guimp->size * guimp->zoom,
				0xffb0b0b0);
			if (guimp->win_main->mouse_down != SDL_BUTTON_LEFT)
				return ;
			ui_surface_circle_draw_filled(active_layer->surface,
				actual_pos, guimp->size, 0x00000000);
		}
		else if (guimp->flood_button->state == UI_STATE_CLICK) // flood fill
		{
			if (guimp->win_main->mouse_down == SDL_BUTTON_LEFT)
				flood_fill(active_layer->surface, guimp->combined_color, actual_pos.x , actual_pos.y);
		}
		else if (guimp->sticker_button->state == UI_STATE_CLICK) // sticker
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
			SDL_BlitScaled(surface, NULL, guimp->hidden_surface, &(SDL_Rect){guimp->win_main->mouse_pos.x, guimp->win_main->mouse_pos.y, surface->w * guimp->zoom, surface->h * guimp->zoom});
			if (guimp->win_main->mouse_down == SDL_BUTTON_LEFT)
				SDL_BlitSurface(surface, NULL, active_layer->surface, &(SDL_Rect){actual_pos.x, actual_pos.y, surface->w, surface->h});
			SDL_FreeSurface(surface);
		}
		else if (guimp->move_button->state == UI_STATE_CLICK) // move
		{
			t_vec2i	mouse_pos;

			if (guimp->win_main->mouse_down != SDL_BUTTON_LEFT)
				return ;
			mouse_pos.x = guimp->win_main->mouse_pos.x - guimp->win_main->mouse_pos_prev.x;
			mouse_pos.y = guimp->win_main->mouse_pos.y - guimp->win_main->mouse_pos_prev.y;
			guimp->layers[guimp->selected_layer].pos.x += mouse_pos.x / guimp->zoom;
			guimp->layers[guimp->selected_layer].pos.y += mouse_pos.y / guimp->zoom;
		}
		else if (guimp->shape_button->state == UI_STATE_CLICK)
		{
			if (guimp->line_button->state == UI_STATE_CLICK) // line tool
			{
				if (guimp->first_set)
				{
					if (guimp->size > 1)
					{
						ui_surface_circle_draw_filled(guimp->hidden_surface,
							guimp->first_pos, guimp->size, guimp->combined_color);
						ui_surface_line_draw_thicc(guimp->hidden_surface,
							guimp->first_pos,
							guimp->win_main->mouse_pos,
							guimp->size,
							guimp->combined_color);
						ui_surface_circle_draw_filled(guimp->hidden_surface,
							guimp->win_main->mouse_pos, guimp->size, guimp->combined_color);
					}
					else
					{
						ui_surface_line_draw(guimp->hidden_surface,
							guimp->first_pos,
							guimp->win_main->mouse_pos,
							guimp->combined_color);
					}
				}
				if (guimp->win_main->mouse_down_last_frame != SDL_BUTTON_LEFT)
					return ;
				if (!guimp->first_set)
				{
					guimp->first_pos_converted = actual_pos;
					guimp->first_pos = guimp->win_main->mouse_pos;
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
			else if (guimp->square_button->state == UI_STATE_CLICK) // rect tool
			{
				if (guimp->first_set)
					ui_surface_rect_draw(guimp->hidden_surface, guimp->first_pos,
						guimp->win_main->mouse_pos, guimp->combined_color);
				if (guimp->win_main->mouse_down_last_frame != SDL_BUTTON_LEFT)
					return ;
				if (!guimp->first_set)
				{
					guimp->first_pos_converted = actual_pos;
					guimp->first_pos = guimp->win_main->mouse_pos;
					guimp->first_set = 1;
				}
				else
				{
					ui_surface_rect_draw(active_layer->surface, guimp->first_pos_converted,
						actual_pos, guimp->combined_color);
					guimp->first_set = 0;
				}
			}
			else if (guimp->circle_button->state == UI_STATE_CLICK) // circle tool
			{
				if (guimp->first_set)
				{
					ui_surface_circle_draw(guimp->hidden_surface, guimp->first_pos,
						dist(guimp->first_pos, guimp->win_main->mouse_pos), guimp->combined_color);
				}
				if (guimp->win_main->mouse_down_last_frame != SDL_BUTTON_LEFT)
					return ;
				if (!guimp->first_set)
				{
					guimp->first_pos_converted = actual_pos;
					guimp->first_pos = guimp->win_main->mouse_pos;
					guimp->first_set = 1;
				}
				else
				{
					ui_surface_circle_draw(active_layer->surface, guimp->first_pos_converted,
						dist(guimp->first_pos_converted, actual_pos), guimp->combined_color);
					guimp->first_set = 0;
				}
			}
		}
		else if (guimp->pipette_button->state == UI_STATE_CLICK) // pipette
		{
			if (guimp->win_main->mouse_down_last_frame != SDL_BUTTON_LEFT)
				return ;
			float img_aspect_x = ((float)guimp->final_image.pos.w / ((float)guimp->final_image.pos.w * guimp->zoom));
			float img_aspect_y = ((float)guimp->final_image.pos.h / ((float)guimp->final_image.pos.h * guimp->zoom));
			t_vec2i	img_pos = vec2i(
					guimp->win_main->mouse_pos.x - guimp->final_image.pos.x,
					guimp->win_main->mouse_pos.y - guimp->final_image.pos.y);
			set_sliders_to_color(guimp, ui_surface_pixel_get(guimp->final_image.surface, img_pos.x * img_aspect_x, img_pos.y * img_aspect_y));
		}
	}
}

/*
 * This render all the layers on the image layer and then renders the image layer on the main window.
 * All the layers will be rendered on the image no matter what, in the order ordererd by user,
 * then when we have image layer done, make texture out of it, then render it on the window at the correct
 * zoom lvl and correct position.
*/
void	layer_render(t_guimp *guimp)
{
	int	ii;

	// Reset final image before redrawing;
	SDL_FillRect(guimp->final_image.surface, NULL, 0xff000000);
	// Blit all the layer surfaces on the final image surface;
	ii = -1;
	while (++ii < guimp->layer_amount)
	{
		if (!*guimp->layers[ii].show)
			continue;
		SDL_BlitSurface(guimp->layers[ii].surface, NULL, guimp->final_image.surface,
			&(SDL_Rect){guimp->layers[ii].pos.x, guimp->layers[ii].pos.y,
				guimp->layers[ii].pos.w, guimp->layers[ii].pos.h});
	}
	// Make dotted outline around the final image dimensions;
	ui_surface_rect_draw(guimp->hidden_surface,
		vec2i(guimp->final_image.pos.x, guimp->final_image.pos.y),
		vec2i(guimp->final_image.pos.x + (guimp->final_image.pos.w * guimp->zoom),
			guimp->final_image.pos.y + (guimp->final_image.pos.h * guimp->zoom)),
		0xffffbe00);

	// Make dotted outline around the selected layer dimensions;
	if (guimp->selected_layer >= 0 && guimp->selected_layer < guimp->layer_amount)
	{
		// figure out layers position relative to image;
		t_vec2i	relative_layer_pos;

		relative_layer_pos.x = guimp->final_image.pos.x + (guimp->layers[guimp->selected_layer].pos.x * guimp->zoom);
		relative_layer_pos.y = guimp->final_image.pos.y + (guimp->layers[guimp->selected_layer].pos.y * guimp->zoom);
		ui_surface_rect_draw(guimp->hidden_surface,
			relative_layer_pos,
			vec2i(relative_layer_pos.x + (guimp->layers[guimp->selected_layer].pos.w * guimp->zoom),
				relative_layer_pos.y + (guimp->layers[guimp->selected_layer].pos.h * guimp->zoom)),
			0xffff00ff);
	}

	if (guimp->hidden_texture == NULL)
		guimp->hidden_texture = SDL_CreateTextureFromSurface(guimp->win_main->renderer, guimp->hidden_surface);
	else
		SDL_UpdateTexture(guimp->hidden_texture, NULL, guimp->hidden_surface->pixels, guimp->hidden_surface->pitch);
	if (guimp->final_image_texture == NULL)
		guimp->final_image_texture = SDL_CreateTextureFromSurface(guimp->win_main->renderer, guimp->final_image.surface);
	else
		SDL_UpdateTexture(guimp->final_image_texture, NULL, guimp->final_image.surface->pixels, guimp->final_image.surface->pitch);
	// final image
	SDL_SetRenderTarget(guimp->win_main->renderer, guimp->win_main->texture);
	SDL_RenderCopy(guimp->win_main->renderer, guimp->final_image_texture, NULL, &(SDL_Rect){guimp->final_image.pos.x, guimp->final_image.pos.y, guimp->final_image.pos.w * guimp->zoom, guimp->final_image.pos.h * guimp->zoom});
	SDL_SetRenderTarget(guimp->win_main->renderer, NULL);

	SDL_SetRenderTarget(guimp->win_main->renderer, guimp->final_image_texture);
	SDL_RenderClear(guimp->win_main->renderer);

	// Hidden surface
	SDL_SetRenderTarget(guimp->win_main->renderer, guimp->win_main->texture);
	SDL_RenderCopy(guimp->win_main->renderer, guimp->hidden_texture, NULL, NULL);

	SDL_FillRect(guimp->hidden_surface, NULL, 0x00000000);

	// Clear Render Target
	SDL_SetRenderTarget(guimp->win_main->renderer, NULL);
}

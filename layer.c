#include "guimp.h"

/*
 * Not to confuse with the elements,
 * these are the actual drawing layers.
*/
void	new_layer(t_layer *layer, char *name, t_vec4i pos)
{
	layer->name = ft_strdup(name);
	layer->pos = pos;
	layer->surface = ui_surface_new(pos.w, pos.h);
	SDL_FillRect(layer->surface, NULL, 0x00000000);
}

/*
 * Selecting the layer and stuff.
*/
void	layer_event(t_guimp *guimp, SDL_Event e)
{
	// Element event
	int				ii;
	t_ui_element	*button;

	ii = -1;
	while (++ii < guimp->layer_amount)
	{
		button = ui_list_get_element_by_id(((t_ui_menu *)guimp->layer_elems[ii]->element)->children, "layer_select_button");
		if (ui_button(button))
		{
			guimp->selected_layer = ii;
			button->is_click = 1;
			button->state = UI_STATE_CLICK;
		}
	}
	// Other layer events
	if (SDL_GetModState() & KMOD_LCTRL && guimp->win_main->scroll)
		guimp->zoom += (float)guimp->win_main->scroll / 10;
}

/*
 * NOTE:
 * When drawing you need to take into consideration the position of the image, the layer, and the zoom...
 * ...and maybe other things in the future.A
*/
void	layer_draw(t_guimp *guimp)
{
	t_vec2i	relative_mouse_pos;

	if (!guimp->win_main->mouse_down)
		return ;
	relative_mouse_pos.x = guimp->win_main->mouse_pos.x - guimp->final_image.pos.x;
	relative_mouse_pos.y = guimp->win_main->mouse_pos.y - guimp->final_image.pos.y;
	if (guimp->win_main->mouse_down == 1) // basically draw button
	{
		if (guimp->selected_layer < 0 || guimp->selected_layer >= guimp->layer_amount)
			return ;
		t_layer	*active_layer = &guimp->layers[guimp->selected_layer];
		float aspect_x = ((float)active_layer->pos.w / ((float)active_layer->pos.w * guimp->zoom));
		float aspect_y = ((float)active_layer->pos.h / ((float)active_layer->pos.h * guimp->zoom));
		ui_surface_pixel_set(guimp->layers[guimp->selected_layer].surface,
			(relative_mouse_pos.x + guimp->layers[guimp->selected_layer].pos.x) * aspect_x,
			(relative_mouse_pos.y + guimp->layers[guimp->selected_layer].pos.y) * aspect_y,
			guimp->combined_color);
	}
	else if (guimp->win_main->mouse_down == 2) // move whole image by holding middle mouse and mouse motioning
	{
		t_vec2i	mouse_pos;

		mouse_pos.x = guimp->win_main->mouse_pos.x - guimp->win_main->mouse_pos_prev.x;
		mouse_pos.y = guimp->win_main->mouse_pos.y - guimp->win_main->mouse_pos_prev.y;
		guimp->final_image.pos = vec4i(mouse_pos.x + guimp->final_image.pos.x, mouse_pos.y + guimp->final_image.pos.y, guimp->final_image.pos.w, guimp->final_image.pos.h);
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

	ii = -1;
	while (++ii < guimp->layer_amount)
	{
		SDL_BlitSurface(guimp->layers[ii].surface, NULL, guimp->final_image.surface,
			&(SDL_Rect){guimp->layers[ii].pos.x, guimp->layers[ii].pos.y,
				guimp->layers[ii].pos.w, guimp->layers[ii].pos.h});
	}
	if (guimp->final_image_texture == NULL)
		guimp->final_image_texture = SDL_CreateTextureFromSurface(guimp->win_main->renderer, guimp->final_image.surface);
	else
		SDL_UpdateTexture(guimp->final_image_texture, NULL, guimp->final_image.surface->pixels, guimp->final_image.surface->pitch); // kuulemma slow function
	SDL_SetRenderTarget(guimp->win_main->renderer, guimp->win_main->texture);
	SDL_RenderCopy(guimp->win_main->renderer, guimp->final_image_texture, NULL, &(SDL_Rect){guimp->final_image.pos.x, guimp->final_image.pos.y, guimp->final_image.pos.w * guimp->zoom, guimp->final_image.pos.h * guimp->zoom});
	SDL_SetRenderTarget(guimp->win_main->renderer, NULL);
}

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
void	layer_event(t_guimp *guimp)
{
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
}

/*
 * NOTE:
 * When drawing you need to take into consideration the position of the image, the layer, and the zoom...
 * ...and maybe other things in the future.A
*/
void	layer_draw(t_guimp *guimp)
{
	t_vec2i	pos; // this takes all the stuff into consideration (look comment above func);

	pos.x = guimp->win_main->mouse_pos.x - guimp->final_image.pos.x;
	pos.y = guimp->win_main->mouse_pos.y - guimp->final_image.pos.y;
	if (guimp->win_main->mouse_down == 1)
	{
		if (guimp->selected_layer < 0 || guimp->selected_layer >= guimp->layer_amount)
			return ;
		ui_surface_pixel_set(guimp->layers[guimp->selected_layer].surface, pos.x, pos.y, guimp->combined_color);
	}
	else if (guimp->win_main->mouse_down == 2)
	{
		ft_printf("mouse_pos : %d %d\n", guimp->win_main->mouse_pos.x, guimp->win_main->mouse_pos.y);
		ft_printf("mouse_pos_prev : %d %d\n", guimp->mouse_pos_prev.x, guimp->mouse_pos_prev.y);
		pos.x = guimp->win_main->mouse_pos.x - guimp->mouse_pos_prev.x;
		pos.y = guimp->win_main->mouse_pos.y - guimp->mouse_pos_prev.y;
		guimp->final_image.pos = vec4i(pos.x + guimp->final_image.pos.x, pos.y + guimp->final_image.pos.y, guimp->final_image.pos.w, guimp->final_image.pos.h);
	}
	else if (SDL_GetModState() & KMOD_LCTRL && guimp->win_main->scroll)
	{
		ft_printf("ctrl is donw\n");
//		guimp->zoom += guimp->win_main->scroll / 10;
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
	SDL_RenderCopy(guimp->win_main->renderer, guimp->final_image_texture, NULL, &(SDL_Rect){guimp->final_image.pos.x, guimp->final_image.pos.y, guimp->final_image.pos.w * guimp->zoom, guimp->final_image.pos.h * guimp->zoom}); // here you should apply the zoom and stuff
	SDL_SetRenderTarget(guimp->win_main->renderer, NULL);
}

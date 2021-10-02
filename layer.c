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
	t_layer	*active_layer;
	t_vec2i	real_image_pos;
	t_vec2i	actual_pos;
	t_vec2i	hidden_pos;
	float	aspect_x;
	float	aspect_y;

	if (guimp->selected_layer < 0 || guimp->selected_layer >= guimp->layer_amount)
		return ;
	active_layer = &guimp->layers[guimp->selected_layer];
	aspect_x = ((float)active_layer->pos.w / ((float)active_layer->pos.w * guimp->zoom));
	aspect_y = ((float)active_layer->pos.h / ((float)active_layer->pos.h * guimp->zoom));

	hidden_pos = vec2i(guimp->win_main->mouse_pos.x * aspect_x, guimp->win_main->mouse_pos.y * aspect_y);

	real_image_pos.x = guimp->win_main->mouse_pos.x - guimp->final_image.pos.x;
	real_image_pos.y = guimp->win_main->mouse_pos.y - guimp->final_image.pos.y;
	actual_pos.x = (real_image_pos.x - active_layer->pos.x) * aspect_x;
	actual_pos.y = (real_image_pos.y - active_layer->pos.y) * aspect_y;

	// Draw Final Image Outline
	t_vec2i	final_real_pos;
	final_real_pos.x = guimp->final_image.pos.x * aspect_x;
	final_real_pos.y = guimp->final_image.pos.y * aspect_y;
	ui_surface_rect_draw(guimp->hidden_surface,
		vec2i(final_real_pos.x, final_real_pos.y),
		vec2i(final_real_pos.x + guimp->final_image.pos.w,
			final_real_pos.y + guimp->final_image.pos.h),
		0xffffbe00);

	// Draw Active Layer Outline
	t_vec2i	layer_rel_pos;
	layer_rel_pos.x = final_real_pos.x + active_layer->pos.x;
	layer_rel_pos.y = final_real_pos.y + active_layer->pos.y;
	ui_surface_rect_draw(guimp->hidden_surface, layer_rel_pos,
		vec2i(layer_rel_pos.x + active_layer->pos.w,
			layer_rel_pos.y + active_layer->pos.h),
		0xffff00ff);

	if (guimp->draw_button->state == UI_STATE_CLICK)
		draw_brush(guimp, active_layer, actual_pos, hidden_pos);
	else if (guimp->text_button->state == UI_STATE_CLICK)
		text_brush(guimp, active_layer, actual_pos, hidden_pos);
	else if (guimp->erase_button->state == UI_STATE_CLICK)
		erase_brush(guimp, active_layer, actual_pos, hidden_pos);
	else if (guimp->flood_button->state == UI_STATE_CLICK)
		flood_brush(guimp, active_layer, actual_pos);
	else if (guimp->sticker_button->state == UI_STATE_CLICK)
		sticker_brush(guimp, active_layer, actual_pos, hidden_pos);
	else if (guimp->move_button->state == UI_STATE_CLICK)
		move_brush(guimp, active_layer, actual_pos);
	else if (guimp->shape_button->state == UI_STATE_CLICK)
		shape_brush(guimp, active_layer, actual_pos, hidden_pos);
	else if (guimp->pipette_button->state == UI_STATE_CLICK)
		pipette_brush(guimp, vec2i(real_image_pos.x * aspect_x, real_image_pos.y * aspect_y));
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
	/*
	ui_surface_rect_draw(guimp->hidden_surface,
		vec2i(guimp->final_image.pos.x, guimp->final_image.pos.y),
		vec2i(guimp->final_image.pos.x + (guimp->final_image.pos.w * guimp->zoom),
			guimp->final_image.pos.y + (guimp->final_image.pos.h * guimp->zoom)),
		0xffffbe00);
		*/

	/*
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
	*/

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
	/*
	int	new_w = guimp->hidden_surface->w * guimp->zoom;
	int	how_much_moved_x = (guimp->hidden_surface->w - new_w) / 2;
	int	new_h = guimp->hidden_surface->h * guimp->zoom;
	int	how_much_moved_y = (guimp->hidden_surface->h - new_h) / 2;
	SDL_SetRenderTarget(guimp->win_main->renderer, guimp->win_main->texture);
	SDL_RenderCopy(guimp->win_main->renderer, guimp->hidden_texture, NULL, &(SDL_Rect){how_much_moved_x, how_much_moved_y, new_w, new_h});
	*/
	SDL_SetRenderTarget(guimp->win_main->renderer, guimp->win_main->texture);
	//SDL_RenderCopy(guimp->win_main->renderer, guimp->hidden_texture, NULL, NULL); 
	SDL_RenderCopy(guimp->win_main->renderer, guimp->hidden_texture, NULL, &(SDL_Rect){0, 0, guimp->hidden_surface->w * guimp->zoom, guimp->hidden_surface->h * guimp->zoom}); 
	SDL_FillRect(guimp->hidden_surface, NULL, 0x00000000);

	// Clear Render Target
	SDL_SetRenderTarget(guimp->win_main->renderer, NULL);
}

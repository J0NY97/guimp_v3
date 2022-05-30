/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   layer1.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nneronin <nneronin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 13:38:44 by nneronin          #+#    #+#             */
/*   Updated: 2022/05/30 13:39:24 by nneronin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "guimp.h"

/*
 * Draw both image outline and selected layer outline;
*/
void	draw_outlines(t_guimp *guimp, t_vec2 aspect)
{
	t_vec2i	final_real_pos;
	t_vec2i	layer_rel_pos;

	final_real_pos.x = guimp->final_image.pos.x * aspect.x;
	final_real_pos.y = guimp->final_image.pos.y * aspect.y;
	ui_surface_rect_draw(guimp->hidden_surface,
		vec2i(final_real_pos.x, final_real_pos.y),
		vec2i(final_real_pos.x + guimp->final_image.pos.w,
			final_real_pos.y + guimp->final_image.pos.h),
		0xffffbe00);
	layer_rel_pos.x = final_real_pos.x + guimp->active_layer->pos.x;
	layer_rel_pos.y = final_real_pos.y + guimp->active_layer->pos.y;
	ui_surface_rect_draw(guimp->hidden_surface, layer_rel_pos,
		vec2i(layer_rel_pos.x + guimp->active_layer->pos.w,
			layer_rel_pos.y + guimp->active_layer->pos.h),
		0xffff00ff);
}

void	draw_with_selected_brush(t_guimp *guimp, t_vec2 aspect)
{
	if (guimp->draw_button->state == UI_STATE_CLICK)
		draw_brush(guimp, guimp->active_layer,
			guimp->actual_pos, guimp->hidden_pos);
	else if (guimp->text_button->state == UI_STATE_CLICK)
		text_brush(guimp, guimp->active_layer,
			guimp->actual_pos, guimp->hidden_pos);
	else if (guimp->erase_button->state == UI_STATE_CLICK)
		erase_brush(guimp, guimp->active_layer,
			guimp->actual_pos, guimp->hidden_pos);
	else if (guimp->flood_button->state == UI_STATE_CLICK)
		flood_brush(guimp, guimp->active_layer, guimp->actual_pos);
	else if (guimp->sticker_button->state == UI_STATE_CLICK)
		sticker_brush(guimp, guimp->active_layer,
			guimp->actual_pos, guimp->hidden_pos);
	else if (guimp->move_button->state == UI_STATE_CLICK)
		move_brush(guimp, guimp->active_layer);
	else if (guimp->shape_button->state == UI_STATE_CLICK)
		shape_brush(guimp, guimp->active_layer,
			guimp->actual_pos, guimp->hidden_pos);
	else if (guimp->pipette_button->state == UI_STATE_CLICK)
		pipette_brush(guimp, vec2i(guimp->real_image_pos.x * aspect.x,
				guimp->real_image_pos.y * aspect.y));
}

/*
 * NOTE:
 * When drawing you need to take into consideration the
 * position of the image, the layer, and the zoom...
 * ...and maybe other things in the future.
*/
void	layer_draw(t_guimp *guimp)
{
	t_vec2	aspect;

	if (guimp->selected_layer < 0
		|| guimp->selected_layer >= guimp->layer_amount)
		return ;
	guimp->active_layer = &guimp->layers[guimp->selected_layer];
	aspect.x = ((float)guimp->active_layer->pos.w
			/ ((float)guimp->active_layer->pos.w * guimp->zoom));
	aspect.y = ((float)guimp->active_layer->pos.h
			/ ((float)guimp->active_layer->pos.h * guimp->zoom));
	guimp->hidden_pos = vec2i(guimp->win_main->mouse_pos.x * aspect.x,
			guimp->win_main->mouse_pos.y * aspect.y);
	guimp->real_image_pos.x = (guimp->win_main->mouse_pos.x
			- guimp->final_image.pos.x) * aspect.x;
	guimp->real_image_pos.y = (guimp->win_main->mouse_pos.y
			- guimp->final_image.pos.y) * aspect.y;
	guimp->actual_pos.x
		= (guimp->real_image_pos.x - guimp->active_layer->pos.x);
	guimp->actual_pos.y
		= (guimp->real_image_pos.y - guimp->active_layer->pos.y);
	draw_outlines(guimp, aspect);
	draw_with_selected_brush(guimp, aspect);
}

void	render_all_layers(t_guimp *guimp)
{
	int	ii;

	ii = -1;
	while (++ii < guimp->layer_amount)
	{
		if (!*guimp->layers[ii].show)
			continue ;
		SDL_BlitSurface(guimp->layers[ii].surface, NULL,
			guimp->final_image.surface, &(SDL_Rect){guimp->layers[ii].pos.x,
			guimp->layers[ii].pos.y, guimp->layers[ii].pos.w,
			guimp->layers[ii].pos.h});
	}
}

/*
 * This renders all the layers on the image layer and
 * 	then renders the image layer on the main window.
 * All the layers will be rendered on the image no matter what,
 * 	in the order ordererd by user,
 * then when we have image layer done, make texture out of it,
 * 	then renders it on the window at the correct zoom lvl and correct position.
*/
void	layer_render(t_guimp *guimp)
{
	SDL_FillRect(guimp->final_image.surface, NULL, 0xff000000);
	render_all_layers(guimp);
	SDL_UpdateTexture(guimp->hidden_texture, NULL,
		guimp->hidden_surface->pixels, guimp->hidden_surface->pitch);
	SDL_UpdateTexture(guimp->final_image_texture, NULL,
		guimp->final_image.surface->pixels, guimp->final_image.surface->pitch);
	SDL_SetRenderTarget(guimp->win_main->renderer, guimp->win_main->texture);
	SDL_RenderCopy(guimp->win_main->renderer, guimp->final_image_texture, NULL,
		&(SDL_Rect){guimp->final_image.pos.x, guimp->final_image.pos.y,
		guimp->final_image.pos.w * guimp->zoom,
		guimp->final_image.pos.h * guimp->zoom});
	SDL_SetRenderTarget(guimp->win_main->renderer, guimp->final_image_texture);
	SDL_RenderClear(guimp->win_main->renderer);
	SDL_SetRenderTarget(guimp->win_main->renderer, guimp->win_main->texture);
	SDL_RenderCopy(guimp->win_main->renderer, guimp->hidden_texture, NULL,
		&(SDL_Rect){0, 0, guimp->hidden_surface->w * guimp->zoom,
		guimp->hidden_surface->h * guimp->zoom});
	SDL_FillRect(guimp->hidden_surface, NULL, 0x00000000);
	SDL_SetRenderTarget(guimp->win_main->renderer, NULL);
}

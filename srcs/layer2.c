/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   layer2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nneronin <nneronin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 13:38:59 by nneronin          #+#    #+#             */
/*   Updated: 2022/05/30 13:40:00 by nneronin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "guimp.h"

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

void	move_image(t_guimp *guimp)
{
	guimp->final_image.pos.x
		+= guimp->win_main->mouse_pos.x - guimp->win_main->mouse_pos_prev.x;
	guimp->final_image.pos.y
		+= guimp->win_main->mouse_pos.y - guimp->win_main->mouse_pos_prev.y;
}

void	zoom_image(t_guimp *guimp)
{
	t_vec2i	prev;
	t_vec2i	next;

	prev.x = guimp->final_image.pos.w * guimp->zoom;
	prev.y = guimp->final_image.pos.h * guimp->zoom;
	guimp->zoom = ft_fclamp(guimp->zoom
			+ (float)guimp->win_main->scroll / 10.0f, 0.1f, 100.0f);
	next.x = guimp->final_image.pos.w * guimp->zoom;
	next.y = guimp->final_image.pos.h * guimp->zoom;
	guimp->final_image.pos.x += (prev.x - next.x) / 2;
	guimp->final_image.pos.y += (prev.y - next.y) / 2;
}

/*
 * Check which layer was selected.
 * Check zoom events.
 * Check move events.
*/
void	layer_event(t_guimp *guimp)
{
	int				ii;
	t_ui_element	*button;

	ii = -1;
	guimp->new_layer_selected = 0;
	while (++ii < guimp->layer_amount)
	{
		button = ui_list_get_element_by_id(guimp->layer_elems[ii]->children,
				"layer_select_button");
		if (ui_button(button))
		{
			guimp->selected_layer = ii;
			guimp->new_layer_selected = 1;
			button->is_click = 1;
			button->state = UI_STATE_CLICK;
		}
	}
	if (SDL_GetModState() & KMOD_LCTRL && guimp->win_main->scroll)
		zoom_image(guimp);
	else if (guimp->win_main->mouse_down == SDL_BUTTON_MIDDLE)
		move_image(guimp);
}

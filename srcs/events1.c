/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   events1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nneronin <nneronin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 13:35:16 by nneronin          #+#    #+#             */
/*   Updated: 2022/05/30 13:37:37 by nneronin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "guimp.h"

/*
 * Not moving the layer on the main_window,
 * 	but moving the layer elements, on the win_toolbox, up and down;
 * 
 * 'dir' : either +1 or -1; (up / down);
*/
void	move_layer_in_dir(t_guimp *guimp, int dir)
{
	t_ui_element	*temp;
	t_layer			temporar;

	if (dir == 1 && guimp->selected_layer >= guimp->layer_amount - 1)
		return ;
	else if (dir == -1 && guimp->selected_layer <= 0)
		return ;
	temporar = guimp->layers[guimp->selected_layer];
	guimp->layers[guimp->selected_layer]
		= guimp->layers[guimp->selected_layer + dir];
	guimp->layers[guimp->selected_layer + dir] = temporar;
	vec4_swap(&guimp->layer_elems[guimp->selected_layer + dir]->pos,
		&guimp->layer_elems[guimp->selected_layer]->pos);
	temp = guimp->layer_elems[guimp->selected_layer];
	guimp->layer_elems[guimp->selected_layer]
		= guimp->layer_elems[guimp->selected_layer + dir];
	guimp->layer_elems[guimp->selected_layer + dir] = temp;
	guimp->selected_layer += dir;
}

/*
 * The buttons that move the layers up and down.
*/
void	button_move_layer_event(t_guimp *guimp)
{
	if (ui_button(guimp->button_move_layer_up))
		move_layer_in_dir(guimp, -1);
	else if (ui_button(guimp->button_move_layer_down))
		move_layer_in_dir(guimp, 1);
}

void	save_button_event(t_guimp *guimp)
{
	if (ui_button(guimp->save_button))
	{
		ui_window_flag_set(guimp->win_save_image, UI_WINDOW_SHOW);
		SDL_RaiseWindow(guimp->win_save_image->win);
	}
	if (ui_button(guimp->button_save_image_ok))
	{
		save_surface(guimp->final_image.surface,
			ui_input_get_text(guimp->input_save_image_name));
		ui_window_flag_set(guimp->win_save_image, UI_WINDOW_HIDE);
	}
}

/*
 * NOTE:
 * 	after resizing the image, we have to put the last_state of the layer_elems
 * 	to something else so that the texture of them will be reset;
*/
void	edit_button_event(t_guimp *guimp)
{
	int	w;
	int	h;
	int	iii;

	if (ui_button(guimp->edit_button))
	{
		ui_window_flag_set(guimp->win_image_edit, UI_WINDOW_SHOW);
		SDL_RaiseWindow(guimp->win_image_edit->win);
	}
	if (ui_button(guimp->new_image_ok_button))
	{
		w = atoi(guimp->new_image_width_input_label->text);
		h = atoi(guimp->new_image_height_input_label->text);
		resize_layer(&guimp->final_image, vec2i(w, h));
		SDL_DestroyTexture(guimp->final_image_texture);
		guimp->final_image_texture
			= ui_create_texture(guimp->win_main->renderer, vec2i(w, h));
		SDL_UpdateTexture(guimp->final_image_texture, NULL,
			guimp->final_image.surface->pixels,
			guimp->final_image.surface->pitch);
		ui_window_flag_set(guimp->win_image_edit, UI_WINDOW_HIDE);
		iii = -1;
		while (++iii < guimp->layer_amount)
			guimp->layer_elems[iii]->last_state = -13;
	}
}

/*
 * Removing all the layers;
*/
void	clear_button_event(t_guimp *guimp)
{
	int	i;
	int	amount;

	if (ui_button(guimp->clear_button))
	{
		i = -1;
		amount = guimp->layer_amount;
		ft_printf("[%s] Starting to remove %d layers.\n", __FUNCTION__, amount);
		while (++i < guimp->layer_amount)
			remove_nth_layer(guimp, 0);
		ft_printf("[%s] %d layers removed.\n", __FUNCTION__, i);
	}
}

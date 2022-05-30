/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   events2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nneronin <nneronin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 13:35:47 by nneronin          #+#    #+#             */
/*   Updated: 2022/05/30 13:37:48 by nneronin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "guimp.h"

void	button_add_layer_event(t_guimp *guimp)
{
	if (ui_button(guimp->button_add_layer))
	{
		ui_window_flag_set(guimp->win_layer_new, UI_WINDOW_SHOW);
		SDL_RaiseWindow(guimp->win_layer_new->win);
	}
	if (ui_button(guimp->new_layer_ok_button))
	{
		new_layer_combination(guimp);
		ui_window_flag_set(guimp->win_layer_new, UI_WINDOW_HIDE);
	}
}

void	reorder_layers(t_guimp *guimp)
{
	int	i;

	i = -1;
	while (++i < guimp->layer_amount)
	{
		if (guimp->layer_elems[i] == NULL)
		{
			guimp->layers[i] = guimp->layers[i + 1];
			guimp->layer_elems[i] = guimp->layer_elems[i + 1];
			guimp->layer_elems[i + 1] = NULL;
			ui_element_pos_set2(guimp->layer_elems[i],
				vec2(guimp->layer_elems[i]->pos.x,
					(guimp->layer_elems[i]->pos.h * i) + (10 * i) + 40));
		}
	}
}

/*
 * removes and reorders layer_elems;
 *
 * NOTE: the '40', in ...pos_set2(), comes from the offset on the menu;
*/
void	remove_nth_layer(t_guimp *guimp, int nth)
{
	t_ui_element	*button;

	if (nth < 0 || nth >= guimp->layer_amount)
		return ;
	layer_free(&guimp->layers[nth]);
	ui_element_free(guimp->layer_elems[guimp->selected_layer],
		sizeof(t_ui_element));
	guimp->layer_elems[nth] = NULL;
	guimp->layer_amount--;
	reorder_layers(guimp);
	guimp->selected_layer
		= ft_clamp(guimp->selected_layer - 1, 0, guimp->layer_amount);
	if (guimp->layer_amount <= 0)
		return ;
	button = ui_list_get_element_by_id(
			guimp->layer_elems[guimp->selected_layer]->children,
			"layer_select_button");
	if (button)
		ui_radio_button_toggle_on(&guimp->radio_layer, button);
}

void	button_remove_layer_event(t_guimp *guimp)
{
	if (ui_button(guimp->button_remove_layer))
		remove_nth_layer(guimp, guimp->selected_layer);
}

void	edit_layer_event(t_guimp *guimp)
{
	char	temp[20];

	ui_window_flag_set(guimp->win_layer_edit, UI_WINDOW_SHOW);
	SDL_RaiseWindow(guimp->win_layer_edit->win);
	ui_label_set_text(ui_input_get_label_element(guimp->input_edit_layer_name),
		guimp->layers[guimp->selected_layer].name);
	ui_label_set_text(ui_input_get_label_element(guimp->input_edit_layer_width),
		ft_b_itoa(guimp->layers[guimp->selected_layer].pos.w, temp));
	ui_label_set_text(
		ui_input_get_label_element(guimp->input_edit_layer_height),
		ft_b_itoa(guimp->layers[guimp->selected_layer].pos.h, temp));
}

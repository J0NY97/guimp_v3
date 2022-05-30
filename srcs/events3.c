/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   events3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nneronin <nneronin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 13:37:12 by nneronin          #+#    #+#             */
/*   Updated: 2022/05/30 13:37:49 by nneronin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "guimp.h"

void	apply_layer_edit_event(t_guimp *guimp)
{
	resize_layer(&guimp->layers[guimp->selected_layer],
		vec2i(ft_atoi(ui_input_get_text(guimp->input_edit_layer_width)),
			ft_atoi(ui_input_get_text(guimp->input_edit_layer_height))));
	rename_layer(guimp, guimp->selected_layer,
		ui_input_get_text(guimp->input_edit_layer_name));
	ui_window_flag_set(guimp->win_layer_edit, UI_WINDOW_HIDE);
}

void	button_edit_layer_event(t_guimp *guimp)
{
	if (guimp->new_layer_selected && guimp->win_layer_edit->show)
		edit_layer_event(guimp);
	if (guimp->selected_layer < 0 || guimp->selected_layer >= MAX_LAYER_AMOUNT)
		return ;
	if (ui_button(guimp->button_edit_layer) && guimp->layer_amount > 0)
		edit_layer_event(guimp);
	if (ui_button(guimp->button_edit_layer_ok))
		apply_layer_edit_event(guimp);
}

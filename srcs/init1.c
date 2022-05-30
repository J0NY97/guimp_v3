/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init1.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nneronin <nneronin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 13:40:42 by nneronin          #+#    #+#             */
/*   Updated: 2022/05/30 13:41:57 by nneronin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "guimp.h"

void	new_layer_window_init(t_guimp *guimp)
{
	guimp->win_layer_new
		= ui_layout_get_window(&guimp->layout, "new_layer_window");
	guimp->new_layer_ok_button
		= ui_layout_get_element(&guimp->layout, "button_ok");
	guimp->new_layer_name_input_label = ui_input_get_label(
			ui_layout_get_element(&guimp->layout, "input_name"));
	guimp->new_layer_width_input_label = ui_input_get_label(
			ui_layout_get_element(&guimp->layout, "input_width"));
	guimp->new_layer_height_input_label = ui_input_get_label(
			ui_layout_get_element(&guimp->layout, "input_height"));
	guimp->win_image_edit
		= ui_layout_get_window(&guimp->layout, "image_edit_window");
	guimp->new_image_ok_button
		= ui_layout_get_element(&guimp->layout, "button_ok_image");
	guimp->new_image_width_input_label = ui_input_get_label(
			ui_layout_get_element(&guimp->layout, "input_width_image"));
	guimp->new_image_height_input_label = ui_input_get_label(
			ui_layout_get_element(&guimp->layout, "input_height_image"));
}

void	edit_layer_window_init(t_guimp *guimp)
{
	guimp->win_layer_edit
		= ui_layout_get_window(&guimp->layout, "window_edit_layer");
	guimp->button_edit_layer_ok
		= ui_layout_get_element(&guimp->layout, "button_edit_layer_ok");
	guimp->input_edit_layer_name
		= ui_layout_get_element(&guimp->layout, "input_edit_layer_name");
	guimp->input_edit_layer_width
		= ui_layout_get_element(&guimp->layout, "input_edit_layer_width");
	guimp->input_edit_layer_height
		= ui_layout_get_element(&guimp->layout, "input_edit_layer_height");
}

void	save_image_window_init(t_guimp *guimp)
{
	guimp->win_save_image
		= ui_layout_get_window(&guimp->layout, "window_save_image");
	guimp->input_save_image_name
		= ui_layout_get_element(&guimp->layout, "input_save_image_name");
	guimp->button_save_image_ok
		= ui_layout_get_element(&guimp->layout, "button_save_image_ok");
}

void	brush_init(t_guimp *guimp)
{
	guimp->draw_button = ui_layout_get_element(&guimp->layout, "draw_button");
	guimp->text_button = ui_layout_get_element(&guimp->layout, "text_button");
	guimp->erase_button = ui_layout_get_element(&guimp->layout, "erase_button");
	guimp->flood_button = ui_layout_get_element(&guimp->layout, "flood_button");
	guimp->sticker_button
		= ui_layout_get_element(&guimp->layout, "sticker_button");
	guimp->move_button = ui_layout_get_element(&guimp->layout, "move_button");
	guimp->shape_button = ui_layout_get_element(&guimp->layout, "shape_button");
	guimp->pipette_button
		= ui_layout_get_element(&guimp->layout, "pipette_button");
	guimp->circle_button
		= ui_layout_get_element(&guimp->layout, "circle_button");
	guimp->square_button
		= ui_layout_get_element(&guimp->layout, "square_button");
	guimp->line_button = ui_layout_get_element(&guimp->layout, "line_button");
}

void	initteroni(t_guimp *guimp)
{
	guimp_init(guimp);
	toolbox_window_init(guimp);
	new_layer_window_init(guimp);
	edit_layer_window_init(guimp);
	save_image_window_init(guimp);
	ui_radio_new(guimp->win_toolbox, &guimp->radio_layer);
	guimp->radio_layer.free_me = 0;
	guimp->radio_buttons = guimp->radio_layer.children;
	brush_init(guimp);
	new_layer_combination(guimp);
}

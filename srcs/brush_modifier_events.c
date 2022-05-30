/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   brush_modifier_events.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nneronin <nneronin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 13:35:01 by nneronin          #+#    #+#             */
/*   Updated: 2022/05/30 13:35:02 by nneronin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "guimp.h"

void	color_slider_events(t_guimp *guimp)
{
	char	*temp;

	guimp->combined_color = rgba_to_hex(rgba(
				ui_slider_value_get(guimp->red_slider),
				ui_slider_value_get(guimp->green_slider),
				ui_slider_value_get(guimp->blue_slider),
				ui_slider_value_get(guimp->alpha_slider)));
	ui_element_color_set(guimp->color_swatch,
		UI_STATE_DEFAULT, guimp->combined_color);
	if (!guimp->color_swatch->is_click)
	{
		temp = ft_itoa_base(guimp->combined_color, 16);
		ui_input_set_text(guimp->color_swatch, temp);
		ft_strdel(&temp);
	}
}

void	color_swatch_event(t_guimp *guimp)
{
	t_ui_label		*label;
	t_rgba			input_rgba;

	if (ui_slider_updated(guimp->red_slider)
		|| ui_slider_updated(guimp->green_slider)
		|| ui_slider_updated(guimp->blue_slider)
		|| ui_slider_updated(guimp->alpha_slider))
		color_slider_events(guimp);
	else if (guimp->color_swatch->is_click)
	{
		label = ui_input_get_label(guimp->color_swatch);
		guimp->combined_color = (unsigned int)strtoul(label->text, NULL, 16);
		input_rgba = hex_to_rgba(guimp->combined_color);
		ui_slider_value_set(guimp->red_slider, input_rgba.r);
		ui_slider_value_set(guimp->green_slider, input_rgba.g);
		ui_slider_value_set(guimp->blue_slider, input_rgba.b);
		ui_slider_value_set(guimp->alpha_slider, input_rgba.a);
		ui_element_color_set(guimp->color_swatch,
			UI_STATE_DEFAULT, guimp->combined_color);
	}
	if (ui_slider_updated(guimp->size_slider))
		guimp->size = ui_slider_value_get(guimp->size_slider);
}

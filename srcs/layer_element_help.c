/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   element_help.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/29 13:55:10 by jsalmi            #+#    #+#             */
/*   Updated: 2022/05/29 13:56:26 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "guimp.h"

void	new_element_from_recipe_with_parent(
	t_ui_element *elem, int elem_type, char *recipe_id, t_ui_element *parent)
{
	t_ui_recipe		*recipe;

	recipe = ui_layout_get_recipe(parent->win->layout, recipe_id);
	g_acceptable[elem_type].maker(parent->win, elem);
	ui_element_set_parent(elem, parent, UI_TYPE_ELEMENT);
	ui_element_edit(elem, recipe);
	ui_element_set_id(elem, recipe_id);
}

float	get_ratio(t_vec2i orig_wh, t_vec2i new_wh)
{
	float	ratio_x;
	float	ratio_y;

	ratio_x = new_wh.x / (float)orig_wh.x;
	ratio_y = new_wh.y / (float)orig_wh.y;
	if (ratio_x < ratio_y)
		return (ratio_x);
	return (ratio_y);
}

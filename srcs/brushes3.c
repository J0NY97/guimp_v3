/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   brushes3.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/29 13:47:10 by jsalmi            #+#    #+#             */
/*   Updated: 2022/05/29 13:47:11 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "guimp.h"

void	line_tool_brush(t_guimp *guimp, t_layer *active_layer,
	t_vec2i actual_pos, t_vec2i hidden_pos)
{
	if (guimp->first_set)
		ui_surface_line_draw(guimp->hidden_surface, guimp->first_pos,
			hidden_pos, guimp->combined_color);
	if (guimp->win_main->mouse_down_last_frame != SDL_BUTTON_LEFT)
		return ;
	if (!guimp->first_set)
	{
		guimp->first_pos_converted = actual_pos;
		guimp->first_pos = hidden_pos;
		guimp->first_set = 1;
	}
	else
	{
		ui_surface_line_draw(active_layer->surface, guimp->first_pos_converted,
			actual_pos, guimp->combined_color);
		guimp->first_set = 0;
	}
}

void	square_tool_brush(t_guimp *guimp, t_layer *active_layer,
	t_vec2i actual_pos, t_vec2i hidden_pos)
{
	if (guimp->first_set)
		ui_surface_rect_draw(guimp->hidden_surface,
			guimp->first_pos, hidden_pos, guimp->combined_color);
	if (guimp->win_main->mouse_down_last_frame != SDL_BUTTON_LEFT)
		return ;
	if (!guimp->first_set)
	{
		guimp->first_pos_converted = actual_pos;
		guimp->first_pos = hidden_pos;
		guimp->first_set = 1;
	}
	else
	{
		ui_surface_rect_draw(active_layer->surface, guimp->first_pos_converted,
			actual_pos, guimp->combined_color);
		guimp->first_set = 0;
	}
}

void	circle_tool_brush(
	t_guimp *guimp, t_layer *active_layer,
	t_vec2i actual_pos, t_vec2i hidden_pos)
{
	if (guimp->first_set)
	{
		ui_surface_circle_draw(guimp->hidden_surface, guimp->first_pos,
			dist(guimp->first_pos, hidden_pos), guimp->combined_color);
	}
	if (guimp->win_main->mouse_down_last_frame != SDL_BUTTON_LEFT)
		return ;
	if (!guimp->first_set)
	{
		guimp->first_pos_converted = actual_pos;
		guimp->first_pos = hidden_pos;
		guimp->first_set = 1;
	}
	else
	{
		ui_surface_circle_draw(active_layer->surface,
			guimp->first_pos_converted,
			dist(guimp->first_pos_converted, actual_pos),
			guimp->combined_color);
		guimp->first_set = 0;
	}
}

/*
 * unselect on right click;
 *	done here so that it applies to all the shape button tools;
 */
void	shape_brush(
	t_guimp *guimp, t_layer *active_layer,
	t_vec2i actual_pos, t_vec2i hidden_pos)
{
	if (guimp->win_main->mouse_down_last_frame == SDL_BUTTON_RIGHT)
	{
		guimp->first_set = 0;
		return ;
	}
	if (guimp->line_button->state == UI_STATE_CLICK)
		line_tool_brush(guimp, active_layer, actual_pos, hidden_pos);
	else if (guimp->square_button->state == UI_STATE_CLICK)
		square_tool_brush(guimp, active_layer, actual_pos, hidden_pos);
	else if (guimp->circle_button->state == UI_STATE_CLICK)
		circle_tool_brush(guimp, active_layer, actual_pos, hidden_pos);
}

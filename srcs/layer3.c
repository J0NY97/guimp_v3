/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   layer3.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nneronin <nneronin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 13:39:43 by nneronin          #+#    #+#             */
/*   Updated: 2022/05/30 13:40:15 by nneronin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

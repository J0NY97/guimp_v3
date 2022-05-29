/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   brushes2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/29 13:46:20 by jsalmi            #+#    #+#             */
/*   Updated: 2022/05/29 13:47:03 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "guimp.h"

/*
 * Checking for sticker_name.png/.bmp/.jpg
 *	from directory 'stickers/';
 *	(All the supported file formats)
 *	Returns the first found;
*/
char	*get_sticker_path(char *sticker_name)
{
	char	*final;	

	if (!sticker_name)
		return (NULL);
	final = ft_strjoiner("stickers/", sticker_name, ".png", NULL);
	if (!access(final, F_OK))
		return (final);
	ft_strdel(&final);
	final = ft_strjoiner("stickers/", sticker_name, ".jpg", NULL);
	if (!access(final, F_OK))
		return (final);
	ft_strdel(&final);
	final = ft_strjoiner("stickers/", sticker_name, ".bmp", NULL);
	if (!access(final, F_OK))
		return (final);
	ft_strdel(&final);
	return (NULL);
}

/*
 * 'adjusted_size[2]' : w/h for the sticker image taking the size slider into
 *		consideration, the size_slider value is interpreted as % of the size
 *		of the original image (5 on slider == 5% of original image w && h);
*/
void	sticker_brush(t_guimp *guimp, t_layer *active_layer,
	t_vec2i actual_pos, t_vec2i hidden_pos)
{
	SDL_Surface	*surface;
	char		*sticker_text;
	char		*full_path;
	int			adjusted_size[2];

	sticker_text = ui_dropdown_active_text(guimp->sticker_dropdown);
	full_path = get_sticker_path(sticker_text);
	if (!full_path)
		return ;
	surface = ui_surface_image_new(full_path);
	ft_strdel(&full_path);
	if (!surface)
	{
		LG_WARN("Sticker surface <%s> couldn\'t be created.", full_path);
		return ;
	}
	adjusted_size[0] = (guimp->size / 100.0f) * surface->w;
	adjusted_size[1] = (guimp->size / 100.0f) * surface->h;
	SDL_BlitScaled(surface, NULL, guimp->hidden_surface, &(SDL_Rect){
		hidden_pos.x, hidden_pos.y, adjusted_size[0], adjusted_size[1]});
	if (guimp->win_main->mouse_down == SDL_BUTTON_LEFT)
		SDL_BlitScaled(surface, NULL, active_layer->surface, &(SDL_Rect){
			actual_pos.x, actual_pos.y, adjusted_size[0], adjusted_size[1]});
	SDL_FreeSurface(surface);
}

/*
 * NOTE:
 *	actual_pos is the final_image position,
 *		instead of layer position like all the other brushes;
*/
void	pipette_brush(t_guimp *guimp, t_vec2i actual_pos)
{
	if (guimp->win_main->mouse_down_last_frame != SDL_BUTTON_LEFT)
		return ;
	set_sliders_to_color(guimp,
		ui_surface_pixel_get(guimp->final_image.surface,
			actual_pos.x, actual_pos.y));
}

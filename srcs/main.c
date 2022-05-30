/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nneronin <nneronin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 13:40:21 by nneronin          #+#    #+#             */
/*   Updated: 2022/05/30 13:43:18 by nneronin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "guimp.h"

/*
 * Reseting the final_image to the center of the
 * 	screen if it has disappeard somewhere.
*/
void	reset_image_events(t_guimp *guimp, SDL_Event e)
{
	if (e.key.keysym.scancode == SDL_SCANCODE_SPACE)
	{
		guimp->zoom = 1.0;
		guimp->final_image.pos.x = guimp->win_main->pos.w / 2
			- guimp->final_image.pos.w / 2;
		guimp->final_image.pos.y = guimp->win_main->pos.h / 2
			- guimp->final_image.pos.h / 2;
	}
}

void	drag_n_drop_events(t_guimp *guimp, SDL_Event e)
{
	SDL_Surface	*dropped_image;

	if (e.drop.type == SDL_DROPFILE
		&& e.drop.windowID == guimp->win_main->window_id)
	{
		ft_printf("%s dropped on window id %d\n", e.drop.file, e.drop.windowID);
		new_layer_combination(guimp);
		rename_layer(guimp, guimp->layer_amount - 1, e.drop.file);
		dropped_image = ui_surface_image_new(e.drop.file);
		resize_layer(&guimp->layers[guimp->layer_amount - 1],
			vec2i(dropped_image->w, dropped_image->h));
		SDL_BlitSurface(dropped_image, NULL,
			guimp->layers[guimp->layer_amount - 1].surface, NULL);
		SDL_FreeSurface(dropped_image);
	}
}

void	user_events(t_guimp *guimp, SDL_Event e)
{
	if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
		guimp->win_toolbox->wants_to_close = 1;
	reset_image_events(guimp, e);
	drag_n_drop_events(guimp, e);
	if (ui_dropdown_is_open(guimp->sticker_dropdown))
		guimp->font_dropdown->event = 0;
	else if (ui_dropdown_exit(guimp->sticker_dropdown))
		guimp->font_dropdown->event = 0;
	if (ui_dropdown_is_open(guimp->font_dropdown))
	{
		guimp->circle_button->event = 0;
		guimp->square_button->event = 0;
		guimp->line_button->event = 0;
	}
	button_add_layer_event(guimp);
	button_remove_layer_event(guimp);
	button_edit_layer_event(guimp);
	button_move_layer_event(guimp);
	color_swatch_event(guimp);
	save_button_event(guimp);
	edit_button_event(guimp);
	clear_button_event(guimp);
}

void	uninit(t_guimp *guimp)
{
	int	i;

	i = -1;
	ui_layout_free(&guimp->layout);
	SDL_DestroyTexture(guimp->final_image_texture);
	layer_free(&guimp->final_image);
	while (++i < MAX_LAYER_AMOUNT)
		layer_free(&guimp->layers[i]);
	SDL_FreeSurface(guimp->hidden_surface);
	SDL_DestroyTexture(guimp->hidden_texture);
}

int	main(void)
{
	t_guimp		guimp;
	SDL_Event	e;

	ui_sdl_init();
	initteroni(&guimp);
	while (!guimp.win_toolbox->wants_to_close)
	{
		while (SDL_PollEvent(&e))
		{
			ui_layout_event(&guimp.layout, e);
			user_events(&guimp, e);
			guimp.circle_button->event = 1;
			guimp.square_button->event = 1;
			guimp.line_button->event = 1;
			guimp.font_dropdown->event = 1;
			layer_event(&guimp);
			ui_radio_event(&guimp.radio_layer, e);
		}
		layer_elements_render(&guimp);
		layer_draw(&guimp);
		layer_render(&guimp);
		ui_layout_render(&guimp.layout);
	}
	uninit(&guimp);
	return (0);
}

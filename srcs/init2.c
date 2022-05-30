/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nneronin <nneronin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 13:41:38 by nneronin          #+#    #+#             */
/*   Updated: 2022/05/30 13:42:01 by nneronin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "guimp.h"

void	guimp_init(t_guimp *guimp)
{
	memset(guimp, 0, sizeof(t_guimp));
	SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
	ui_layout_load(&guimp->layout, NULL, "layout.ui");
	if (!guimp->layout.layout_file_content
		|| !guimp->layout.style_file_content)
		exit(0);
	guimp->win_main = ui_layout_get_window(&guimp->layout, "main_window");
	layer_new(&guimp->final_image, "Image",
		vec4i(guimp->win_main->pos.w / 2 - (1280 / 2),
			guimp->win_main->pos.h / 2 - (720 / 2), 1280, 720), NULL);
	SDL_FillRect(guimp->final_image.surface, NULL, 0xff000000);
	guimp->final_image_texture
		= ui_create_texture(guimp->win_main->renderer,
			vec2i(guimp->final_image.surface->w,
				guimp->final_image.surface->h));
	SDL_UpdateTexture(guimp->final_image_texture, NULL,
		guimp->final_image.surface->pixels, guimp->final_image.surface->pitch);
	guimp->selected_layer = -1;
	guimp->layer_amount = 0;
	guimp->combined_color = 0xffffffff;
	guimp->zoom = 1.0f;
	guimp->hidden_surface
		= ui_surface_new(guimp->win_main->pos.w, guimp->win_main->pos.h);
	guimp->hidden_texture = SDL_CreateTextureFromSurface(
			guimp->win_main->renderer, guimp->hidden_surface);
}

void	color_swatch_init(t_guimp *guimp)
{
	guimp->color_swatch = ui_layout_get_element(&guimp->layout, "color_swatch");
	guimp->red_slider = ui_layout_get_element(&guimp->layout, "r_slider");
	guimp->green_slider = ui_layout_get_element(&guimp->layout, "g_slider");
	guimp->blue_slider = ui_layout_get_element(&guimp->layout, "b_slider");
	guimp->alpha_slider = ui_layout_get_element(&guimp->layout, "a_slider");
	guimp->size_slider = ui_layout_get_element(&guimp->layout, "size_slider");
}

void	toolbox_window_init(t_guimp *guimp)
{
	guimp->win_toolbox = ui_layout_get_window(&guimp->layout, "toolbox_window");
	guimp->layer_recipe = ui_layout_get_recipe(&guimp->layout, "layer");
	guimp->layer_parent = ui_layout_get_element(&guimp->layout, "layer_menu");
	guimp->button_add_layer
		= ui_layout_get_element(&guimp->layout, "button_add_layer");
	guimp->button_remove_layer
		= ui_layout_get_element(&guimp->layout, "button_remove_layer");
	guimp->button_edit_layer
		= ui_layout_get_element(&guimp->layout, "button_edit_layer");
	guimp->button_move_layer_up
		= ui_layout_get_element(&guimp->layout, "button_move_layer_up");
	guimp->button_move_layer_down
		= ui_layout_get_element(&guimp->layout, "button_move_layer_down");
	guimp->text_input = ui_layout_get_element(&guimp->layout, "text_input");
	guimp->text_input_str = ui_input_get_text(guimp->text_input);
	guimp->font_dropdown = ui_layout_get_element(&guimp->layout, "font_drop");
	guimp->sticker_dropdown
		= ui_layout_get_element(&guimp->layout, "sticker_drop");
	guimp->save_button = ui_layout_get_element(&guimp->layout, "save_button");
	guimp->edit_button
		= ui_layout_get_element(&guimp->layout, "edit_image_button");
	guimp->clear_button = ui_layout_get_element(&guimp->layout, "clear_button");
	color_swatch_init(guimp);
}

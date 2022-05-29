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
	guimp->radio_buttons = guimp->radio_layer.children;
	brush_init(guimp);
	new_layer_combination(guimp);
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
	return (0);
}

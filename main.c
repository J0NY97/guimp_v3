#include "guimp.h"

void	user_events(t_guimp *guimp)
{
	// Layer button events
	button_add_layer_event(guimp);
	button_remove_layer_event(guimp);
	button_edit_layer_event(guimp);
	button_move_layer_event(guimp);
	// Other button events
	color_swatch_event(guimp);
	save_button_event(guimp);
	edit_button_event(guimp);
	clear_button_event(guimp);
}

void	user_render(t_guimp *guimp)
{
	(void)guimp;
}

void	user_code(t_guimp *guimp)
{
	(void)guimp;
}

void	guimp_init(t_guimp *guimp)
{
	memset(guimp, 0, sizeof(t_guimp));
	SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
	ui_layout_load_v2(&guimp->layout, "layout_v2.ui");
	// Main Win
	guimp->win_main = ui_list_get_window_by_id(guimp->layout.windows, "main_window");
	layer_new(&guimp->final_image, "Image", vec4i(10, 10, 1280, 720), NULL);
	SDL_FillRect(guimp->final_image.surface, NULL, 0xff000000); // fill image with black so the alpha:ed layers can show.
	guimp->final_image_texture = NULL;
	guimp->selected_layer = -1;
	guimp->layer_amount = 0;
	guimp->combined_color = 0xffffffff;
	guimp->zoom = 1.0f;
	guimp->hidden_surface = ui_surface_new(guimp->win_main->pos.w, guimp->win_main->pos.h);
}

void	toolbox_window_init(t_guimp *guimp)
{
	// Toolbox Win
	guimp->win_toolbox = ui_list_get_window_by_id(guimp->layout.windows, "toolbox_window");
	guimp->layer_recipe = ui_list_get_recipe_by_id_v2(guimp->layout.recipes, "layer");
	guimp->layer_parent = ui_list_get_element_by_id(guimp->layout.elements, "layer_menu");
	ui_menu_get_menu(guimp->layer_parent)->event_and_render_children = 1;
	// Layer buttons
	guimp->button_add_layer = ui_list_get_element_by_id(guimp->layout.elements, "button_add_layer");
	guimp->button_remove_layer = ui_list_get_element_by_id(guimp->layout.elements, "button_remove_layer");
	guimp->button_edit_layer = ui_list_get_element_by_id(guimp->layout.elements, "button_edit_layer");
	guimp->button_move_layer_up = ui_list_get_element_by_id(guimp->layout.elements, "button_move_layer_up");
	guimp->button_move_layer_down = ui_list_get_element_by_id(guimp->layout.elements, "button_move_layer_down");
	// Color Stuff
	guimp->color_swatch = ui_list_get_element_by_id(guimp->layout.elements, "color_swatch");
	guimp->red_slider = ui_list_get_element_by_id(guimp->layout.elements, "r_slider");
	guimp->green_slider = ui_list_get_element_by_id(guimp->layout.elements, "g_slider");
	guimp->blue_slider = ui_list_get_element_by_id(guimp->layout.elements, "b_slider");
	guimp->alpha_slider = ui_list_get_element_by_id(guimp->layout.elements, "a_slider");
	// size slider get
	guimp->size_slider = ui_list_get_element_by_id(guimp->layout.elements, "size_slider");	
	// text input
	guimp->text_input = ui_list_get_element_by_id(guimp->layout.elements, "text_input");	
	guimp->text_input_str = ui_input_label_get(guimp->text_input)->text;
	// loading fonts and stickers to the dropdown
	guimp->font_dropdown = ui_list_get_element_by_id(guimp->layout.elements, "font_drop");
	guimp->sticker_dropdown = ui_list_get_element_by_id(guimp->layout.elements, "sticker_drop");
	// other buttons
	guimp->save_button = ui_list_get_element_by_id(guimp->layout.elements, "save_button");
	guimp->edit_button = ui_list_get_element_by_id(guimp->layout.elements, "edit_image_button");
	guimp->clear_button = ui_list_get_element_by_id(guimp->layout.elements, "clear_button");
}

void	new_layer_window_init(t_guimp *guimp)
{
	guimp->win_layer_new = ui_list_get_window_by_id(guimp->layout.windows, "new_layer_window");
	if (!guimp->win_layer_new)
	{
		ft_printf("[%s] Couldnt find window from layout.\n", __FUNCTION__);
		exit(0);
	}
	else
		ft_printf("[%s] Correct window got.\n", __FUNCTION__);
	guimp->new_layer_ok_button = ui_list_get_element_by_id(guimp->layout.elements, "button_ok");
	guimp->new_layer_name_input_label = ui_input_label_get(ui_list_get_element_by_id(guimp->layout.elements, "input_name"));
	guimp->new_layer_width_input_label = ui_input_label_get(ui_list_get_element_by_id(guimp->layout.elements, "input_width"));
	guimp->new_layer_height_input_label = ui_input_label_get(ui_list_get_element_by_id(guimp->layout.elements, "input_height"));

	// New Image win
	guimp->win_image_edit = ui_list_get_window_by_id(guimp->layout.windows, "image_edit_window");
	guimp->new_image_ok_button = ui_list_get_element_by_id(guimp->layout.elements, "button_ok_image");
	guimp->new_image_width_input_label = ui_input_label_get(ui_list_get_element_by_id(guimp->layout.elements, "input_width_image"));
	guimp->new_image_height_input_label = ui_input_label_get(ui_list_get_element_by_id(guimp->layout.elements, "input_height_image"));
}

void	edit_layer_window_init(t_guimp *guimp)
{
	guimp->win_layer_edit = ui_list_get_window_by_id(guimp->layout.windows, "window_edit_layer");
	guimp->button_edit_layer_ok = ui_list_get_element_by_id(guimp->layout.elements, "button_edit_layer_ok");
	guimp->input_edit_layer_name = ui_list_get_element_by_id(guimp->layout.elements, "input_edit_layer_name");
	guimp->input_edit_layer_width = ui_list_get_element_by_id(guimp->layout.elements, "input_edit_layer_width");
	guimp->input_edit_layer_height = ui_list_get_element_by_id(guimp->layout.elements, "input_edit_layer_height");
}

void	save_image_window_init(t_guimp *guimp)
{
	guimp->win_save_image = ui_list_get_window_by_id(guimp->layout.windows, "window_save_image");
	guimp->input_save_image_name = ui_list_get_element_by_id(guimp->layout.elements, "input_save_image_name");
	guimp->button_save_image_ok = ui_list_get_element_by_id(guimp->layout.elements, "button_save_image_ok");
}

int	main(void)
{
	t_guimp		guimp;
	int			run;
	SDL_Event	e;

	ui_sdl_init();

	/*
	 * TESTING
	int	iter = 1000;
	int	i;
	t_vec2i wh = vec2i(1000, 1000);
	t_vec2i v1 = vec2i(0, 0);
	t_vec2i v2 = vec2i(wh.x - 1, wh.y - 1);
	SDL_Surface *surf = ui_surface_new(wh.x, wh.y);

	i = -1;
	ft_timer_start();
	while (++i < iter)
		ui_surface_line_draw_thicc(surf, v1, v2, 1, 0);
	ft_printf("Anti-Alien (AA): %f\n", ft_timer_end());

	i = -1;
	ft_timer_start();
	while (++i < iter)
		ui_surface_line_draw(surf, v1, v2, 0);
	ft_printf("Current Best (CBA): %f\n", ft_timer_end());

	exit(0);
	*/
	/*
	 * TESTING END
	*/

	guimp_init(&guimp);
	toolbox_window_init(&guimp);
	new_layer_window_init(&guimp);
	edit_layer_window_init(&guimp);
	save_image_window_init(&guimp);
	ft_printf("All Inits done.\n");

	run = 1;

	ui_radio_new(guimp.win_toolbox, &guimp.radio_layer);
	guimp.radio_buttons = guimp.radio_layer.children;

	// Brush Buttons
	guimp.draw_button = ui_list_get_element_by_id(guimp.layout.elements, "draw_button");
	guimp.text_button = ui_list_get_element_by_id(guimp.layout.elements, "text_button");
	guimp.erase_button = ui_list_get_element_by_id(guimp.layout.elements, "erase_button");
	guimp.flood_button = ui_list_get_element_by_id(guimp.layout.elements, "flood_button");
	guimp.sticker_button = ui_list_get_element_by_id(guimp.layout.elements, "sticker_button");
	guimp.move_button = ui_list_get_element_by_id(guimp.layout.elements, "move_button");
	guimp.shape_button = ui_list_get_element_by_id(guimp.layout.elements, "shape_button");
	guimp.pipette_button = ui_list_get_element_by_id(guimp.layout.elements, "pipette_button");

	// Shape Buttons
	guimp.circle_button = ui_list_get_element_by_id(guimp.layout.elements, "circle_button");
	guimp.square_button = ui_list_get_element_by_id(guimp.layout.elements, "square_button");
	guimp.line_button = ui_list_get_element_by_id(guimp.layout.elements, "line_button");

	new_layer_combination(&guimp); // lets make default 1 layer;
	guimp.win_main->user_handled_event = 1;
	guimp.win_toolbox->user_handled_event = 1;

	while (run)
	{
		if (guimp.win_toolbox->wants_to_close)
			run = 0;
		while (SDL_PollEvent(&e))
		{
			ui_window_event(guimp.win_main, e);
			ui_window_event(guimp.win_toolbox, e);
			// Drop Event
			if (e.drop.type == SDL_DROPFILE && e.drop.windowID == guimp.win_main->window_id)
			{
				SDL_Surface	*dropped_image;

				ft_printf("File : %s dropped on windowID %d\n", e.drop.file, e.drop.windowID);
				new_layer_combination(&guimp);
				ui_label_text_set(ui_button_get_label_element(ui_list_get_element_by_id(guimp.layer_elems[guimp.layer_amount - 1]->children, "layer_select_button")), e.drop.file);
				dropped_image = ui_surface_image_new(e.drop.file);	
				resize_layer(&guimp.layers[guimp.layer_amount - 1], vec2i(dropped_image->w, dropped_image->h));
				SDL_BlitSurface(dropped_image, NULL, guimp.layers[guimp.layer_amount - 1].surface, NULL);
				SDL_FreeSurface(dropped_image);
			}
			if (e.key.keysym.scancode == SDL_SCANCODE_SPACE) // reseting the final_image to the center of the screen if it has disappeard somewhere.
			{
				guimp.zoom = 1.0;
				guimp.final_image.pos.x = guimp.win_main->pos.w / 2 - guimp.final_image.pos.w / 2;
				guimp.final_image.pos.y = guimp.win_main->pos.h / 2 - guimp.final_image.pos.h / 2;
			}
			if (e.key.keysym.scancode == SDL_SCANCODE_P)
			{
				ui_element_print(ui_list_get_element_by_id(guimp.layout.elements, "label_edit_layer_name"));
			}
			// dropdown elems will be evented here, we dont want anything else to get evented if its open;
			// i hope i come up with other way of doing this.
			if (ui_dropdown_get_dropdown(guimp.font_dropdown)->menu.show)
			{
				ui_dropdown_event(guimp.font_dropdown, e);
			}
			else if (ui_dropdown_get_dropdown(guimp.sticker_dropdown)->menu.show)
			{
				ui_dropdown_event(guimp.sticker_dropdown, e);
			}
			else
			{
				// Event
				ui_layout_event_v2(&guimp.layout, e);

				// Layer
				layer_event(&guimp);

				ui_radio_event(&guimp.radio_layer, e);
			}
		}

		// User
		user_events(&guimp);
		user_code(&guimp);
		user_render(&guimp);
		
		// Layer
		layer_elements_render(&guimp);
		layer_draw(&guimp);
		layer_render(&guimp);

		// Render
		ui_layout_render_v2(&guimp.layout);
	}
	return (0);
}

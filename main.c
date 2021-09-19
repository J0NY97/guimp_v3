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

/*
 * Make ui_element of all the fonts found in the fonts dir.
 * Adds the elements to the fonts dropdown.
*/
void	load_fonts(t_guimp *guimp)
{
	t_ui_recipe_v2		*font_button_recipe;
	t_dir_content	font_dir;
	t_ui_element	*elem;
	char			*temp;
	int				i;
	char			tt[20];

	get_dir_content(&font_dir, "fonts/");
	font_button_recipe = ui_list_get_recipe_by_id_v2(guimp->layout.recipes, "font_button");
	i = -1;
	while (++i < font_dir.file_amount)
	{
		if (ft_strendswith(font_dir.files[i], ".ttf"))
			continue ;
		elem = ft_memalloc(sizeof(t_ui_element));
		ui_element_new(guimp->win_toolbox, elem);
		ui_element_edit(elem, font_button_recipe);
		add_to_drop_menu(guimp->font_dropdown, elem);
		ui_label_text_set(&((t_ui_button *)elem->element)->label, font_dir.files[i]);
		ft_printf("font button %s made.\n", font_dir.files[i]);
	}
	free_dir_content(&font_dir);
}

/*
 * the same as the fonts, but takes stickers from the stickers dir.
*/
void	load_stickers(t_guimp *guimp)
{
	t_ui_recipe_v2		*font_button_recipe;
	t_dir_content	sticker_dir;
	t_ui_element	*elem;
	int				i;

	get_dir_content(&sticker_dir, "stickers/");
	font_button_recipe = ui_list_get_recipe_by_id_v2(guimp->layout.recipes, "font_button");
	i = -1;
	while (++i < sticker_dir.file_amount)
	{
		if (ft_strendswith(sticker_dir.files[i], ".jpg"))
			if (ft_strendswith(sticker_dir.files[i], ".png"))
				if (ft_strendswith(sticker_dir.files[i], ".bmp"))
					continue ;
		elem = ft_memalloc(sizeof(t_ui_element));
		ui_element_new(guimp->win_toolbox, elem);
		ui_element_edit(elem, font_button_recipe);
		add_to_drop_menu(guimp->sticker_dropdown, elem);
		ui_label_text_set(&((t_ui_button *)elem->element)->label, sticker_dir.files[i]);
		ft_printf("sticker button %s made.\n", sticker_dir.files[i]);
	}
	free_dir_content(&sticker_dir);
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
	guimp->font_radio = ui_list_get_element_by_id(guimp->layout.elements, "font_radio");
//	load_fonts(guimp);
	guimp->sticker_dropdown = ui_list_get_element_by_id(guimp->layout.elements, "sticker_drop");
	guimp->sticker_radio = ui_list_get_element_by_id(guimp->layout.elements, "sticker_radio");
//	load_stickers(guimp);
	// other buttons
	guimp->save_button = ui_list_get_element_by_id(guimp->layout.elements, "save_button");
	guimp->edit_button = ui_list_get_element_by_id(guimp->layout.elements, "edit_image_button");
	guimp->clear_button = ui_list_get_element_by_id(guimp->layout.elements, "clear_button");
}

void	new_layer_window_init(t_guimp *guimp)
{
	guimp->win_layer_new = ui_list_get_window_by_id(guimp->layout.windows, "layer_edit_window");
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

	guimp_init(&guimp);
	toolbox_window_init(&guimp);
	new_layer_window_init(&guimp);
	edit_layer_window_init(&guimp);
	save_image_window_init(&guimp);
	ft_printf("All Inits done.\n");

	run = 1;

	/*
	 * Testing
	*/
	ui_radio_new(guimp.win_toolbox, &guimp.radio_layer);
	guimp.radio_buttons = guimp.radio_layer.children;

	guimp.draw_button = ui_list_get_element_by_id(guimp.layout.elements, "draw_button");
	guimp.text_button = ui_list_get_element_by_id(guimp.layout.elements, "text_button");
	guimp.erase_button = ui_list_get_element_by_id(guimp.layout.elements, "erase_button");
	guimp.flood_button = ui_list_get_element_by_id(guimp.layout.elements, "flood_button");
	guimp.sticker_button = ui_list_get_element_by_id(guimp.layout.elements, "sticker_button");
	guimp.move_button = ui_list_get_element_by_id(guimp.layout.elements, "move_button");
	guimp.shape_button = ui_list_get_element_by_id(guimp.layout.elements, "shape_button");
	guimp.pipette_button = ui_list_get_element_by_id(guimp.layout.elements, "pipette_button");
	guimp.mode_button_list = NULL;
	add_to_list(&guimp.mode_button_list, guimp.draw_button, UI_TYPE_ELEMENT);
	add_to_list(&guimp.mode_button_list, guimp.text_button, UI_TYPE_ELEMENT);
	add_to_list(&guimp.mode_button_list, guimp.erase_button, UI_TYPE_ELEMENT);
	add_to_list(&guimp.mode_button_list, guimp.flood_button, UI_TYPE_ELEMENT);
	add_to_list(&guimp.mode_button_list, guimp.sticker_button, UI_TYPE_ELEMENT);
	add_to_list(&guimp.mode_button_list, guimp.move_button, UI_TYPE_ELEMENT);
	add_to_list(&guimp.mode_button_list, guimp.shape_button, UI_TYPE_ELEMENT);
	add_to_list(&guimp.mode_button_list, guimp.pipette_button, UI_TYPE_ELEMENT);

	ui_radio_new(guimp.win_toolbox, &guimp.radio_mode_buttons);
	guimp.radio_mode_buttons.children = guimp.mode_button_list;

	guimp.circle_button = ui_list_get_element_by_id(guimp.layout.elements, "circle_button");
	guimp.square_button = ui_list_get_element_by_id(guimp.layout.elements, "square_button");
	guimp.line_button = ui_list_get_element_by_id(guimp.layout.elements, "line_button");
	guimp.shape_button_list = NULL;
	add_to_list(&guimp.shape_button_list, guimp.circle_button, UI_TYPE_ELEMENT);
	add_to_list(&guimp.shape_button_list, guimp.square_button, UI_TYPE_ELEMENT);
	add_to_list(&guimp.shape_button_list, guimp.line_button, UI_TYPE_ELEMENT);

	ui_radio_new(guimp.win_toolbox, &guimp.radio_shape_buttons);
	guimp.radio_shape_buttons.children = guimp.shape_button_list;

	ft_printf("Testing done.\n");
	/*
	 * Testing END
	*/
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
			if (e.key.keysym.scancode == SDL_SCANCODE_SPACE) // reseting the final_image to the center of the screen if it has disappeard somewhere.
			{
				guimp.zoom = 1.0;
				guimp.final_image.pos.x = guimp.win_main->pos.w / 2 - guimp.final_image.pos.w / 2;
				guimp.final_image.pos.y = guimp.win_main->pos.h / 2 - guimp.final_image.pos.h / 2;
			}
			if (e.key.keysym.scancode == SDL_SCANCODE_P)
				ui_element_print(((t_ui_radio *)guimp.radio_layer.element)->active);
			// dropdown elems will be evented here, we dont want anything else to get evented if its open;
			// i hope i come up with other way of doing this.
			if (((t_ui_dropdown *)guimp.font_dropdown->element)->menu.show)
			{
				ui_dropdown_event(guimp.font_dropdown, e);
				ui_radio_event(guimp.font_radio, e);
			}
			else if (((t_ui_dropdown *)guimp.sticker_dropdown->element)->menu.show)
			{
				ui_dropdown_event(guimp.sticker_dropdown, e);
				ui_radio_event(guimp.sticker_radio, e);
			}
			else
			{
				// Event
				ui_layout_event_v2(&guimp.layout, e);

				// Layer
				layer_elements_event(&guimp, e);
				layer_event(&guimp, e);

				ui_radio_event(&guimp.radio_layer, e);
				ui_radio_event(&guimp.radio_mode_buttons, e);
				ui_radio_event(&guimp.radio_shape_buttons, e);
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

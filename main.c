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
	t_ui_recipe		*font_button_recipe;
	t_dir_content	font_dir;
	t_ui_element	*elem;
	char			*temp;
	int				i;
	char			tt[20];

	get_dir_content(&font_dir, "fonts/");
	font_button_recipe = ui_layout_get_recipe_by_id(&guimp->layout, "font_button");
	i = -1;
	while (++i < font_dir.file_amount)
	{
		if (ft_strendswith(font_dir.files[i], ".ttf"))
			continue ;
		elem = ui_element_create_from_recipe(guimp->win_toolbox, font_button_recipe, &guimp->layout);
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
	t_ui_recipe		*font_button_recipe;
	t_dir_content	sticker_dir;
	t_ui_element	*elem;
	int				i;

	get_dir_content(&sticker_dir, "stickers/");
	font_button_recipe = ui_layout_get_recipe_by_id(&guimp->layout, "font_button");
	i = -1;
	while (++i < sticker_dir.file_amount)
	{
		if (ft_strendswith(sticker_dir.files[i], ".jpg"))
			if (ft_strendswith(sticker_dir.files[i], ".png"))
				if (ft_strendswith(sticker_dir.files[i], ".bmp"))
					continue ;
		elem = ui_element_create_from_recipe(guimp->win_toolbox, font_button_recipe, &guimp->layout);
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
	ui_layout_load(&guimp->layout, "layout.ui");
	// Main Win
	guimp->win_main = ui_layout_get_window_by_id(&guimp->layout, "main_window");
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
	guimp->win_toolbox = ui_layout_get_window_by_id(&guimp->layout, "toolbox_window");
	guimp->layer_recipe = ui_layout_get_recipe_by_id(&guimp->layout, "layer");
	guimp->layer_parent = ui_layout_get_element_by_id(&guimp->layout, "layer_menu");
	// Layer buttons
	guimp->button_add_layer = ui_layout_get_element_by_id(&guimp->layout, "button_add_layer");
	guimp->button_remove_layer = ui_layout_get_element_by_id(&guimp->layout, "button_remove_layer");
	guimp->button_edit_layer = ui_layout_get_element_by_id(&guimp->layout, "button_edit_layer");
	guimp->button_move_layer_up = ui_layout_get_element_by_id(&guimp->layout, "button_move_layer_up");
	guimp->button_move_layer_down = ui_layout_get_element_by_id(&guimp->layout, "button_move_layer_down");
	// Color Stuff
	guimp->color_swatch = ui_layout_get_element_by_id(&guimp->layout, "color_swatch");
	guimp->red_slider = ui_layout_get_element_by_id(&guimp->layout, "r_slider");
	guimp->green_slider = ui_layout_get_element_by_id(&guimp->layout, "g_slider");
	guimp->blue_slider = ui_layout_get_element_by_id(&guimp->layout, "b_slider");
	guimp->alpha_slider = ui_layout_get_element_by_id(&guimp->layout, "a_slider");
	ui_slider_render(guimp->red_slider); // this needs to go
	ui_slider_render(guimp->blue_slider); // this needs to go
	ui_slider_render(guimp->green_slider); // this needs to go
	ui_texture_fill_rect(guimp->red_slider->win->renderer, guimp->red_slider->textures[UI_STATE_DEFAULT], 0xffff0000, vec4i(0, guimp->red_slider->pos.h / 3, guimp->red_slider->pos.w, guimp->red_slider->pos.h / 3));
	ui_texture_fill_rect(guimp->green_slider->win->renderer, guimp->green_slider->textures[UI_STATE_DEFAULT], 0xff00ff00, vec4i(0, guimp->green_slider->pos.h / 3, guimp->green_slider->pos.w, guimp->green_slider->pos.h / 3));
	ui_texture_fill_rect(guimp->blue_slider->win->renderer, guimp->blue_slider->textures[UI_STATE_DEFAULT], 0xff0000ff, vec4i(0, guimp->blue_slider->pos.h / 3, guimp->blue_slider->pos.w, guimp->blue_slider->pos.h / 3));
	// size slider get
	guimp->size_slider = ui_layout_get_element_by_id(&guimp->layout, "size_slider");	
	// text input
	guimp->text_input = ui_layout_get_element_by_id(&guimp->layout, "text_input");	
	guimp->text_input_str = ui_input_label_get(guimp->text_input)->text;
	// loading fonts and stickers to the dropdown
	guimp->font_dropdown = ui_layout_get_element_by_id(&guimp->layout, "font_drop");
	load_fonts(guimp);
	guimp->sticker_dropdown = ui_layout_get_element_by_id(&guimp->layout, "sticker_drop");
	load_stickers(guimp);
	// other buttons
	guimp->save_button = ui_layout_get_element_by_id(&guimp->layout, "save_button");
	guimp->edit_button = ui_layout_get_element_by_id(&guimp->layout, "edit_image_button");
}

void	new_layer_window_init(t_guimp *guimp)
{
	int	result;

	result = ui_layout_load(&guimp->layout_layer, "layer_new.ui");
	if (!result)
	{
		ft_printf("[%s] Couldnt open/read layout.\n", __FUNCTION__);
		exit(0);
	}
	else
		ft_printf("[%s] Layout read and returned.\n", __FUNCTION__);
	guimp->win_layer_new = ui_layout_get_window_by_id(&guimp->layout_layer, "layer_edit_window");
	if (!guimp->win_layer_new)
	{
		ft_printf("[%s] Couldnt find window from layout.\n", __FUNCTION__);
		exit(0);
	}
	else
		ft_printf("[%s] Correct window got.\n", __FUNCTION__);
	guimp->new_layer_ok_button = ui_layout_get_element_by_id(&guimp->layout_layer, "button_ok");
	guimp->new_layer_name_input_label = ui_input_label_get(ui_layout_get_element_by_id(&guimp->layout_layer, "input_name"));
	guimp->new_layer_width_input_label = ui_input_label_get(ui_layout_get_element_by_id(&guimp->layout_layer, "input_width"));
	guimp->new_layer_height_input_label = ui_input_label_get(ui_layout_get_element_by_id(&guimp->layout_layer, "input_height"));

	// New Image win
	guimp->win_image_edit = ui_layout_get_window_by_id(&guimp->layout_layer, "image_edit_window");
	guimp->new_image_ok_button = ui_layout_get_element_by_id(&guimp->layout_layer, "button_ok_image");
	guimp->new_image_width_input_label = ui_input_label_get(ui_layout_get_element_by_id(&guimp->layout_layer, "input_width_image"));
	guimp->new_image_height_input_label = ui_input_label_get(ui_layout_get_element_by_id(&guimp->layout_layer, "input_height_image"));
}

void	edit_layer_window_init(t_guimp *guimp)
{
	guimp->win_layer_edit = ui_layout_get_window_by_id(&guimp->layout, "window_edit_layer");
	guimp->button_edit_layer_ok = ui_layout_get_element_by_id(&guimp->layout, "button_edit_layer_ok");
	guimp->input_edit_layer_name = ui_layout_get_element_by_id(&guimp->layout, "input_edit_layer_name");
	guimp->input_edit_layer_width = ui_layout_get_element_by_id(&guimp->layout, "input_edit_layer_width");
	guimp->input_edit_layer_height = ui_layout_get_element_by_id(&guimp->layout, "input_edit_layer_height");
}

void	save_image_window_init(t_guimp *guimp)
{
	guimp->win_save_image = ui_layout_get_window_by_id(&guimp->layout, "window_save_image");
	guimp->input_save_image_name = ui_layout_get_element_by_id(&guimp->layout, "input_save_image_name");
	guimp->button_save_image_ok = ui_layout_get_element_by_id(&guimp->layout, "button_save_image_ok");
}

int	main(void)
{
	t_guimp		guimp;
	int			run;
	SDL_Event	e;

	ui_sdl_init();
	/*
	 * line testing
	*/
	/*
	int	w = 1000;
	int	iter = 24000;
	SDL_Surface	*surface = ui_surface_new(w, w);
	int	i;
	ft_timer_start();
	i = -1;
	while (++i < iter)
	{
		ui_surface_line_draw(surface, vec2i(0, 0), vec2i(w - 1, 0), 0x00); // left, right
		ui_surface_line_draw(surface, vec2i(w - 1, 0), vec2i(0, 0), 0x00); // right, left
		ui_surface_line_draw(surface, vec2i(0, 0), vec2i(0, w - 1), 0x00); // top, bot
		ui_surface_line_draw(surface, vec2i(0, w - 1), vec2i(0, 0), 0x00); // bot , top

		ui_surface_line_draw(surface, vec2i(0, 0), vec2i(w - 1, w - 1), 0x00); // top left, bot right
		ui_surface_line_draw(surface, vec2i(w - 1, 0), vec2i(0, w - 1), 0x00); // top right, bot left
		ui_surface_line_draw(surface, vec2i(0, w - 1), vec2i(w - 1, 0), 0x00); // bot left, top right
		ui_surface_line_draw(surface, vec2i(w - 1, w - 1), vec2i(0, 0), 0x00); // bot right, top left
	}
	ft_printf("Reduced : %f\n", ft_timer_end());
	ft_timer_start();
	i = -1;
	while (++i < iter)
	{
		ui_surface_line_draw_v2(surface, vec2i(0, 0), vec2i(w - 1, 0), 0x00); // left, right
		ui_surface_line_draw_v2(surface, vec2i(w - 1, 0), vec2i(0, 0), 0x00); // right, left
		ui_surface_line_draw_v2(surface, vec2i(0, 0), vec2i(0, w - 1), 0x00); // top, bot
		ui_surface_line_draw_v2(surface, vec2i(0, w - 1), vec2i(0, 0), 0x00); // bot , top

		ui_surface_line_draw_v2(surface, vec2i(0, 0), vec2i(w - 1, w - 1), 0x00); // top left, bot right
		ui_surface_line_draw_v2(surface, vec2i(w - 1, 0), vec2i(0, w - 1), 0x00); // top right, bot left
		ui_surface_line_draw_v2(surface, vec2i(0, w - 1), vec2i(w - 1, 0), 0x00); // bot left, top right
		ui_surface_line_draw_v2(surface, vec2i(w - 1, w - 1), vec2i(0, 0), 0x00); // bot right, top left
	}
	ft_printf("reduced_v2 %f\n", ft_timer_end());
	ft_timer_start();
	i = -1;
	while (++i < iter)
	{
		ui_surface_line_draw_nik(surface, vec2i(0, 0), vec2i(w - 1, 0), 0x00); // left, right
		ui_surface_line_draw_nik(surface, vec2i(w - 1, 0), vec2i(0, 0), 0x00); // right, left
		ui_surface_line_draw_nik(surface, vec2i(0, 0), vec2i(0, w - 1), 0x00); // top, bot
		ui_surface_line_draw_nik(surface, vec2i(0, w - 1), vec2i(0, 0), 0x00); // bot , top

		ui_surface_line_draw_nik(surface, vec2i(0, 0), vec2i(w - 1, w - 1), 0x00); // top left, bot right
		ui_surface_line_draw_nik(surface, vec2i(w - 1, 0), vec2i(0, w - 1), 0x00); // top right, bot left
		ui_surface_line_draw_nik(surface, vec2i(0, w - 1), vec2i(w - 1, 0), 0x00); // bot left, top right
		ui_surface_line_draw_nik(surface, vec2i(w - 1, w - 1), vec2i(0, 0), 0x00); // bot right, top left
	}
	ft_printf("Nik : %f\n", ft_timer_end());
	ft_timer_start();
	i = -1;
	while (++i < iter)
	{
		ui_surface_line_draw_orig(surface, vec2i(0, 0), vec2i(w - 1, 0), 0x00); // left, right
		ui_surface_line_draw_orig(surface, vec2i(w - 1, 0), vec2i(0, 0), 0x00); // right, left
		ui_surface_line_draw_orig(surface, vec2i(0, 0), vec2i(0, w - 1), 0x00); // top, bot
		ui_surface_line_draw_orig(surface, vec2i(0, w - 1), vec2i(0, 0), 0x00); // bot , top

		ui_surface_line_draw_orig(surface, vec2i(0, 0), vec2i(w - 1, w - 1), 0x00); // top left, bot right
		ui_surface_line_draw_orig(surface, vec2i(w - 1, 0), vec2i(0, w - 1), 0x00); // top right, bot left
		ui_surface_line_draw_orig(surface, vec2i(0, w - 1), vec2i(w - 1, 0), 0x00); // bot left, top right
		ui_surface_line_draw_orig(surface, vec2i(w - 1, w - 1), vec2i(0, 0), 0x00); // bot right, top left
	}
	ft_printf("Orig : %f\n", ft_timer_end());
	exit(0);
	*/
	/*
	 * line testing END
	*/

	/*
	 * circle testing
	*/
/*
	int	w = 1000;
	int	r = w / 2;
	int	iter = 1000;
	SDL_Surface	*surface = ui_surface_new(w, w);
	int	i;

	ft_timer_start();
	i = -1;
	while (++i < iter)
		ui_surface_circle_draw_filled(surface, vec2i(r, r), r, 0x00);
	ft_printf("v1 : %f\n", ft_timer_end());

	ft_timer_start();
	i = -1;
	while (++i < iter)
		ui_surface_circle_draw_filled_v2(surface, vec2i(r, r), r, 0x00);
	ft_printf("v2 : %f\n", ft_timer_end());

	exit(0);
 */
	/*
	 * circle testing
	*/

	guimp_init(&guimp);
	toolbox_window_init(&guimp);
	new_layer_window_init(&guimp);
	edit_layer_window_init(&guimp);
	save_image_window_init(&guimp);
	run = 1;

	/*
	 * Testing
	*/
	ui_radio_new(guimp.win_toolbox, &guimp.radio_layer);
	guimp.radio_buttons = ((t_ui_radio *)guimp.radio_layer.element)->buttons;

	guimp.draw_button = ui_layout_get_element_by_id(&guimp.layout, "draw_button");
	guimp.text_button = ui_layout_get_element_by_id(&guimp.layout, "text_button");
	guimp.erase_button = ui_layout_get_element_by_id(&guimp.layout, "erase_button");
	guimp.flood_button = ui_layout_get_element_by_id(&guimp.layout, "flood_button");
	guimp.sticker_button = ui_layout_get_element_by_id(&guimp.layout, "sticker_button");
	guimp.move_button = ui_layout_get_element_by_id(&guimp.layout, "move_button");
	guimp.shape_button = ui_layout_get_element_by_id(&guimp.layout, "shape_button");
	guimp.pipette_button = ui_layout_get_element_by_id(&guimp.layout, "pipette_button");
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
	((t_ui_radio *)guimp.radio_mode_buttons.element)->buttons = guimp.mode_button_list;

	guimp.circle_button = ui_layout_get_element_by_id(&guimp.layout, "circle_button");
	guimp.square_button = ui_layout_get_element_by_id(&guimp.layout, "square_button");
	guimp.line_button = ui_layout_get_element_by_id(&guimp.layout, "line_button");
	guimp.shape_button_list = NULL;
	add_to_list(&guimp.shape_button_list, guimp.circle_button, UI_TYPE_ELEMENT);
	add_to_list(&guimp.shape_button_list, guimp.square_button, UI_TYPE_ELEMENT);
	add_to_list(&guimp.shape_button_list, guimp.line_button, UI_TYPE_ELEMENT);

	ui_radio_new(guimp.win_toolbox, &guimp.radio_shape_buttons);
	((t_ui_radio *)guimp.radio_shape_buttons.element)->buttons = guimp.shape_button_list;

	// drops
	ui_radio_new(guimp.win_toolbox, &guimp.font_radio);
	((t_ui_radio *)guimp.font_radio.element)->buttons = ((t_ui_menu *)((t_ui_dropdown *)guimp.font_dropdown->element)->menu.element)->children;;

	ui_radio_new(guimp.win_toolbox, &guimp.sticker_radio);
	((t_ui_radio *)guimp.sticker_radio.element)->buttons = ((t_ui_menu *)((t_ui_dropdown *)guimp.sticker_dropdown->element)->menu.element)->children;;
	/*
	 * Testing END
	*/
	new_layer_combination(&guimp); // lets make default 1 layer;
	guimp.win_main->user_handled_event = 1;
	guimp.win_toolbox->user_handled_event = 1;
	while (run)
	{
		while (SDL_PollEvent(&e))
		{
			ui_window_event(guimp.win_main, e);
			ui_window_event(guimp.win_toolbox, e);
			if (guimp.win_toolbox->wants_to_close)
				run = 0;
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
				ui_radio_event(&guimp.font_radio, e);
			}
			else if (((t_ui_dropdown *)guimp.sticker_dropdown->element)->menu.show)
			{
				ui_dropdown_event(guimp.sticker_dropdown, e);
				ui_radio_event(&guimp.sticker_radio, e);
			}
			else
			{
			// Event
			ui_layout_event(&guimp.layout, e);
			ui_layout_event(&guimp.layout_layer, e);

			// Layer
			layer_elements_event(&guimp, e);
			layer_event(&guimp, e);

			ui_radio_event(&guimp.radio_layer, e);
			ui_radio_event(&guimp.radio_mode_buttons, e);
			ui_radio_event(&guimp.radio_shape_buttons, e);
			/*
			 * Testing
			*/
			/*
			 * Testing end
			*/
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

		/*
		 * Testing
		*/
		/*
		 * Testing END
		*/

		// Render
		ui_layout_render(&guimp.layout);
		ui_layout_render(&guimp.layout_layer);
	}
	return (0);
}

#include "guimp.h"

void	user_events(t_guimp *guimp)
{
	layer_plus_button_event(guimp);
	new_layer_ok_button_event(guimp);
	color_swatch_event(guimp);
}

void	user_render(t_guimp *guimp)
{

}

void	user_code(t_guimp *guimp)
{
	(void)guimp;
}

void	guimp_init(t_guimp *guimp)
{
	memset(guimp, 0, sizeof(t_guimp));
	SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
	ui_layout_load(&guimp->layout, "layout.ui");
	// Main Win
	guimp->win_main = ui_layout_get_window_by_id(&guimp->layout, "main_window");
	new_layer(&guimp->final_image, "Image", vec4i(10, 10, 1280, 720), NULL);
	SDL_FillRect(guimp->final_image.surface, NULL, 0xff000000); // fill image with black so the alpha:ed layers can show.
	guimp->final_image_texture = NULL;
	guimp->selected_layer = -1;
	guimp->layer_amount = 0;
	guimp->combined_color = 0xffffffff;
	guimp->zoom = 1.0f;
	// Toolbox Win
	guimp->win_toolbox = ui_layout_get_window_by_id(&guimp->layout, "toolbox_window");
	guimp->layer_recipe = ui_layout_get_recipe_by_id(&guimp->layout, "layer");
	guimp->layer_parent = ui_layout_get_element_by_id(&guimp->layout, "layer_menu");
	guimp->layer_plus_button = ui_layout_get_element_by_id(&guimp->layout, "button_add_layer");
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
}

void	layer_edit_window_init(t_guimp	*guimp)
{
	int	result;

	result = ui_layout_load(&guimp->layout_layer_edit, "layer_edit.ui");
	if (!result)
	{
		ft_printf("[%s] Couldnt open/read layout.\n", __FUNCTION__);
		exit(0);
	}
	else
		ft_printf("[%s] Layout read and returned.\n", __FUNCTION__);
	guimp->win_layer_edit = ui_layout_get_window_by_id(&guimp->layout_layer_edit, "layer_edit_window");
	if (!guimp->win_layer_edit)
	{
		ft_printf("[%s] Couldnt find window from layout.\n", __FUNCTION__);
		exit(0);
	}
	else
		ft_printf("[%s] Correct window got.\n", __FUNCTION__);
	guimp->new_layer_ok_button = ui_layout_get_element_by_id(&guimp->layout_layer_edit, "button_ok");
	guimp->new_layer_name_input_label = ((t_ui_input *)ui_layout_get_element_by_id(&guimp->layout_layer_edit, "input_name")->element)->label.element;
	guimp->new_layer_width_input_label = ((t_ui_input *)ui_layout_get_element_by_id(&guimp->layout_layer_edit, "input_width")->element)->label.element;
	guimp->new_layer_height_input_label = ((t_ui_input *)ui_layout_get_element_by_id(&guimp->layout_layer_edit, "input_height")->element)->label.element;
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
	*/
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
	/*
	 * line testing END
	*/
	guimp_init(&guimp);
	layer_edit_window_init(&guimp);
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
	/*
	 * Testing END
	*/
	while (run)
	{
		while (SDL_PollEvent(&e))
		{
			if (guimp.win_toolbox->wants_to_close)
				run = 0;
			// Event
			ui_layout_event(&guimp.layout, e);
			ui_layout_event(&guimp.layout_layer_edit, e);

			// Layer
			layer_elements_event(&guimp, e);
			layer_event(&guimp, e);

			/*
			 * Testing
			*/
			ui_radio_event(&guimp.radio_layer, e);
			ui_radio_event(&guimp.radio_mode_buttons, e);
			ui_radio_event(&guimp.radio_shape_buttons, e);
			/*
			 * Testing end
			*/
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
		ui_layout_render(&guimp.layout_layer_edit);
	}
	return (0);
}

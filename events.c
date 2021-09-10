#include "guimp.h"

t_ui_element	*new_layer_element(t_guimp *guimp, char *layer_name, int nth_layer)
{
	t_ui_element	*menu; // menu
	t_ui_element	*show; // checkbox
	t_ui_element	*select; // radio button
	t_ui_recipe		*recipe_menu;
	t_ui_recipe		*recipe_show;
	t_ui_recipe		*recipe_select;
	char			temp[20];

	recipe_menu = ui_layout_get_recipe_by_id(&guimp->layout, "layer");
	recipe_show = ui_layout_get_recipe_by_id(&guimp->layout, "layer_show_checkbox");
	recipe_select = ui_layout_get_recipe_by_id(&guimp->layout, "layer_select_button");

	menu = ui_element_create_from_recipe(guimp->win_toolbox, recipe_menu, &guimp->layout);
	show = ui_element_create_from_recipe(guimp->win_toolbox, recipe_show, &guimp->layout);
	select = ui_element_create_from_recipe(guimp->win_toolbox, recipe_select, &guimp->layout);

	ui_element_parent_set(menu, guimp->layer_parent, UI_TYPE_ELEMENT);
	ui_element_pos_set2(menu, vec2(menu->pos.x, (menu->pos.h * nth_layer) + (nth_layer * 10) + menu->pos.y));
	ui_element_id_set(menu, ft_strjoin("layer", ft_b_itoa(nth_layer, temp)));

	ui_menu_add(menu, show);
	ui_menu_add(menu, select);

	ui_label_text_set(&((t_ui_button *)select->element)->label, layer_name);

	show->is_click = 1;

	ui_element_print(menu);
	ui_element_print(show);
	ui_element_print(select);

	return (menu);
}

/*
 * These are the layer elements on the toolbox;
*/
void	layer_elements_event(t_guimp *guimp, SDL_Event e)
{
	int	ii;

	ii = -1;
	while (++ii < guimp->layer_amount)
	{
		ui_menu_event(guimp->layer_elems[ii], e);
	}
}

void	layer_elements_render(t_guimp *guimp)
{
	int				ii;

	ii = -1;
	while (++ii < guimp->layer_amount)
	{
		ui_menu_render(guimp->layer_elems[ii]);
	}
}

void	new_layer_combination(t_guimp *guimp)
{
	t_ui_element	*layer_menu;
	t_ui_element	*layer_button;
	char			*new_id;
	char			temp[20];

	if (guimp->layer_amount >= MAX_LAYER_AMOUNT)
	{
		ft_printf("[%s] No new layer added, layer cap reached (%d).\n", __FUNCTION__, MAX_LAYER_AMOUNT);
		return ;
	}
	// Making new layer element
	layer_menu = new_layer_element(guimp, guimp->new_layer_name_input_label->text, guimp->layer_amount);
	guimp->layer_elems[guimp->layer_amount] = layer_menu;
	// adding layer to the radio buttons;
	layer_button = ui_list_get_element_by_id(((t_ui_menu *)layer_menu->element)->children, "layer_select_button");
	add_to_list(&((t_ui_radio *)guimp->radio_layer.element)->buttons, layer_button, UI_TYPE_ELEMENT);

	// Making new actual layer
	t_layer	*layer;

	layer = &guimp->layers[guimp->layer_amount];
	new_layer(layer,
		guimp->new_layer_name_input_label->text,
		vec4i(0, 0, atoi(guimp->new_layer_width_input_label->text), atoi(guimp->new_layer_height_input_label->text)),
		&ui_list_get_element_by_id(((t_ui_menu *)layer_menu->element)->children, "layer_show_checkbox")->is_click);
	ft_printf("New Layer Info : %s, %d %d.\n", layer->name, layer->pos.w, layer->pos.h);
	
	// Make the new layer the selected layer
	guimp->selected_layer = guimp->layer_amount;
	// make function for radio to togggle on specific button through code.
	((t_ui_radio *)guimp->radio_layer.element)->active = layer_button;
	
	guimp->layer_amount += 1;
	ft_printf("[%s] New layer added. (%d)\n", __FUNCTION__, guimp->layer_amount);
}

void	button_add_layer_event(t_guimp *guimp)
{
	if (ui_button(guimp->button_add_layer))
	{
		ui_window_flag_set(guimp->win_layer_new, UI_WINDOW_SHOW);
		SDL_RaiseWindow(guimp->win_layer_new->win);
	}
	// new layer window events
	if (SDL_GetWindowFlags(guimp->win_layer_new->win) & SDL_WINDOW_SHOWN) // i dont want to have to do this (doenst really make sense since the button is_click == 0 and ui_button() should only return 1 if the is_click == 1...)
	{
		if (ui_button(guimp->new_layer_ok_button))
		{
			new_layer_combination(guimp);
			ui_window_flag_set(guimp->win_layer_new, UI_WINDOW_HIDE);
			ui_element_print(guimp->new_layer_ok_button);
		}
	}
}

void	button_edit_layer_event(t_guimp *guimp)
{
	char	temp[20];

	if (guimp->selected_layer < 0 || guimp->selected_layer >= MAX_LAYER_AMOUNT)
		return ;
	if (ui_button(guimp->button_edit_layer))
	{
		ui_window_flag_set(guimp->win_layer_edit, UI_WINDOW_SHOW);
		SDL_RaiseWindow(guimp->win_layer_edit->win);
		guimp->win_layer_edit->textures_recreate = 1; // this is how to fix small bug;
		// fill with the current info of the layer
		ui_label_text_set(&((t_ui_input *)guimp->input_edit_layer_name->element)->label,
			guimp->layers[guimp->selected_layer].name);	
		ui_label_text_set(&((t_ui_input *)guimp->input_edit_layer_width->element)->label,
			ft_b_itoa(guimp->layers[guimp->selected_layer].pos.w, temp));	
		ui_label_text_set(&((t_ui_input *)guimp->input_edit_layer_height->element)->label,
			ft_b_itoa(guimp->layers[guimp->selected_layer].pos.h, temp));	
	}
	// the window events
	if (SDL_GetWindowFlags(guimp->win_layer_edit->win) & SDL_WINDOW_SHOWN) 
	{
		if (ui_button(guimp->button_edit_layer_ok))
		{
			resize_layer(&guimp->layers[guimp->selected_layer], vec2i(
				atoi(((t_ui_label *)((t_ui_input *)guimp->input_edit_layer_width->element)->label.element)->text),
				atoi(((t_ui_label *)((t_ui_input *)guimp->input_edit_layer_height->element)->label.element)->text)));
			ft_strdel(&guimp->layers[guimp->selected_layer].name);
			guimp->layers[guimp->selected_layer].name
					= ft_strdup(((t_ui_label *)((t_ui_input *)guimp->input_edit_layer_name->element)->label.element)->text);
			ui_label_text_set(&((t_ui_button *)ui_list_get_element_by_id(((t_ui_menu *)guimp->layer_elems[guimp->selected_layer]->element)->children, "layer_select_button")->element)->label, guimp->layers[guimp->selected_layer].name);
			ui_window_flag_set(guimp->win_layer_edit, UI_WINDOW_HIDE);
		}
	}
}

void	color_swatch_event(t_guimp *guimp)
{
	char	temp[20];
	t_ui_slider	*red_slider;
	t_ui_slider	*green_slider;
	t_ui_slider	*blue_slider;
	t_ui_slider	*alpha_slider;
	t_ui_element	input_label;

	red_slider = guimp->red_slider->element;
	green_slider = guimp->green_slider->element;
	blue_slider = guimp->blue_slider->element;
	alpha_slider = guimp->alpha_slider->element;
	input_label = ((t_ui_input *)guimp->color_swatch->element)->label;
	if (red_slider->update || green_slider->update || blue_slider->update || alpha_slider->update)
	{
		guimp->combined_color = rgba_to_hex((t_rgba){.r = red_slider->value, .g = green_slider->value, .b = blue_slider->value, .a = alpha_slider->value});
		ui_element_color_set(guimp->color_swatch, UI_STATE_DEFAULT, guimp->combined_color);
		if (!guimp->color_swatch->is_click)
		{
			itoa(guimp->combined_color, temp, 16);
			ui_label_text_set(&input_label, temp);
		}
	}
	else if (guimp->color_swatch->is_click) // from input label set to sliders.
	{
		t_ui_label	*label;
		t_rgba		input_rgba;

		label = input_label.element;
		guimp->combined_color = (unsigned int)strtoul(label->text, NULL, 16);
		input_rgba = hex_to_rgba(guimp->combined_color);
		ui_slider_value_set(guimp->red_slider, input_rgba.r);
		ui_slider_value_set(guimp->green_slider, input_rgba.g);
		ui_slider_value_set(guimp->blue_slider, input_rgba.b);
		ui_slider_value_set(guimp->alpha_slider, input_rgba.a);
	}
	if (((t_ui_slider *)guimp->size_slider->element)->update)
		guimp->size = ((t_ui_slider *)guimp->size_slider->element)->value;
}

void	save_button_event(t_guimp *guimp)
{
	if (ui_button(guimp->save_button))
	{
		ft_printf("i like to party.\n");
		ui_window_flag_set(guimp->win_save_image, UI_WINDOW_SHOW);
		SDL_RaiseWindow(guimp->win_save_image->win);
	}
	if (ui_button(guimp->button_save_image_ok))
		save_surface(guimp->final_image.surface, ui_input_text_get(guimp->input_save_image_name));
}

void	edit_button_event(t_guimp *guimp)
{
	if (ui_button(guimp->edit_button))
	{
		ui_window_flag_set(guimp->win_image_edit, UI_WINDOW_SHOW);
		SDL_RaiseWindow(guimp->win_image_edit->win);
	}
	if (ui_button(guimp->new_image_ok_button))
	{
		int w = atoi(guimp->new_image_width_input_label->text);
		int h = atoi(guimp->new_image_height_input_label->text);
		ft_printf("we want iamge size of : %d %d\n", w, h);
		resize_layer(&guimp->final_image, vec2i(w, h));
		SDL_DestroyTexture(guimp->final_image_texture);
		guimp->final_image_texture = SDL_CreateTextureFromSurface(guimp->win_main->renderer, guimp->final_image.surface);
	}
}



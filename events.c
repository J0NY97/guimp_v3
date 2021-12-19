#include "guimp.h"

t_ui_element	*new_layer_element(t_guimp *guimp, char *layer_name, int nth_layer)
{
	t_ui_element	*menu; // menu
	t_ui_element	*show; // checkbox
	t_ui_element	*image; // menu
	t_ui_element	*select; // radio button
	t_ui_recipe		*recipe_menu;
	t_ui_recipe		*recipe_show;
	t_ui_recipe		*recipe_image;
	t_ui_recipe		*recipe_select;
	char			temp[20];
	char			*pmet;

	recipe_menu = ui_list_get_recipe_by_id(guimp->layout.recipes, "layer");
	recipe_show = ui_list_get_recipe_by_id(guimp->layout.recipes, "layer_show_checkbox");
	recipe_image = ui_list_get_recipe_by_id(guimp->layout.recipes, "layer_image_elem");
	recipe_select = ui_list_get_recipe_by_id(guimp->layout.recipes, "layer_select_button");

	menu = ft_memalloc(sizeof(t_ui_element));
	ui_menu_new(guimp->win_toolbox, menu);
	((t_ui_menu *)menu->element)->event_children = 1;
	((t_ui_menu *)menu->element)->render_children = 1;
	ui_element_set_parent(menu, guimp->layer_parent, UI_TYPE_ELEMENT);
	ui_element_edit(menu, recipe_menu);
	ui_element_pos_set(menu, vec4(recipe_menu->pos.x, (recipe_menu->pos.h * nth_layer) + (nth_layer * 10) + recipe_menu->pos.y, recipe_menu->pos.w, recipe_menu->pos.h));
	pmet = ft_strjoin("layer", ft_b_itoa(nth_layer, temp));
	ui_element_set_id(menu, pmet);
	ft_strdel(&pmet);

	// Only making this so it would be easier to change the values of the size of the shown image.
	image = ft_memalloc(sizeof(t_ui_element));
	ui_menu_new(guimp->win_toolbox, image);
	ui_element_set_parent(image, menu, UI_TYPE_ELEMENT);
	ui_element_edit(image, recipe_image);
	ui_element_set_id(image, "layer_image_elem");
	image->show = 0;

	show = ft_memalloc(sizeof(t_ui_element));
	ui_checkbox_new(guimp->win_toolbox, show);
	ui_element_set_parent(show, menu, UI_TYPE_ELEMENT);
	ui_element_edit(show, recipe_show);
	ui_element_set_id(show, "layer_show_checkbox");

	select = ft_memalloc(sizeof(t_ui_element));
	ui_button_new(guimp->win_toolbox, select);
	ui_element_set_parent(select, menu, UI_TYPE_ELEMENT);
	ui_element_edit(select, recipe_select);
	ui_element_set_id(select, "layer_select_button");

	ui_label_set_text(&((t_ui_button *)select->element)->label, layer_name);

	show->is_click = 1;

	return (menu);
}

float	get_ratio(t_vec2i orig_wh, t_vec2i new_wh)
{
	float	ratio_x;
	float	ratio_y;

	ratio_x = new_wh.x / (float)orig_wh.x;
	ratio_y = new_wh.y / (float)orig_wh.y;
	if (ratio_x < ratio_y)
		return (ratio_x);
	return (ratio_y);
}

void	layer_elements_render(t_guimp *guimp)
{
	int			jj;
	SDL_Surface	*tt;
	t_vec4		pos;

	pos = vec4(30, 5, 72, 20);
	if (guimp->layer_amount > 0)
		pos = ui_list_get_element_by_id(guimp->layer_elems[0]->children, "layer_image_elem")->pos;
	float	ratio = get_ratio(vec2i(guimp->final_image.pos.w, guimp->final_image.pos.h), vec2i(pos.w, pos.h));
	int		final_w = guimp->final_image.pos.w * ratio;
	int		final_h = guimp->final_image.pos.h * ratio;
	jj = -1;
	while (++jj < guimp->layer_amount)
	{
		if (!guimp->layer_elems[jj]->texture)
			continue ;
		tt = ui_surface_new(guimp->layer_elems[jj]->pos.w, guimp->layer_elems[jj]->pos.h);
		SDL_FillRect(tt, NULL, 0xff037171);
		SDL_BlitScaled(guimp->layers[jj].surface, &(SDL_Rect){-guimp->layers[jj].pos.x, -guimp->layers[jj].pos.y, guimp->final_image.pos.w, guimp->final_image.pos.h}, tt, &(SDL_Rect){pos.x + (pos.w / 2) - (final_w / 2), pos.y + (pos.h / 2) - (final_h / 2), final_w, final_h});
		SDL_UpdateTexture(guimp->layer_elems[jj]->texture, NULL, tt->pixels, tt->pitch);
		SDL_FreeSurface(tt);
	}
	SDL_RenderPresent(guimp->win_toolbox->renderer);
	SDL_SetRenderTarget(guimp->win_toolbox->renderer, NULL);
}

void	new_layer_combination(t_guimp *guimp)
{
	t_ui_element	*layer_menu;
	t_ui_element	*layer_button;

	if (guimp->layer_amount >= MAX_LAYER_AMOUNT)
	{
		ft_printf("[%s] No new layer added, layer cap reached (%d).\n", __FUNCTION__, MAX_LAYER_AMOUNT);
		return ;
	}
	// Making new layer element
	layer_menu = new_layer_element(guimp, guimp->new_layer_name_input_label->text, guimp->layer_amount);
	guimp->layer_elems[guimp->layer_amount] = layer_menu;
	// adding layer to the radio buttons;
	layer_button = ui_list_get_element_by_id(layer_menu->children, "layer_select_button");
	add_to_list(&guimp->radio_layer.children, layer_button, UI_TYPE_ELEMENT);

	// Making new actual layer
	layer_new(&guimp->layers[guimp->layer_amount],
		guimp->new_layer_name_input_label->text,
		vec4i(0, 0, atoi(guimp->new_layer_width_input_label->text), atoi(guimp->new_layer_height_input_label->text)),
		&ui_list_get_element_by_id(layer_menu->children, "layer_show_checkbox")->is_click);
	
	// Make the new layer the selected layer
	guimp->selected_layer = guimp->layer_amount;
	ui_radio_button_toggle_on(&guimp->radio_layer, layer_button);
	
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
		}
	}
}

/*
 * removes and reorders layer_elems;
*/
void	remove_nth_layer(t_guimp *guimp, int nth)
{
	t_ui_element	*button;
	int				i;

	if (nth < 0 || nth >= guimp->layer_amount)
		return ;
	layer_free(&guimp->layers[nth]);
	/*
	ui_menu_free(guimp->layer_elems[guimp->selected_layer]);
	*/
	ui_element_remove_child_from_parent(guimp->layer_elems[nth]);
	guimp->layer_elems[nth] = NULL; // this should be removed when freeing it correctly;
	i = nth;
	for (; i < guimp->layer_amount - 1; ++i)
	{
		if (guimp->layer_elems[i] == NULL)
		{
			guimp->layers[i] = guimp->layers[i + 1];
			guimp->layer_elems[i] = guimp->layer_elems[i + 1];
			guimp->layer_elems[i + 1] = NULL;
			ui_element_pos_set2(guimp->layer_elems[i], vec2(guimp->layer_elems[i]->pos.x, (guimp->layer_elems[i]->pos.h * i) + (10 * i) + 40)); // NOTE: this '40' comes from the offset on the menu;
		}
	}
	guimp->layer_amount--;
	guimp->selected_layer = ft_clamp(guimp->selected_layer - 1, 0, guimp->layer_amount);
	if (guimp->layer_amount <= 0)
		return ;
	button = ui_list_get_element_by_id(guimp->layer_elems[guimp->selected_layer]->children, "layer_select_button");
	if (button)
		ui_radio_button_toggle_on(&guimp->radio_layer, button);
}

void	button_remove_layer_event(t_guimp *guimp)
{
	if (ui_button(guimp->button_remove_layer))
		remove_nth_layer(guimp, guimp->selected_layer);
}

void	button_edit_layer_event(t_guimp *guimp)
{
	char	temp[20];

	if (guimp->selected_layer < 0 || guimp->selected_layer >= MAX_LAYER_AMOUNT)
		return ;
	if (ui_button(guimp->button_edit_layer) && guimp->layer_amount > 0)
	{
		ui_window_flag_set(guimp->win_layer_edit, UI_WINDOW_SHOW);
		SDL_RaiseWindow(guimp->win_layer_edit->win);
		guimp->win_layer_edit->textures_recreate = 1; // this is how to fix small bug;
		// fill with the current info of the layer
		ui_label_set_text(&((t_ui_input *)guimp->input_edit_layer_name->element)->label,
			guimp->layers[guimp->selected_layer].name);	
		ui_label_set_text(&((t_ui_input *)guimp->input_edit_layer_width->element)->label,
			ft_b_itoa(guimp->layers[guimp->selected_layer].pos.w, temp));	
		ui_label_set_text(&((t_ui_input *)guimp->input_edit_layer_height->element)->label,
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
			ui_label_set_text(&((t_ui_button *)ui_list_get_element_by_id(guimp->layer_elems[guimp->selected_layer]->children, "layer_select_button")->element)->label, guimp->layers[guimp->selected_layer].name);
			ui_window_flag_set(guimp->win_layer_edit, UI_WINDOW_HIDE);
		}
	}
}

/*
 * The buttons that move the layers up and down.
*/
void	button_move_layer_event(t_guimp *guimp)
{
	if (ui_button(guimp->button_move_layer_up))
	{
		if (guimp->selected_layer <= 0) // already first
			return ;

		t_layer temporar = guimp->layers[guimp->selected_layer];
		guimp->layers[guimp->selected_layer] = guimp->layers[guimp->selected_layer - 1];
		guimp->layers[guimp->selected_layer - 1] = temporar;

		vec4_swap(&guimp->layer_elems[guimp->selected_layer - 1]->pos, &guimp->layer_elems[guimp->selected_layer]->pos);

		t_ui_element *temp = guimp->layer_elems[guimp->selected_layer];
		guimp->layer_elems[guimp->selected_layer] = guimp->layer_elems[guimp->selected_layer - 1];
		guimp->layer_elems[guimp->selected_layer - 1] = temp;
		guimp->selected_layer--;
	}
	else if (ui_button(guimp->button_move_layer_down))
	{
		if (guimp->selected_layer >= guimp->layer_amount - 1) // already last, or youre the only one
			return ;

		t_layer temporar = guimp->layers[guimp->selected_layer];
		guimp->layers[guimp->selected_layer] = guimp->layers[guimp->selected_layer + 1];
		guimp->layers[guimp->selected_layer + 1] = temporar;

		vec4_swap(&guimp->layer_elems[guimp->selected_layer + 1]->pos, &guimp->layer_elems[guimp->selected_layer]->pos);

		t_ui_element *temp = guimp->layer_elems[guimp->selected_layer];
		guimp->layer_elems[guimp->selected_layer] = guimp->layer_elems[guimp->selected_layer + 1];
		guimp->layer_elems[guimp->selected_layer + 1] = temp;
		guimp->selected_layer++;
	}
}

void	color_swatch_event(t_guimp *guimp)
{
	char			*temp;
	t_ui_slider		*red_slider;
	t_ui_slider		*green_slider;
	t_ui_slider		*blue_slider;
	t_ui_slider		*alpha_slider;
	t_ui_element	*input_label;

	red_slider = guimp->red_slider->element;
	green_slider = guimp->green_slider->element;
	blue_slider = guimp->blue_slider->element;
	alpha_slider = guimp->alpha_slider->element;
	input_label = ui_input_get_label_element(guimp->color_swatch);
	if (red_slider->update || green_slider->update || blue_slider->update || alpha_slider->update)
	{
		guimp->combined_color = rgba_to_hex(rgba(red_slider->value, green_slider->value, blue_slider->value, alpha_slider->value));
		ui_element_color_set(guimp->color_swatch, UI_STATE_DEFAULT, guimp->combined_color);
		if (!guimp->color_swatch->is_click)
		{
			temp = ft_itoa_base(guimp->combined_color, 16);
			ui_label_set_text(input_label, temp);
			ft_strdel(&temp);
		}
	}
	else if (guimp->color_swatch->is_click) // from input label set to sliders.
	{
		t_ui_label	*label;
		t_rgba		input_rgba;

		label = input_label->element;
		guimp->combined_color = (unsigned int)strtoul(label->text, NULL, 16);
		input_rgba = hex_to_rgba(guimp->combined_color);
		ui_slider_value_set(guimp->red_slider, input_rgba.r);
		ui_slider_value_set(guimp->green_slider, input_rgba.g);
		ui_slider_value_set(guimp->blue_slider, input_rgba.b);
		ui_slider_value_set(guimp->alpha_slider, input_rgba.a);
		ui_element_color_set(guimp->color_swatch, UI_STATE_DEFAULT, guimp->combined_color);
	}
	if (((t_ui_slider *)guimp->size_slider->element)->update)
		guimp->size = ((t_ui_slider *)guimp->size_slider->element)->value;
}

void	save_button_event(t_guimp *guimp)
{
	if (ui_button(guimp->save_button))
	{
		ui_window_flag_set(guimp->win_save_image, UI_WINDOW_SHOW);
		SDL_RaiseWindow(guimp->win_save_image->win);
	}
	if (ui_button(guimp->button_save_image_ok))
	{
		save_surface(guimp->final_image.surface, ui_input_get_text(guimp->input_save_image_name));
		ui_window_flag_set(guimp->win_save_image, UI_WINDOW_HIDE);
	}
}

void	edit_button_event(t_guimp *guimp)
{
	int	w;
	int	h;

	if (ui_button(guimp->edit_button))
	{
		ui_window_flag_set(guimp->win_image_edit, UI_WINDOW_SHOW);
		SDL_RaiseWindow(guimp->win_image_edit->win);
	}
	if (ui_button(guimp->new_image_ok_button))
	{
		w = atoi(guimp->new_image_width_input_label->text);
		h = atoi(guimp->new_image_height_input_label->text);
		resize_layer(&guimp->final_image, vec2i(w, h));
		SDL_DestroyTexture(guimp->final_image_texture);
		guimp->final_image_texture = SDL_CreateTextureFromSurface(guimp->win_main->renderer, guimp->final_image.surface);
		ui_window_flag_set(guimp->win_image_edit, UI_WINDOW_HIDE);
	}
}

void	clear_button_event(t_guimp *guimp)
{
	int	i;
	int	amount;

	// remove all layers.
	if (ui_button(guimp->clear_button))
	{
		i = -1;
		amount = guimp->layer_amount;
		ft_printf("[%s] Starting to remove %d layers.\n", __FUNCTION__, amount);
		while (++i < guimp->layer_amount)
			remove_nth_layer(guimp, 0);
		ft_printf("[%s] %d layers removed.\n", __FUNCTION__, i);
	}
}

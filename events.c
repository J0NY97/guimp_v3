#include "guimp.h"

void	new_element_from_recipe_with_parent(
	t_ui_element *elem, int elem_type, char *recipe_id, t_ui_element *parent)
{
	t_ui_recipe		*recipe;

	recipe = ui_layout_get_recipe(parent->win->layout, recipe_id);
	g_acceptable[elem_type].maker(parent->win, elem);
	ui_element_set_parent(elem, parent, UI_TYPE_ELEMENT);
	ui_element_edit(elem, recipe);
	ui_element_set_id(elem, recipe_id);
}

t_ui_element	*new_layer_element(
	t_guimp *guimp, char *layer_name, int nth_layer)
{
	t_ui_element	*menu;
	t_ui_element	*show;
	t_ui_element	*image;
	t_ui_element	*select;
	t_ui_recipe		*r_menu;

	r_menu = ui_list_get_recipe_by_id(guimp->layout.recipes, "layer");
	menu = ft_memalloc(sizeof(t_ui_element));
	new_element_from_recipe_with_parent(
		menu, UI_TYPE_MENU, "layer", guimp->layer_parent);
	ui_element_pos_set(menu, vec4(r_menu->pos.x, (r_menu->pos.h * nth_layer)
		+ (nth_layer * 10) + r_menu->pos.y, r_menu->pos.w, r_menu->pos.h));
	image = ft_memalloc(sizeof(t_ui_element));
	new_element_from_recipe_with_parent(
		image, UI_TYPE_MENU, "layer_image_elem", menu);
	show = ft_memalloc(sizeof(t_ui_element));
	new_element_from_recipe_with_parent(
		show, UI_TYPE_CHECKBOX, "layer_show_checkbox", menu);
	select = ft_memalloc(sizeof(t_ui_element));
	new_element_from_recipe_with_parent(
		select, UI_TYPE_BUTTON, "layer_select_button", menu);
	ui_label_set_text(&((t_ui_button *)select->element)->label, layer_name);
	ui_checkbox_toggle_on(show);
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
	t_vec2i		final;
	float		ratio;

	pos = vec4(30, 5, 72, 20);
	if (guimp->layer_amount > 0)
		pos = ui_list_get_element_by_id(guimp->layer_elems[0]->children,
				"layer_image_elem")->pos;
	ratio = get_ratio(vec2i(guimp->final_image.pos.w, guimp->final_image.pos.h),
			vec2i(pos.w, pos.h));
	final.x = guimp->final_image.pos.w * ratio;
	final.y = guimp->final_image.pos.h * ratio;
	jj = -1;
	while (++jj < guimp->layer_amount)
	{
		if (!guimp->layer_elems[jj]->texture)
			continue ;
		tt = ui_surface_new(final.x, final.y);
		SDL_BlitScaled(guimp->layers[jj].surface,
			&(SDL_Rect){-guimp->layers[jj].pos.x, -guimp->layers[jj].pos.y,
				guimp->final_image.pos.w, guimp->final_image.pos.h}, tt, NULL);
		SDL_UpdateTexture(guimp->layer_elems[jj]->texture,
			&(SDL_Rect){pos.x + (pos.w / 2) - (final.x / 2),
				pos.y + (pos.h / 2) - (final.y / 2), final.x, final.y},
			tt->pixels, tt->pitch);
		SDL_FreeSurface(tt);
	}
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
		&ui_list_get_element_by_id(layer_menu->children, "layer_show_checkbox")->is_toggle);
	
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
	if (ui_button(guimp->new_layer_ok_button))
	{
		new_layer_combination(guimp);
		ui_window_flag_set(guimp->win_layer_new, UI_WINDOW_HIDE);
	}
}

/*
 * removes and reorders layer_elems;
 *
 * NOTE: the '40', in ...pos_set2(), comes from the offset on the menu;
*/
void	remove_nth_layer(t_guimp *guimp, int nth)
{
	t_ui_element	*button;
	int				i;

	if (nth < 0 || nth >= guimp->layer_amount)
		return ;
	layer_free(&guimp->layers[nth]);
	ui_element_free(guimp->layer_elems[guimp->selected_layer],
		sizeof(t_ui_element));
	guimp->layer_elems[nth] = NULL;
	i = nth;
	for (; i < guimp->layer_amount - 1; ++i)
	{
		if (guimp->layer_elems[i] == NULL)
		{
			guimp->layers[i] = guimp->layers[i + 1];
			guimp->layer_elems[i] = guimp->layer_elems[i + 1];
			guimp->layer_elems[i + 1] = NULL;
			ui_element_pos_set2(guimp->layer_elems[i],
				vec2(guimp->layer_elems[i]->pos.x,
					(guimp->layer_elems[i]->pos.h * i) + (10 * i) + 40));
		}
	}
	guimp->layer_amount--;
	guimp->selected_layer = ft_clamp(guimp->selected_layer - 1, 0, guimp->layer_amount);
	if (guimp->layer_amount <= 0)
		return ;
	button = ui_list_get_element_by_id(
			guimp->layer_elems[guimp->selected_layer]->children,
			"layer_select_button");
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
		ui_label_set_text(
			ui_input_get_label_element(guimp->input_edit_layer_name),
			guimp->layers[guimp->selected_layer].name);	
		ui_label_set_text(
			ui_input_get_label_element(guimp->input_edit_layer_width),
			ft_b_itoa(guimp->layers[guimp->selected_layer].pos.w, temp));	
		ui_label_set_text(
			ui_input_get_label_element(guimp->input_edit_layer_height),
			ft_b_itoa(guimp->layers[guimp->selected_layer].pos.h, temp));	
	}
	if (ui_button(guimp->button_edit_layer_ok))
	{
		resize_layer(&guimp->layers[guimp->selected_layer],
			vec2i(ft_atoi(ui_input_get_text(guimp->input_edit_layer_width)),
				ft_atoi(ui_input_get_text(guimp->input_edit_layer_height))));
		ft_strdel(&guimp->layers[guimp->selected_layer].name);
		guimp->layers[guimp->selected_layer].name
				= ft_strdup(ui_input_get_text(guimp->input_edit_layer_name));
		ui_label_set_text(ui_button_get_label_element(
				ui_list_get_element_by_id(
					guimp->layer_elems[guimp->selected_layer]->children,
					"layer_select_button")),
			guimp->layers[guimp->selected_layer].name);
		ui_window_flag_set(guimp->win_layer_edit, UI_WINDOW_HIDE);
	}
}

/*
 * The buttons that move the layers up and down.
*/
void	button_move_layer_event(t_guimp *guimp)
{
	t_layer			temporar;
	t_ui_element	*temp;

	if (ui_button(guimp->button_move_layer_up))
	{
		if (guimp->selected_layer <= 0)
			return ;
		temporar = guimp->layers[guimp->selected_layer];
		guimp->layers[guimp->selected_layer]
			= guimp->layers[guimp->selected_layer - 1];
		guimp->layers[guimp->selected_layer - 1] = temporar;

		vec4_swap(&guimp->layer_elems[guimp->selected_layer - 1]->pos,
			&guimp->layer_elems[guimp->selected_layer]->pos);

		temp = guimp->layer_elems[guimp->selected_layer];
		guimp->layer_elems[guimp->selected_layer]
			= guimp->layer_elems[guimp->selected_layer - 1];
		guimp->layer_elems[guimp->selected_layer - 1] = temp;
		guimp->selected_layer--;
	}
	else if (ui_button(guimp->button_move_layer_down))
	{
		if (guimp->selected_layer >= guimp->layer_amount - 1)
			return ;

		temporar = guimp->layers[guimp->selected_layer];
		guimp->layers[guimp->selected_layer]
			= guimp->layers[guimp->selected_layer + 1];
		guimp->layers[guimp->selected_layer + 1] = temporar;

		vec4_swap(&guimp->layer_elems[guimp->selected_layer + 1]->pos,
			&guimp->layer_elems[guimp->selected_layer]->pos);

		temp = guimp->layer_elems[guimp->selected_layer];
		guimp->layer_elems[guimp->selected_layer]
			= guimp->layer_elems[guimp->selected_layer + 1];
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
	t_ui_label		*label;
	t_rgba			input_rgba;

	red_slider = guimp->red_slider->element;
	green_slider = guimp->green_slider->element;
	blue_slider = guimp->blue_slider->element;
	alpha_slider = guimp->alpha_slider->element;
	input_label = ui_input_get_label_element(guimp->color_swatch);
	if (red_slider->update || green_slider->update
		|| blue_slider->update || alpha_slider->update)
	{
		guimp->combined_color = rgba_to_hex(
				rgba(red_slider->value, green_slider->value,
					blue_slider->value, alpha_slider->value));
		ui_element_color_set(guimp->color_swatch,
			UI_STATE_DEFAULT, guimp->combined_color);
		if (!guimp->color_swatch->is_click)
		{
			temp = ft_itoa_base(guimp->combined_color, 16);
			ui_label_set_text(input_label, temp);
			ft_strdel(&temp);
		}
	}
	else if (guimp->color_swatch->is_click)
	{
		label = input_label->element;
		guimp->combined_color = (unsigned int)strtoul(label->text, NULL, 16);
		input_rgba = hex_to_rgba(guimp->combined_color);
		ui_slider_value_set(guimp->red_slider, input_rgba.r);
		ui_slider_value_set(guimp->green_slider, input_rgba.g);
		ui_slider_value_set(guimp->blue_slider, input_rgba.b);
		ui_slider_value_set(guimp->alpha_slider, input_rgba.a);
		ui_element_color_set(guimp->color_swatch,
			UI_STATE_DEFAULT, guimp->combined_color);
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
		save_surface(guimp->final_image.surface,
			ui_input_get_text(guimp->input_save_image_name));
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
		guimp->final_image_texture
			= ui_create_texture(guimp->win_main->renderer, vec2i(w, h));
		SDL_UpdateTexture(guimp->final_image_texture, NULL,
			guimp->final_image.surface->pixels,
			guimp->final_image.surface->pitch);
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

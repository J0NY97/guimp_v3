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

void	small_image_drawer(t_guimp *guimp, t_vec4 img_pos, t_vec2i img_size)
{
	int			jj;
	SDL_Surface	*img;

	jj = -1;
	while (++jj < guimp->layer_amount)
	{
		if (!guimp->layer_elems[jj]->texture)
			continue ;
		img = ui_surface_new(img_size.x, img_size.y);
		SDL_BlitScaled(guimp->layers[jj].surface,
			&(SDL_Rect){-guimp->layers[jj].pos.x, -guimp->layers[jj].pos.y,
			guimp->final_image.pos.w, guimp->final_image.pos.h}, img, NULL);
		SDL_UpdateTexture(guimp->layer_elems[jj]->texture,
			&(SDL_Rect){img_pos.x + (img_pos.w / 2) - (img_size.x / 2),
			img_pos.y + (img_pos.h / 2) - (img_size.y / 2),
			img_size.x, img_size.y},
			img->pixels, img->pitch);
		SDL_FreeSurface(img);
	}
}

/*
 * from guimp->layers (t_layer, that have the surface we draw on, specific to
 *	that layer) we take the surface and figure out the ratio so it fits inside
 *	a set dimension (the size of the layer_elems texture), and blit that surface
 *	with that ratio on the 'layer_elems' element->texture;
*/
void	layer_elements_render(t_guimp *guimp)
{
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
	small_image_drawer(guimp, pos, final);
}

/*
 * Create new layer element;
 * Add layer element button to radio buttons list;
 * Create new layer (t_layer);
 * Set the newly created layer (t_layer) the currently selected layer;
*/
void	new_layer_combination(t_guimp *guimp)
{
	t_ui_element	*layer_menu;
	t_ui_element	*layer_button;

	if (guimp->layer_amount >= MAX_LAYER_AMOUNT)
	{
		ft_printf("[%s] No new layer added, layer cap reached (%d).\n",
			__FUNCTION__, MAX_LAYER_AMOUNT);
		return ;
	}
	layer_menu = new_layer_element(guimp,
			guimp->new_layer_name_input_label->text, guimp->layer_amount);
	guimp->layer_elems[guimp->layer_amount] = layer_menu;
	layer_button = ui_list_get_element_by_id(layer_menu->children,
			"layer_select_button");
	add_to_list(&guimp->radio_layer.children, layer_button, UI_TYPE_ELEMENT);
	layer_new(&guimp->layers[guimp->layer_amount],
		guimp->new_layer_name_input_label->text,
		vec4i(0, 0, atoi(guimp->new_layer_width_input_label->text),
			atoi(guimp->new_layer_height_input_label->text)),
		&ui_list_get_element_by_id(layer_menu->children,
			"layer_show_checkbox")->is_toggle);
	guimp->selected_layer = guimp->layer_amount;
	ui_radio_button_toggle_on(&guimp->radio_layer, layer_button);
	guimp->layer_amount += 1;
	ft_printf("[%s] Layer added. (%d)\n", __FUNCTION__, guimp->layer_amount);
}

void	rename_layer(t_guimp *guimp, int layer_index, char *name)
{
	ft_strdel(&guimp->layers[layer_index].name);
	guimp->layers[layer_index].name = ft_strdup(name);
	ui_label_set_text(ui_button_get_label_element(ui_list_get_element_by_id(
				guimp->layer_elems[layer_index]->children,
				"layer_select_button")), name);
}

void	button_add_layer_event(t_guimp *guimp)
{
	if (ui_button(guimp->button_add_layer))
	{
		ui_window_flag_set(guimp->win_layer_new, UI_WINDOW_SHOW);
		SDL_RaiseWindow(guimp->win_layer_new->win);
	}
	if (ui_button(guimp->new_layer_ok_button))
	{
		new_layer_combination(guimp);
		ui_window_flag_set(guimp->win_layer_new, UI_WINDOW_HIDE);
	}
}

void	reorder_layers(t_guimp *guimp)
{
	int	i;

	i = -1;
	while (++i < guimp->layer_amount)
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
}

/*
 * removes and reorders layer_elems;
 *
 * NOTE: the '40', in ...pos_set2(), comes from the offset on the menu;
*/
void	remove_nth_layer(t_guimp *guimp, int nth)
{
	t_ui_element	*button;

	if (nth < 0 || nth >= guimp->layer_amount)
		return ;
	layer_free(&guimp->layers[nth]);
	ui_element_free(guimp->layer_elems[guimp->selected_layer],
		sizeof(t_ui_element));
	guimp->layer_elems[nth] = NULL;
	guimp->layer_amount--;
	reorder_layers(guimp);
	guimp->selected_layer
		= ft_clamp(guimp->selected_layer - 1, 0, guimp->layer_amount);
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

void	edit_layer_event(t_guimp *guimp)
{
	char	temp[20];

	ui_window_flag_set(guimp->win_layer_edit, UI_WINDOW_SHOW);
	SDL_RaiseWindow(guimp->win_layer_edit->win);
	ui_label_set_text(ui_input_get_label_element(guimp->input_edit_layer_name),
		guimp->layers[guimp->selected_layer].name);
	ui_label_set_text(ui_input_get_label_element(guimp->input_edit_layer_width),
		ft_b_itoa(guimp->layers[guimp->selected_layer].pos.w, temp));
	ui_label_set_text(
		ui_input_get_label_element(guimp->input_edit_layer_height),
		ft_b_itoa(guimp->layers[guimp->selected_layer].pos.h, temp));
}

void	apply_layer_edit_event(t_guimp *guimp)
{
	resize_layer(&guimp->layers[guimp->selected_layer],
		vec2i(ft_atoi(ui_input_get_text(guimp->input_edit_layer_width)),
			ft_atoi(ui_input_get_text(guimp->input_edit_layer_height))));
	rename_layer(guimp, guimp->selected_layer,
		ui_input_get_text(guimp->input_edit_layer_name));
	ui_window_flag_set(guimp->win_layer_edit, UI_WINDOW_HIDE);
}

void	button_edit_layer_event(t_guimp *guimp)
{
	if (guimp->new_layer_selected && guimp->win_layer_edit->show)
		edit_layer_event(guimp);
	if (guimp->selected_layer < 0 || guimp->selected_layer >= MAX_LAYER_AMOUNT)
		return ;
	if (ui_button(guimp->button_edit_layer) && guimp->layer_amount > 0)
		edit_layer_event(guimp);
	if (ui_button(guimp->button_edit_layer_ok))
		apply_layer_edit_event(guimp);
}

/*
 * Not moving the layer on the main_window,
 * 	but moving the layer elements, on the win_toolbox, up and down;
*/
void	move_layer_up_event(t_guimp *guimp)
{
	t_ui_element	*temp;
	t_layer			temporar;

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

void	move_layer_down_event(t_guimp *guimp)
{
	t_ui_element	*temp;
	t_layer			temporar;

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

/*
 * The buttons that move the layers up and down.
*/
void	button_move_layer_event(t_guimp *guimp)
{
	if (ui_button(guimp->button_move_layer_up))
		move_layer_up_event(guimp);
	else if (ui_button(guimp->button_move_layer_down))
		move_layer_down_event(guimp);
}

void	color_slider_events(t_guimp *guimp)
{
	char	*temp;

	guimp->combined_color = rgba_to_hex(rgba(
				ui_slider_value_get(guimp->red_slider),
				ui_slider_value_get(guimp->green_slider),
				ui_slider_value_get(guimp->blue_slider),
				ui_slider_value_get(guimp->alpha_slider)));
	ui_element_color_set(guimp->color_swatch,
		UI_STATE_DEFAULT, guimp->combined_color);
	if (!guimp->color_swatch->is_click)
	{
		temp = ft_itoa_base(guimp->combined_color, 16);
		ui_input_set_text(guimp->color_swatch, temp);
		ft_strdel(&temp);
	}
}

void	color_swatch_event(t_guimp *guimp)
{
	t_ui_label		*label;
	t_rgba			input_rgba;

	if (ui_slider_updated(guimp->red_slider)
		|| ui_slider_updated(guimp->green_slider)
		|| ui_slider_updated(guimp->blue_slider)
		|| ui_slider_updated(guimp->alpha_slider))
		color_slider_events(guimp);
	else if (guimp->color_swatch->is_click)
	{
		label = ui_input_get_label(guimp->color_swatch);
		guimp->combined_color = (unsigned int)strtoul(label->text, NULL, 16);
		input_rgba = hex_to_rgba(guimp->combined_color);
		ui_slider_value_set(guimp->red_slider, input_rgba.r);
		ui_slider_value_set(guimp->green_slider, input_rgba.g);
		ui_slider_value_set(guimp->blue_slider, input_rgba.b);
		ui_slider_value_set(guimp->alpha_slider, input_rgba.a);
		ui_element_color_set(guimp->color_swatch,
			UI_STATE_DEFAULT, guimp->combined_color);
	}
	if (ui_slider_updated(guimp->size_slider))
		guimp->size = ui_slider_value_get(guimp->size_slider);
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

/*
 * NOTE:
 * 	after resizing the image, we have to put the last_state of the layer_elems
 * 	to something else so that the texture of them will be reset;
*/
void	edit_button_event(t_guimp *guimp)
{
	int	w;
	int	h;
	int	iii;

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
		iii = -1;
		while (++iii < guimp->layer_amount)
			guimp->layer_elems[iii]->last_state = -13;
	}
}

/*
 * Removing all the layers;
*/
void	clear_button_event(t_guimp *guimp)
{
	int	i;
	int	amount;

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
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   layer_element.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nneronin <nneronin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/29 13:54:32 by jsalmi            #+#    #+#             */
/*   Updated: 2022/05/30 13:50:58 by nneronin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "guimp.h"

t_ui_element	*new_layer_element(t_guimp *guimp, char *layer_name,
	int nth_layer)
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

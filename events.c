#include "guimp.h"

/*
void	layer_element(t_guimp *guimp)
{
	t_ui_element	*menu; // menu
	t_ui_element	*show; // checkbox
	t_ui_element	*select; // radio button
	t_ui_element	*layer; // button (probably doesnt even need to be a button, but just an element)
	t_ui_recipe		*recipe_menu;
	t_ui_recipe		*recipe_show;
	t_ui_recipe		*recipe_select;
	t_ui_recipe		*recipe_layer;

	recipe_menu = ui_layout_get_recipe_by_id(&guimp->layout, "layer");
	recipe_show = ui_layout_get_recipe_by_id(&guimp->layout, "layer_show_checkbox");
	recipe_select = ui_layout_get_recipe_by_id(&guimp->layout, "layer_select_button");
	recipe_layer = ui_layout_get_recipe_by_id(&guimp->layout, "layer_layer");

	menu = ui_element_create_from_recipe(recipe_menu);
}
*/

void	new_layer(t_guimp *guimp)
{
	t_ui_element	*elem;
	char			*new_id;
	char			temp[20];

	if (guimp->layer_count >= 5)
	{
		ft_printf("[%s] No new layer added, layer cap reached (5).\n", __FUNCTION__);
		return ;
	}
	// Making new actual layer
	t_layer	*layer;

	layer = &guimp->layers[guimp->layer_count];	
	layer->name = ft_strdup(guimp->new_layer_name_input_label->text);
	layer->pos.x = 0;
	layer->pos.y = 0;
	layer->pos.w = atoi(guimp->new_layer_width_input_label->text);
	layer->pos.h = atoi(guimp->new_layer_height_input_label->text);

	ft_printf("New Layer Info : %s, %d %d.\n", layer->name, layer->pos.w, layer->pos.h);
	
	// Making new layer element
	ui_layout_element_new(&guimp->layout, guimp->win_toolbox, guimp->layer_recipe);
	elem = guimp->layout.elements->content;
	new_id = ft_strjoin(elem->id, ft_b_itoa(guimp->layer_count, temp));
	ui_element_id_set(elem, new_id);
	ui_element_pos_set2(elem, vec2(elem->pos.x, (guimp->layer_count * elem->pos.h) + (guimp->layer_count * 10) + elem->pos.y));
	ui_element_parent_set(elem, guimp->layer_parent, UI_TYPE_ELEMENT);
	ui_element_print(elem);
	add_to_list(&guimp->layer_elems, elem, UI_TYPE_ELEMENT);

	guimp->layer_count += 1;
	ft_printf("[%s] New layer added. (%d)\n", __FUNCTION__, guimp->layer_count);
}

void	layer_plus_button_event(t_guimp *guimp)
{
	if (ui_button(guimp->layer_plus_button))
		ui_window_flag_set(guimp->win_layer_edit, UI_WINDOW_SHOW);
}

void	new_layer_ok_button_event(t_guimp *guimp)
{
	if (ui_button(guimp->new_layer_ok_button))
	{
		new_layer(guimp);
		ui_window_flag_set(guimp->win_layer_edit, UI_WINDOW_HIDE);
		ui_element_print(guimp->new_layer_ok_button);
	}
}

void	color_swatch_event(t_guimp *guimp)
{
	Uint32	combined_slider_color;
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
	combined_slider_color = 0;
	if (red_slider->update || green_slider->update || blue_slider->update || alpha_slider->update)
	{
		combined_slider_color = rgba_to_hex((t_rgba){.r = red_slider->value, .g = green_slider->value, .b = blue_slider->value, .a = alpha_slider->value});
		ui_element_color_set(guimp->color_swatch, UI_STATE_DEFAULT, combined_slider_color);
		if (!guimp->color_swatch->is_click)
		{
			itoa(combined_slider_color, temp, 16);
			ui_label_text_set(&input_label, temp);
		}
	}
	else if (guimp->color_swatch->is_click) // from input label set to sliders.
	{
		t_ui_label	*label;
		t_rgba		input_rgba;

		label = input_label.element;
		combined_slider_color = (unsigned int)strtoul(label->text, NULL, 16);
		input_rgba = hex_to_rgba(combined_slider_color);
		ui_slider_value_set(guimp->red_slider, input_rgba.r);
		ui_slider_value_set(guimp->green_slider, input_rgba.g);
		ui_slider_value_set(guimp->blue_slider, input_rgba.b);
		ui_slider_value_set(guimp->alpha_slider, input_rgba.a);
	}
}

#include "guimp.h"

void	new_layer(t_guimp *guimp)
{
	t_ui_element	*elem;
	char			*new_id;
	char			temp[20];

	if (0)//guimp->layer_count >= 5)
	{
		ft_printf("[%s] No new layer added, layer cap reached (5).\n", __FUNCTION__);
		return ;
	}
	/*
	elem = ui_element_create_from_recipe(guimp->win_toolbox, guimp->layer_recipe, &guimp->layout);
	ui_element_pos_set2(elem, vec2(elem->pos.x, guimp->layer_count * elem->pos.h + elem->pos.y));
	new_id = ft_strjoin(elem->id, ft_b_itoa(guimp->layer_count, temp));
	ui_element_id_set(elem, new_id);
	ui_menu_add(guimp->layer_parent, elem, UI_TYPE_ELEMENT);
	*/
	ui_layout_element_new(&guimp->layout, guimp->win_toolbox, guimp->layer_recipe);
	elem = guimp->layout.elements->content;
	ui_element_pos_set2(elem, vec2(elem->pos.x, (guimp->layer_count * elem->pos.h) + (guimp->layer_count * 10) + elem->pos.y));
	ui_element_parent_set(elem, guimp->layer_parent, UI_TYPE_ELEMENT);
	ui_element_print(elem);
	add_to_list(&guimp->layers, elem, UI_TYPE_ELEMENT);
	guimp->layer_count += 1;
	ft_printf("[%s] New layer added. (%d)\n", __FUNCTION__, guimp->layer_count);
}

void	new_layer_button_event(t_guimp *guimp)
{
	if (ui_button(guimp->new_layer_button))
		new_layer(guimp);
}

void	color_swatch_event(t_guimp *guimp)
{
	(void)guimp;	
}

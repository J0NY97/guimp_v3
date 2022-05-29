#include "guimp.h"

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
 * 
 * 'dir' : either +1 or -1; (up / down);
*/
void	move_layer_in_dir(t_guimp *guimp, int dir)
{
	t_ui_element	*temp;
	t_layer			temporar;

	if (dir == 1 && guimp->selected_layer >= guimp->layer_amount - 1)
		return ;
	else if (dir == -1 && guimp->selected_layer <= 0)
		return ;
	temporar = guimp->layers[guimp->selected_layer];
	guimp->layers[guimp->selected_layer]
		= guimp->layers[guimp->selected_layer + dir];
	guimp->layers[guimp->selected_layer + dir] = temporar;
	vec4_swap(&guimp->layer_elems[guimp->selected_layer + dir]->pos,
		&guimp->layer_elems[guimp->selected_layer]->pos);
	temp = guimp->layer_elems[guimp->selected_layer];
	guimp->layer_elems[guimp->selected_layer]
		= guimp->layer_elems[guimp->selected_layer + dir];
	guimp->layer_elems[guimp->selected_layer + dir] = temp;
	guimp->selected_layer += dir;
}

/*
 * The buttons that move the layers up and down.
*/
void	button_move_layer_event(t_guimp *guimp)
{
	if (ui_button(guimp->button_move_layer_up))
		move_layer_in_dir(guimp, -1);
	else if (ui_button(guimp->button_move_layer_down))
		move_layer_in_dir(guimp, 1);
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

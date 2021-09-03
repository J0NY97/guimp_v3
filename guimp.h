#ifndef GUIMP_H
# define GUIMP_H
# include "libft.h"
# include "libpf.h"
# include "libui.h"
# include "SDL.h"
# include "SDL_ttf.h"
# include "SDL_image.h"

/*
 * Layer struct, not to confuse with the layer elements in the ui.
 * These are the actual layers you draw on.
 * BTW also used in the image.
*/
typedef struct s_layer
{
	t_vec4i			pos;
	char			*name;
	SDL_Texture		*texture;
}					t_layer;

/*
 * t_ui_window		*win_main;			main window where all the layers are and you draw on;
 * t_layer			layers[5];			array of layers, we are capping this to 5 for now. included image at offset 0;
 *
 * t_ui_layout		layout;				the layout...;
 * t_ui_window		*win_toolbox;		window of the toolbox;
 * t_ui_recipe		*layer_recipe;		recipe for a new layer so that it will be easier to make a new one;
 * t_list			*layers;			list of layers, t_ui_element, created from the "layer_recipe";
 * t_ui_element		*layer_parent;		the parent elem you place all the layers on;
 * t_ui_element		*new_layer_button;	pretty self-explanatory, event : new_layer_button_event();
 * int				layer_count;		amount of layers currently;
 *
 * Color shower:
 * t_ui_element		*color_swatch;		final color of the color sliders;	
 * t_ui_element		*red_slider;		<-
 * t_ui_element		*green_slider;		<-
 * t_ui_element		*blue_slider;		<-
 * t_ui_element		*alpha_slider;		not niklas atleast;
*/
typedef struct s_guimp
{
	////////////////
	// Main Win
	////////////////
	t_ui_window		*win_main;
	t_layer			layers[5];

	////////////////
	// Toolbox Win
	////////////////
	t_ui_layout		layout;
	t_ui_window		*win_toolbox;
	t_ui_recipe		*layer_recipe;
	t_list			*layer_elems;
	t_ui_element	*layer_parent;
	t_ui_element	*layer_plus_button;
	int				layer_count;
	t_ui_element	*color_swatch;
	t_ui_element	*red_slider;
	t_ui_element	*green_slider;
	t_ui_element	*blue_slider;
	t_ui_element	*alpha_slider;

	////////////////
	// New Layer Win
	/////////////////
	t_ui_layout		layout_layer_edit;
	t_ui_window		*win_layer_edit;
	t_ui_element	*new_layer_ok_button;
	t_ui_label		*new_layer_name_input_label;
	t_ui_label		*new_layer_width_input_label;
	t_ui_label		*new_layer_height_input_label;
}					t_guimp;

// Events
void				color_swatch_event(t_guimp *guimp);
void				layer_plus_button_event(t_guimp *guimp);
void				new_layer_ok_button_event(t_guimp *guimp);

#endif

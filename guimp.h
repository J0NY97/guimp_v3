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
/*
 *
*/
typedef struct s_layer
{
	t_vec4i			pos;
	char			*name;
	SDL_Surface		*surface;
}					t_layer;


/*
 * t_ui_window		*win_main;			main window where all the layers are and you draw on;
 * SDL_Texture		*final_image_texture;	the texture of the final image, which will be rendered on the main window;
 * t_layer			final_image;		this is the image where all the layers are blitted to and then showed on screen;
 * t_layer			*selected_layer;	pointer to the currenctly selected layer, this is the one we will draw on;
 * t_layer			layers[5];			array of layers, we are capping this to 5 for now. included image at offset 0;
 * t_vec2i			mouse_pos_prev;		the last updated mouse pos for win_main;
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
	SDL_Texture		*final_image_texture;
	t_layer			final_image;
	int				selected_layer;
	t_layer			layers[5];
	int				layer_amount;
	t_vec2i			mouse_pos_prev;
	int				combined_color;
	float			zoom;

	t_ui_element	radio_layer;
	t_list			*radio_buttons; // this is the same list as the radio_layer.element->buttons
	////////////////
	// Toolbox Win
	////////////////
	t_ui_layout		layout;
	t_ui_window		*win_toolbox;
	t_ui_recipe		*layer_recipe;
	t_ui_element	*layer_elems[5];
	t_ui_element	*layer_parent;
	t_ui_element	*layer_plus_button;
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

// Layer
void				layer_elements_render(t_guimp *guimp);
void				layer_elements_event(t_guimp *guimp, SDL_Event e);
void				new_layer(t_layer *layer, char *name, t_vec4i pos);
void				layer_event(t_guimp *guimp);
void				layer_draw(t_guimp *guimp);
void				layer_render(t_guimp *guimp);

#endif

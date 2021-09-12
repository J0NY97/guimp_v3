#ifndef GUIMP_H
# define GUIMP_H
# include "libft.h"
# include "libpf.h"
# include "libui.h"
# include "SDL.h"
# include "SDL_ttf.h"
# include "SDL_image.h"
# include "dirent.h"

/*
 * char		*dir;			the path to the dir;
 * char		**files;		char * of all the file names in the directory;
 * int		file_amount;	self-explanatory;
*/
typedef struct s_dir_content
{
	char			*dir;
	char			**files;
	int				file_amount;
}					t_dir_content;

/*
 * Layer struct, not to confuse with the layer elements in the ui.
 * These are the actual layers you draw on.
 * BTW also used in the image.
*/
/*
 * bool		*show;		pointer to the layer_show_checkbox->is_click
 * int		id;			how manyeth in the order;
*/
typedef struct s_layer
{
	t_vec4i			pos;
	char			*name;
	int				id;
	SDL_Surface		*surface;
	bool			*show;
}					t_layer;


/*
 * t_ui_window		*win_main;			main window where all the layers are and you draw on;
 * SDL_Texture		*final_image_texture;	the texture of the final image, which will be rendered on the main window;
 * t_layer			final_image;		this is the image where all the layers are blitted to and then showed on screen;
 * t_layer			*selected_layer;	pointer to the currenctly selected layer, this is the one we will draw on;
 * t_layer			layers[5];			array of layers, we are capping this to 5 for now. included image at offset 0;
 * SDL_Surface		*hidden_surface;	the surface where all the tool preshowers will be drawn on;
 * SDL_Texture		*hidden_texture;	the texture where all the tool preshowers will be drawn on;
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
 *
 * t_ui_element		*text_input;		text input element;
 * char				*text_input_str;	the text of text_input label;
 *
 * Tools:
 * t_vec2i			first_pos;				screen position of first mouse click;
 * t_vec2i			first_pos_converted;	position on the actual layer position;
*/
#define MAX_LAYER_AMOUNT 5
typedef struct s_guimp
{
	////////////////
	// Main Win
	////////////////
	t_ui_window		*win_main;
	SDL_Texture		*final_image_texture;
	t_layer			final_image;
	int				selected_layer;
	t_layer			layers[MAX_LAYER_AMOUNT];
	SDL_Surface		*hidden_surface;
	SDL_Texture		*hidden_texture;
	int				layer_amount;
	int				combined_color;
	float			zoom;
	int				size;

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
	// Layer Buttons
	t_ui_element	*button_add_layer;
	t_ui_element	*button_remove_layer;
	t_ui_element	*button_edit_layer;
	t_ui_element	*button_move_layer_up;
	t_ui_element	*button_move_layer_down;
	// Color
	t_ui_element	*color_swatch;
	t_ui_element	*red_slider;
	t_ui_element	*green_slider;
	t_ui_element	*blue_slider;
	t_ui_element	*alpha_slider;
	t_ui_element	*size_slider;
	// Mode Buttons
	t_ui_element	*draw_button;
	t_ui_element	*text_button;
	t_ui_element	*erase_button;
	t_ui_element	*flood_button;
	t_ui_element	*sticker_button;
	t_ui_element	*move_button;
	t_ui_element	*shape_button;
	t_ui_element	*pipette_button;
	t_list			*mode_button_list; // list of the buttons;
	t_ui_element	radio_mode_buttons; // radio of list of the drawing mode buttons;
	// shape tools
	t_vec2i			first_pos;
	t_vec2i			first_pos_converted;
	bool			first_set;
	t_ui_element	*circle_button;
	t_ui_element	*square_button;
	t_ui_element	*line_button;
	t_list			*shape_button_list; // list of the buttons;
	t_ui_element	radio_shape_buttons; // radio of list of the shape buttons;
	// text input
	t_ui_element	*text_input;
	char			*text_input_str;
	// drops
	t_ui_element	*font_dropdown;
	t_ui_element	*sticker_dropdown;
	t_ui_element	font_radio;
	t_ui_element	sticker_radio;
	// buttons
	t_ui_element	*save_button;
	t_ui_element	*edit_button;

	////////////////
	// New Layer Win
	/////////////////
	t_ui_window		*win_layer_new;
	t_ui_element	*new_layer_ok_button;
	t_ui_label		*new_layer_name_input_label;
	t_ui_label		*new_layer_width_input_label;
	t_ui_label		*new_layer_height_input_label;

	////////////////
	// Edit Layer Win
	/////////////////
	t_ui_window		*win_layer_edit;
	t_ui_element	*button_edit_layer_ok;
	t_ui_element	*input_edit_layer_name;
	t_ui_element	*input_edit_layer_width;
	t_ui_element	*input_edit_layer_height;

	////////////////
	// New Image Win
	/////////////////
	t_ui_window		*win_image_edit;
	t_ui_element	*new_image_ok_button;
	t_ui_label		*new_image_width_input_label;
	t_ui_label		*new_image_height_input_label;

	////////////////
	// Save Image Win
	/////////////////
	t_ui_window		*win_save_image;
	t_ui_element	*input_save_image_name;
	t_ui_element	*button_save_image_ok;
}					t_guimp;

// Events
void				color_swatch_event(t_guimp *guimp);
void				button_add_layer_event(t_guimp *guimp);
void				button_remove_layer_event(t_guimp *guimp);
void				button_edit_layer_event(t_guimp *guimp);
void				button_move_layer_event(t_guimp *guimp);
void				new_layer_ok_button_event(t_guimp *guimp);
void				save_button_event(t_guimp *guimp);
void				edit_button_event(t_guimp *guimp);

// Layer
void				new_layer_combination(t_guimp *guimp);
void				layer_elements_render(t_guimp *guimp);
void				layer_elements_event(t_guimp *guimp, SDL_Event e);
void				layer_new(t_layer *layer, char *name, t_vec4i pos, bool *show);
void				layer_free(t_layer *layer);
void				resize_layer(t_layer *layer, t_vec2i wh);
void				layer_event(t_guimp *guimp, SDL_Event e);
void				layer_draw(t_guimp *guimp);
void				layer_render(t_guimp *guimp);

// Help
void				set_sliders_to_color(t_guimp *guimp, Uint32 color);
void				add_to_drop_menu(t_ui_element *dropdown_elem, t_ui_element *child);
void				get_dir_content(t_dir_content *content, char *path);
void				free_dir_content(t_dir_content *content);
void				save_surface(SDL_Surface *surface, char *file);

#endif

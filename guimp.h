/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   guimp.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nneronin <nneronin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 13:43:29 by nneronin          #+#    #+#             */
/*   Updated: 2022/05/30 14:31:08 by nneronin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GUIMP_H
# define GUIMP_H
# include "libft.h"
# include "libpf.h"
# include "libui.h"
# include "liblg.h"
# include "SDL.h"
# include "SDL_ttf.h"
# include "SDL_image.h"
# include "dirent.h"

# define FF_STACK_SIZE 25000

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
 * t_ui_window		*win_main;			main window where all the layers are
 										and you draw on;
 * SDL_Texture		*final_image_texture;	the texture of the final image,
 									which will be rendered on the main window;
 * t_layer			final_image;		this is the image where all the layers
 									are blitted to and then showed on screen;
 * t_layer			*active_layer;		pointer to the currenctly selected
 										layer, this is the one we will draw on;
 * int				selected_layer;		the index of the selected layer;
 * int				new_layer_selected; if new layer was selected this frame;
 * t_layer			layers[5];			array of layers, we are capping this to
 * 										5 for now. included image at offset 0;
 * SDL_Surface		*hidden_surface;	the surface where all the tool
 * 											preshowers will be drawn on;
 * SDL_Texture		*hidden_texture;	the texture where all the tool
 * 											preshowers will be drawn on;
 * t_vec2i			real_image_pos;		position on image where mouse is,
 * 									taking into consideration position and zoom;
 * t_vec2i			hidden_pos;			position on hidden layer where mouse is,
 * 									taking into consideration position and zoom;
 * t_vec2i			actual_pos;			position on selected layer where mouse
 * 								is, taking into consideration position and zoom;
 *
 * t_ui_layout		layout;				the layout...;
 * t_ui_window		*win_toolbox;		window of the toolbox;
 * t_ui_recipe		*layer_recipe;		recipe for a new layer so that it will
 * 											be easier to make a new one;
 * t_list			*layers;			list of layers, t_ui_element, created
 * 											from the "layer_recipe";
 * t_ui_element		*layer_parent;		parent elem you place all the layers on;
 * t_ui_element		*new_layer_button;	pretty self-explanatory,
 * 											event : new_layer_button_event();
 * int				layer_count;		amount of layers currently;
 *
 * t_list			*radio_buttons;		pointer to the layout element
 * 										'radio_buttons'->children; dont free!
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
 * t_vec2i			first_pos;			 screen position of first mouse click;
 * t_vec2i			first_pos_converted; position on the actual layer position;
*/
# define MAX_LAYER_AMOUNT 5

typedef struct s_guimp
{
	t_ui_window		*win_main;
	SDL_Texture		*final_image_texture;
	t_layer			final_image;
	int				selected_layer;
	int				new_layer_selected;
	t_layer			*active_layer;
	t_layer			layers[MAX_LAYER_AMOUNT];
	SDL_Surface		*hidden_surface;
	SDL_Texture		*hidden_texture;
	int				layer_amount;
	Uint32			combined_color;
	float			zoom;
	int				size;
	t_vec2i			real_image_pos;
	t_vec2i			hidden_pos;
	t_vec2i			actual_pos;

	t_ui_element	radio_layer;
	t_list			*radio_buttons;

	t_ui_layout		layout;
	t_ui_window		*win_toolbox;
	t_ui_recipe		*layer_recipe;
	t_ui_element	*layer_elems[MAX_LAYER_AMOUNT];
	t_ui_element	*layer_parent;

	t_ui_element	*button_add_layer;
	t_ui_element	*button_remove_layer;
	t_ui_element	*button_edit_layer;
	t_ui_element	*button_move_layer_up;
	t_ui_element	*button_move_layer_down;

	t_ui_element	*color_swatch;
	t_ui_element	*red_slider;
	t_ui_element	*green_slider;
	t_ui_element	*blue_slider;
	t_ui_element	*alpha_slider;
	t_ui_element	*size_slider;

	t_ui_element	*draw_button;
	t_ui_element	*text_button;
	t_ui_element	*erase_button;
	t_ui_element	*flood_button;
	t_ui_element	*sticker_button;
	t_ui_element	*move_button;
	t_ui_element	*shape_button;
	t_ui_element	*pipette_button;

	t_vec2i			first_pos;
	t_vec2i			first_pos_converted;
	bool			first_set;
	t_ui_element	*circle_button;
	t_ui_element	*square_button;
	t_ui_element	*line_button;

	t_ui_element	*text_input;
	char			*text_input_str;

	t_ui_element	*font_dropdown;
	t_ui_element	*sticker_dropdown;

	t_ui_element	*save_button;
	t_ui_element	*edit_button;
	t_ui_element	*clear_button;

	t_ui_window		*win_layer_new;
	t_ui_element	*new_layer_ok_button;
	t_ui_label		*new_layer_name_input_label;
	t_ui_label		*new_layer_width_input_label;
	t_ui_label		*new_layer_height_input_label;

	t_ui_window		*win_layer_edit;
	t_ui_element	*button_edit_layer_ok;
	t_ui_element	*input_edit_layer_name;
	t_ui_element	*input_edit_layer_width;
	t_ui_element	*input_edit_layer_height;

	t_ui_window		*win_image_edit;
	t_ui_element	*new_image_ok_button;
	t_ui_label		*new_image_width_input_label;
	t_ui_label		*new_image_height_input_label;

	t_ui_window		*win_save_image;
	t_ui_element	*input_save_image_name;
	t_ui_element	*button_save_image_ok;
}					t_guimp;

/* File: srcs/brush_modifier_events.c */
void			color_slider_events(t_guimp *guimp);
void			color_swatch_event(t_guimp *guimp);
/* File: srcs/brushes.c */
void			draw_brush(t_guimp *guimp, t_layer *active_layer,
					t_vec2i actual_pos, t_vec2i hidden_pos);
void			text_brush(t_guimp *guimp, t_layer *active_layer,
					t_vec2i actual_pos, t_vec2i hidden_pos);
void			erase_brush(t_guimp *guimp, t_layer *active_layer,
					t_vec2i actual_pos, t_vec2i hidden_pos);
void			flood_brush(t_guimp *guimp, t_layer *active_layer,
					t_vec2i actual_pos);
void			move_brush(t_guimp *guimp, t_layer *active_layer);
/* File: srcs/brushes2.c */
char			*get_sticker_path(char *sticker_name);
void			sticker_brush(t_guimp *guimp, t_layer *active_layer,
					t_vec2i actual_pos, t_vec2i hidden_pos);
void			pipette_brush(t_guimp *guimp, t_vec2i actual_pos);
/* File: srcs/brushes3.c */
void			line_tool_brush(t_guimp *guimp, t_layer *active_layer,
					t_vec2i actual_pos, t_vec2i hidden_pos);
void			square_tool_brush(t_guimp *guimp, t_layer *active_layer,
					t_vec2i actual_pos, t_vec2i hidden_pos);
void			circle_tool_brush(t_guimp *guimp, t_layer *active_layer,
					t_vec2i actual_pos, t_vec2i hidden_pos);
void			shape_brush(t_guimp *guimp, t_layer *active_layer,
					t_vec2i actual_pos, t_vec2i hidden_pos);
/* File: srcs/events1.c */
void			move_layer_in_dir(t_guimp *guimp, int dir);
void			button_move_layer_event(t_guimp *guimp);
void			save_button_event(t_guimp *guimp);
void			edit_button_event(t_guimp *guimp);
void			clear_button_event(t_guimp *guimp);
/* File: srcs/events2.c */
void			button_add_layer_event(t_guimp *guimp);
void			reorder_layers(t_guimp *guimp);
void			remove_nth_layer(t_guimp *guimp, int nth);
void			button_remove_layer_event(t_guimp *guimp);
void			edit_layer_event(t_guimp *guimp);
/* File: srcs/events3.c */
void			apply_layer_edit_event(t_guimp *guimp);
void			button_edit_layer_event(t_guimp *guimp);
/* File: srcs/help.c */
t_rgba			rgba(Uint32 r, Uint32 g, Uint32 b, Uint32 a);
void			set_sliders_to_color(t_guimp *guimp, Uint32 color);
void			get_dir_content(t_dir_content *content, char *path);
void			free_dir_content(t_dir_content *content);
void			save_surface(SDL_Surface *surface, char *file);
/* File: srcs/init1.c */
void			new_layer_window_init(t_guimp *guimp);
void			edit_layer_window_init(t_guimp *guimp);
void			save_image_window_init(t_guimp *guimp);
void			brush_init(t_guimp *guimp);
void			initteroni(t_guimp *guimp);
/* File: srcs/init2.c */
void			guimp_init(t_guimp *guimp);
void			color_swatch_init(t_guimp *guimp);
void			toolbox_window_init(t_guimp *guimp);
/* File: srcs/layer1.c */
void			draw_outlines(t_guimp *guimp, t_vec2 aspect);
void			draw_with_selected_brush(t_guimp *guimp, t_vec2 aspect);
void			layer_draw(t_guimp *guimp);
void			render_all_layers(t_guimp *guimp);
void			layer_render(t_guimp *guimp);
/* File: srcs/layer2.c */
void			layer_free(t_layer *layer);
void			resize_layer(t_layer *layer, t_vec2i wh);
void			move_image(t_guimp *guimp);
void			zoom_image(t_guimp *guimp);
void			layer_event(t_guimp *guimp);
/* File: srcs/layer3.c */
void			layer_new(t_layer *layer, char *name, t_vec4i pos,
					bool *show);
/* File: srcs/layer_element.c */
t_ui_element	*new_layer_element(t_guimp *guimp, char *layer_name,
					int nth_layer);
void			small_image_drawer(t_guimp *guimp, t_vec4 img_pos,
					t_vec2i img_size);
void			layer_elements_render(t_guimp *guimp);
void			new_layer_combination(t_guimp *guimp);
void			rename_layer(t_guimp *guimp, int layer_index, char *name);
/* File: srcs/layer_element_help.c */
void			new_element_from_recipe_with_parent(t_ui_element *elem,
					int elem_type, char *recipe_id, t_ui_element *parent);
float			get_ratio(t_vec2i orig_wh, t_vec2i new_wh);
/* File: srcs/main.c */
void			reset_image_events(t_guimp *guimp, SDL_Event e);
void			drag_n_drop_events(t_guimp *guimp, SDL_Event e);
void			user_events(t_guimp *guimp, SDL_Event e);

//////////////////////////// Flood Fille ///////////////////////////////////////

enum
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

typedef struct s_ff_pos
{
	short	x1;
	short	x2;
	short	y;
	short	dir;
}					t_ff_pos;

typedef struct s_flood_fill
{
	t_ff_pos	pos;
	Uint32		old_clr;
	int			stack_ptr;
	t_ff_pos	stack[FF_STACK_SIZE];
	int			x;
	int			y;
}				t_flood_fill;

/* File: srcs/floodfill1.c */
bool			clr_comp(SDL_Surface *surface, Uint32 clr, int x, int y);
void			push4(t_flood_fill *env, t_ff_pos pos);
void			flood_fill(SDL_Surface *surface, Uint32 newcolor, int x,
					int y);
/* File: srcs/floodfill2.c */
void			up_fill(SDL_Surface *surface, t_flood_fill *env,
					Uint32 newcolor);
void			down_fill(SDL_Surface *surface, t_flood_fill *env,
					Uint32 newcolor);
void			left_fill(SDL_Surface *surface, t_flood_fill *env,
					Uint32 newcolor);
void			right_fill(SDL_Surface *surface, t_flood_fill *env,
					Uint32 newcolor);

#endif

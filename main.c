#include "guimp.h"

void	user_events(t_guimp *guimp)
{
	new_layer_button_event(guimp);
	color_swatch_event(guimp);
}

void	user_render(t_guimp *guimp)
{

}

void	user_code(t_guimp *guimp)
{
	(void)guimp;
}

void	guimp_init(t_guimp *guimp)
{
	SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
	ui_layout_load(&guimp->layout, "layout.ui");
	guimp->win_toolbox = ui_layout_get_window_by_id(&guimp->layout, "toolbox_window");
	guimp->layer_recipe = ui_layout_get_recipe_by_id(&guimp->layout, "layer");
	guimp->layers = NULL;
	guimp->layer_parent = ui_layout_get_element_by_id(&guimp->layout, "layer_menu");
	guimp->new_layer_button = ui_layout_get_element_by_id(&guimp->layout, "button_add_layer");
	guimp->layer_count = 0;
	// Color Stuff
	guimp->color_swatch = ui_layout_get_element_by_id(&guimp->layout, "color_swatch");
	guimp->red_slider = ui_layout_get_element_by_id(&guimp->layout, "r_slider");
	guimp->green_slider = ui_layout_get_element_by_id(&guimp->layout, "g_slider");
	guimp->blue_slider = ui_layout_get_element_by_id(&guimp->layout, "b_slider");
	guimp->alpha_slider = ui_layout_get_element_by_id(&guimp->layout, "a_slider");
}

void	guimp_finalizing(t_guimp *guimp)
{
	ui_texture_fill_rect(guimp->red_slider->win->renderer, guimp->red_slider->textures[UI_STATE_DEFAULT], 0xffff0000, vec4i(0, guimp->red_slider->pos.h / 3, guimp->red_slider->pos.w, guimp->red_slider->pos.h / 3));
	ui_texture_fill_rect(guimp->green_slider->win->renderer, guimp->green_slider->textures[UI_STATE_DEFAULT], 0xff00ff00, vec4i(0, guimp->green_slider->pos.h / 3, guimp->green_slider->pos.w, guimp->green_slider->pos.h / 3));
	ui_texture_fill_rect(guimp->blue_slider->win->renderer, guimp->blue_slider->textures[UI_STATE_DEFAULT], 0xff0000ff, vec4i(0, guimp->blue_slider->pos.h / 3, guimp->blue_slider->pos.w, guimp->blue_slider->pos.h / 3));
}

int	main(void)
{
	t_guimp		guimp;
	int			run;
	SDL_Event	e;

	ui_sdl_init();
	guimp_init(&guimp);
	guimp_finalizing(&guimp);
	run = 1;
	while (run)
	{
		while (SDL_PollEvent(&e))
		{
			if (guimp.win_toolbox->wants_to_close)
				run = 0;
			// Event
			ui_layout_event(&guimp.layout, e);
		}

		// User
		user_events(&guimp);
		user_code(&guimp);
		user_render(&guimp);

		// Render
		ui_layout_render(&guimp.layout);
	}
	return (0);
}

#include  "guimp.h"

/*
 * Sets the r,g,b,a sliders on the toolbox to specifc value according to the 'color';
*/
void	set_sliders_to_color(t_guimp *guimp, Uint32 color)
{
	t_ui_slider		*red;
	t_ui_slider		*green;
	t_ui_slider		*blue;
	t_ui_slider		*alpha;
	t_ui_element	color_swatch_label;
	t_rgba		rgba;
	char		temp[20];

	rgba = hex_to_rgba(color);
	color_swatch_label = ((t_ui_input *)guimp->color_swatch->element)->label;
	red = guimp->red_slider->element;
	green = guimp->green_slider->element;
	blue = guimp->blue_slider->element;
	alpha = guimp->alpha_slider->element;
	ui_slider_value_set(guimp->red_slider, rgba.r);
	ui_slider_value_set(guimp->green_slider, rgba.g);
	ui_slider_value_set(guimp->blue_slider, rgba.b);
	ui_slider_value_set(guimp->alpha_slider, rgba.a);
	guimp->combined_color = color;
	ui_element_color_set(guimp->color_swatch, UI_STATE_DEFAULT, guimp->combined_color);
	itoa(guimp->combined_color, temp, 16);
	ui_label_text_set(&color_swatch_label, temp);
}

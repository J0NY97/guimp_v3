#include  "guimp.h"

t_rgba	rgba(Uint32 r, Uint32 g, Uint32 b, Uint32 a)
{
	return ((t_rgba){.r = r, .g = g, .b = b, .a = a});
}

/*
 * Sets the r,g,b,a sliders on the toolbox to specific
 * 	value according to the 'color';
 *
 * 	Happens in the init btw.
*/
void	set_sliders_to_color(t_guimp *guimp, Uint32 color)
{
	t_ui_element	color_swatch_label;
	t_rgba			rgba;
	char			*temp;

	rgba = hex_to_rgba(color);
	color_swatch_label = ((t_ui_input *)guimp->color_swatch->element)->label;
	ui_slider_value_set(guimp->red_slider, rgba.r);
	ui_slider_value_set(guimp->green_slider, rgba.g);
	ui_slider_value_set(guimp->blue_slider, rgba.b);
	ui_slider_value_set(guimp->alpha_slider, rgba.a);
	guimp->combined_color = color;
	ui_element_color_set(guimp->color_swatch,
		UI_STATE_DEFAULT, guimp->combined_color);
	temp = ft_itoa_base(guimp->combined_color, 16);
	ui_label_set_text(&color_swatch_label, temp);
	ft_strdel(&temp);
}

/*
 * looks into path and fills the t_dir_content with all the files inside the dir
*/
void	get_dir_content(t_dir_content *content, char *path)
{
	DIR				*d;
	struct dirent	*dir;

	memset(content, 0, sizeof(t_dir_content));
	content->dir = ft_strdup(path);
	d = opendir(path);
	if (d)
	{
		while (1)
		{
			dir = readdir(d);
			if (!dir)
				break ;
			if (dir->d_type != DT_REG)
				continue ;
			content->files = realloc(content->files,
					sizeof(char *) * ++content->file_amount);
			content->files[content->file_amount - 1] = ft_strdup(dir->d_name);
		}
		closedir(d);
	}
}

void	free_dir_content(t_dir_content *content)
{
	(void)content;
}

/*
 * takes surface and outputs to file.
*/
void	save_surface(SDL_Surface *surface, char *file)
{
	if (ft_strendswith(file, ".bmp"))
		SDL_SaveBMP(surface, file);
	else if (ft_strendswith(file, ".png"))
		IMG_SavePNG(surface, file);
	else if (ft_strendswith(file, ".jpg"))
		IMG_SaveJPG(surface, file, 10);
}

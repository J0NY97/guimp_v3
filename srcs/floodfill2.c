/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floodfill2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nneronin <nneronin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 12:57:34 by nneronin          #+#    #+#             */
/*   Updated: 2022/05/30 13:28:06 by nneronin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "guimp.h"

void	up_fill(SDL_Surface *surface, t_flood_fill *env, Uint32 newcolor)
{
	int	i;
	int	x;
	int	y;

	i = env->pos.y + 1;
	x = env->pos.x1 - 1;
	while (++x < env->pos.x2)
	{
		y = env->pos.y;
		if (clr_comp(surface, env->old_clr, x, y))
		{
			((Uint32 *)surface->pixels)[y * surface->w + x] = newcolor;
			while (--y > -1 && clr_comp(surface, env->old_clr, x, y))
				((Uint32 *)surface->pixels)[y * surface->w + x] = newcolor;
			if (y < i - 1 && x > 0)
				push4(env, (t_ff_pos){y + 1, i, x - 1, LEFT});
			else if (y > i + 1 && x < surface->w)
				push4(env, (t_ff_pos){i + 1, y, x, RIGHT});
		}
		else if (y > i + 1 && x < surface->w)
			push4(env, (t_ff_pos){i + 1, y, x, RIGHT});
		i = y;
	}
	if (i < env->pos.y && x < surface->w)
		push4(env, (t_ff_pos){i + 1, env->pos.y + 1, x, RIGHT});
}

void	down_fill(SDL_Surface *surface, t_flood_fill *env, Uint32 newcolor)
{
	int	i;
	int	x;
	int	y;

	i = env->pos.y - 1;
	x = env->pos.x1 - 1;
	while (++x < env->pos.x2)
	{
		y = env->pos.y;
		if (clr_comp(surface, env->old_clr, x, y))
		{
			((Uint32 *)surface->pixels)[y * surface->w + x] = newcolor;
			while (++y < surface->h && clr_comp(surface, env->old_clr, x, y))
				((Uint32 *)surface->pixels)[y * surface->w + x] = newcolor;
			if (y < i - 1 && x < surface->w)
				push4(env, (t_ff_pos){y - 1, i, x, RIGHT});
			else if (y > i + 1 && x > 0)
				push4(env, (t_ff_pos){i + 1, y, x - 1, LEFT});
		}
		else if (y < i && x < surface->w)
			push4(env, (t_ff_pos){y - 1, i, x, RIGHT});
		i = y;
	}
	if (env->pos.y < i && x < surface->w)
		push4(env, (t_ff_pos){env->pos.y, i, x, RIGHT});
}

void	left_fill(SDL_Surface *surface, t_flood_fill *env, Uint32 newcolor)
{
	int	i;
	int	x;
	int	y;

	i = env->pos.y + 1;
	y = env->pos.x1 - 1;
	while (++y < env->pos.x2)
	{
		x = env->pos.y;
		if (clr_comp(surface, env->old_clr, x, y))
		{
			((Uint32 *)surface->pixels)[y * surface->w + x] = newcolor;
			while (--x > -1 && clr_comp(surface, env->old_clr, x, y))
				((Uint32 *)surface->pixels)[y * surface->w + x] = newcolor;
			if (x < i - 1 && y > 0)
				push4(env, (t_ff_pos){x + 1, i, y - 1, UP});
			else if (x > i + 1 && y < surface->h)
				push4(env, (t_ff_pos){i + 1, x, y, DOWN});
		}
		else if (x > i + 1 && y < surface->h)
			push4(env, (t_ff_pos){i + 1, x, y, DOWN});
		i = x;
	}
	if (i < env->pos.y && y < surface->h)
		push4(env, (t_ff_pos){i + 1, env->pos.y + 1, y, DOWN});
}

void	right_fill(SDL_Surface *surface, t_flood_fill *env, Uint32 newcolor)
{
	int	i;
	int	x;
	int	y;

	i = env->pos.y - 1;
	y = env->pos.x1 - 1;
	while (++y < env->pos.x2)
	{
		x = env->pos.y;
		if (clr_comp(surface, env->old_clr, x, y))
		{
			((Uint32 *)surface->pixels)[y * surface->w + x] = newcolor;
			while (++x < surface->w && clr_comp(surface, env->old_clr, x, y))
				((Uint32 *)surface->pixels)[y * surface->w + x] = newcolor;
			if (x < i - 1 && y < surface->h)
				push4(env, (t_ff_pos){x - 1, i, y, DOWN});
			else if (x > i + 1 && y > 0)
				push4(env, (t_ff_pos){i + 1, x, y - 1, UP});
		}
		else if (x < i - 1 && y < surface->h)
			push4(env, (t_ff_pos){x - 1, i, y, DOWN});
		i = x;
	}
	if (env->pos.y < i && y < surface->h)
		push4(env, (t_ff_pos){env->pos.y, i, y, DOWN});
}

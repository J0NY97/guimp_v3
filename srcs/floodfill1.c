/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floodfill1.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nneronin <nneronin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 12:57:34 by nneronin          #+#    #+#             */
/*   Updated: 2022/05/30 13:29:43 by nneronin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "guimp.h"

inline bool	clr_comp(SDL_Surface *surface, Uint32 clr, int x, int y)
{
	if (((Uint32 *)surface->pixels)[y * surface->w + x] == clr)
		return (true);
	return (false);
}

static inline bool	pop4(t_flood_fill *env)
{
	if (env->stack_ptr)
	{
		env->stack_ptr--;
		env->pos = env->stack[env->stack_ptr];
		return (true);
	}
	return (false);
}

inline void	push4(t_flood_fill *env, t_ff_pos pos)
{
	if (env->stack_ptr < FF_STACK_SIZE)
	{
		env->stack[env->stack_ptr] = pos;
		env->stack_ptr++;
	}
}

static void	flood_fill2(SDL_Surface *surface,
	t_flood_fill *env, Uint32 newcolor)
{
	while (pop4(env))
	{
		env->pos.x1 = ft_max(env->pos.x1, 0);
		if (env->pos.dir == UP)
		{
			env->pos.x2 = ft_min(env->pos.x2, surface->w);
			up_fill(surface, env, newcolor);
		}
		else if (env->pos.dir == DOWN)
		{
			env->pos.x2 = ft_min(env->pos.x2, surface->w);
			down_fill(surface, env, newcolor);
		}
		else if (env->pos.dir == LEFT)
		{
			env->pos.x2 = ft_min(env->pos.x2, surface->h);
			left_fill(surface, env, newcolor);
		}
		else
		{
			env->pos.x2 = ft_min(env->pos.x2, surface->h);
			right_fill(surface, env, newcolor);
		}
	}	
}

void	flood_fill(SDL_Surface *surface, Uint32 newcolor, int x, int y)
{
	t_flood_fill	env;

	env.old_clr = ((Uint32 *)surface->pixels)[y * surface->w + x];
	if (env.old_clr == newcolor)
		return ;
	env.stack_ptr = 0;
	((Uint32 *)surface->pixels)[y * surface->w + x] = newcolor;
	env.pos.x1 = x;
	while (++x < surface->w && clr_comp(surface, env.old_clr, x, y))
		((Uint32 *)surface->pixels)[y * surface->w + x] = newcolor;
	env.pos.x2 = x - 1;
	while (--env.pos.x1 > -1 && clr_comp(surface, env.old_clr, env.pos.x1, y))
		((Uint32 *)surface->pixels)[y * surface->w + env.pos.x1] = newcolor;
	env.pos.x1++;
	if (y > 0)
		push4(&env, (t_ff_pos){env.pos.x1, env.pos.x2, y - 1, UP});
	if (y < surface->h - 1)
		push4(&env, (t_ff_pos){env.pos.x1, env.pos.x2, y + 1, DOWN});
	flood_fill2(surface, &env, newcolor);
}

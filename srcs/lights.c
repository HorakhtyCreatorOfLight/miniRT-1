/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lights.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sdunckel <sdunckel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/14 13:16:49 by sdunckel          #+#    #+#             */
/*   Updated: 2019/11/15 21:58:42 by sdunckel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mini_rt.h"

static	void	find_shadow(t_mini_rt *rt, t_element *obj, t_vec p, t_vec l)
{
	if (obj->id == SPHERE)
		sphere(rt, obj, p, l);
	else if (obj->id == PLANE)
		plane(rt, obj, p, l);
	else if (obj->id == CYLINDER)
		cylinder(rt, obj, p, l);
	else if (obj->id == CIRCLE)
		circle(rt, obj, p, l);
	//ft_strequ(obj->id, SQUARE) ? square(rt, obj) : 0;
	//ft_strequ(obj->id, TRIANGLE) ? triangle(rt, obj) : 0;
	//ft_strequ(obj->id, CYLINDER) ? cylinder(rt, obj) : 0;
}

t_color			color_average(t_color color1, t_color color2)
{
	t_color		color;

	color.r = ((color1.r + color2.r) / 2);
	color.g = ((color1.g + color2.g) / 2);
	color.b = ((color1.b + color2.b) / 2);
	return (color);
}

void			apply_intensity(t_mini_rt *rt, double intensity)
{
	if (intensity > 1 || intensity < 0)
		return ;
	rt->color.r = rt->color.r * intensity;
	rt->color.g = rt->color.g * intensity;
	rt->color.b = rt->color.b * intensity;
	// rt->color.r > 255 ? rt->color.r = 255 : 0;
	// rt->color.g > 255 ? rt->color.r = 255 : 0;
	// rt->color.b > 255 ? rt->color.r = 255 : 0;
}

int				apply_shadows(t_mini_rt *rt, t_vec P, t_vec L)
{
	t_list		*tmp;
	t_element	*obj;
	t_element	*save;

	save = rt->obj;
	rt->obj = NULL;
	tmp = rt->objs_list;
	rt->t = INT_MAX;
	rt->k = INT_MAX;
	while (tmp)
	{
		obj = tmp->content;
		if (save == obj)
		{
			save->id == CYLINDER ? tmp = tmp->next->next : 0;
			tmp = tmp->next;
			continue ;
		}
		find_shadow(rt, tmp->content, P, L);
		if (rt->t > 0 && rt->t < rt->k)
		{
			rt->k = rt->t;
			rt->obj = tmp->content;
		}
		tmp = tmp->next;
	}
	if (rt->obj != NULL)
		return (0);
	return (1);
}

void			apply_lights(t_mini_rt *rt)
{
	t_list		*tmp;
	t_element	*light;
	double		dot;
	double		intensity;
	t_vec		P;
	t_vec		N;
	t_vec		L;

	intensity = rt->ambient.ratio;
	P = vec_add(rt->ray.ori, vec_mul(rt->ray.dir, rt->k));
	rt->obj->id == SPHERE || rt->obj->id == CYLINDER ? N = vec_normalize(vec_sub(P, rt->obj->point)) : VEC_CREATE(0,0,0);
	rt->obj->id == PLANE ? N = rt->obj->orient : VEC_CREATE(0,0,0);
	tmp = rt->light_list;
	while (tmp)
	{
		light = tmp->content;
		L = vec_normalize(vec_sub(light->point, P));
		dot = VEC_ADD(vec_dot(N, L));
		//if (!apply_shadows(rt, P, L))
		//{
		//	tmp = tmp->next;
		//	continue ;
		//}
		if (dot > 0)
			intensity += light->ratio * dot / (vec_len(N) * vec_len(L));
		tmp = tmp->next;
	}
	apply_intensity(rt, intensity);
}

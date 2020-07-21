/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   vec_new.c                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/04 22:52:46 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/07/21 18:41:50 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <vector.h>
#include <stddef.h>

int		vec_new(t_vec *vec, size_t typesize)
{
	vec->capacity = 8;
	vec->data = malloc(typesize * vec->capacity);
	if (!vec->data)
		return (0);
	vec->len = 0;
	vec->size = typesize;
	return (1);
}

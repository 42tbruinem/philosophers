/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   vector_destroy.c                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/04 22:58:27 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/07/21 18:45:46 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <vector.h>

int		vec_destroy(t_vec *vec)
{
	free(vec->data);
	vec->capacity = 0;
	vec->len = 0;
	vec->size = 0;
	return (1);
}

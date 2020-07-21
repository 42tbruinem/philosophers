/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   vector.h                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/21 18:42:20 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/07/21 18:43:10 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef VECTOR_H
# define VECTOR_H

# include <stddef.h>

typedef struct		s_vec
{
	char			*data;
	size_t			size;
	size_t			capacity;
	size_t			len;
}					t_vec;

int					vec_nullterm(t_vec *vec);
int					vec_realloc(t_vec *vec, size_t amount);
int					vec_insert(t_vec *vec, void *buff, size_t amount,
								size_t index);
int					vec_add(t_vec *vec, void *buff, size_t amount);
int					vec_destroy(t_vec *vec);
int					vec_new(t_vec *vec, size_t typesize);

#endif

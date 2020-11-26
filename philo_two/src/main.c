/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/15 17:43:25 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 15:03:15 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <philo_two.h>

static int		error(t_data *data, char *errmsg)
{
	free(data->phil);
	free(data);
	write(1, errmsg, ft_strlen(errmsg));
	return (1);
}

int				main(int argc, char **argv)
{
	t_data	*data;

	data = malloc(sizeof(t_data));
	if (!data)
		return (error(data, ERROR_MEM));
	memset(data, 0, sizeof(t_data));
	if (argc != 5 && argc != 6)
		return (error(data, ERROR_ARGS));
	if (init_data(data, (argc == 6), &argv[1]))
		return (error(data, ERROR_MEM));
	data->phil = malloc(sizeof(t_phil) * data->phil_cnt);
	if (!data->phil)
		return (error(data, ERROR_MEM));
	if (start_threads(data, data->phil))
		return (error(data, ERROR_PTHREAD));
	free(data->phil);
	free(data);
	return (0);
}

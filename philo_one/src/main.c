/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/15 17:43:25 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 14:37:58 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"

static int		error(t_data *data, char *errmsg)
{
	int	i;

	i = 0;
	if (!data)
		return (1);
	while (i < data->phil_cnt)
	{
		if (data->forks_init)
			pthread_mutex_destroy(&data->forks[i]);
		if (data->phil && data->phil[i].eatlock_init)
			pthread_mutex_destroy(&data->phil->eatlock);
		i++;
	}
	if (data->messenger_init)
		pthread_mutex_destroy(&data->messenger);
	if (data->deadlock_init)
		pthread_mutex_destroy(&data->deadlock);
	free(data->phil);
	free(data->forks);
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
	safe_init_philo(data);
	if (start_threads(data, data->phil))
		return (error(data, ERROR_PTHREAD));
	free(data->phil);
	free(data->forks);
	free(data);
	return (0);
}

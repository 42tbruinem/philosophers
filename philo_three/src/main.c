/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/15 17:43:25 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 16:28:29 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo_three.h"

int		error(t_data *data, char *errmsg)
{
	free(data->phil);
	free(data);
	write(1, errmsg, ft_strlen(errmsg));
	return (1);
}

void	*mealcounter(void *arg)
{
	t_data	*data;
	sem_t	**mealsreached;
	sem_t	*halt;
	int		phil_cnt;
	int		i;

	i = 0;
	data = arg;
	mealsreached = data->mealsreached;
	phil_cnt = data->phil_cnt;
	halt = data->halt;
	sem_post(data->init);
	while (i < phil_cnt)
	{
		sem_wait(mealsreached[i]);
		i++;
	}
	sem_post(halt);
	return (NULL);
}

int		main(int argc, char **argv)
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
	start_processes(data, data->phil);
	return (0);
}

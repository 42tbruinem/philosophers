/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   data.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/04 15:38:33 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 16:32:16 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <philo_three.h>

int		parse_data(t_data *data, int eat_minimum)
{
	if (data->phil_cnt <= 0 || data->phil_cnt >= 200)
		return (1);
	if (eat_minimum && data->eat_minimum <= 0)
		return (1);
	if (data->timer.die < 60)
		return (1);
	if (data->timer.eat < 60)
		return (1);
	if (data->timer.sleep < 60)
		return (1);
	return (0);
}

int		init_data(t_data *data, int eat_minimum, char **argv)
{
	data->phil_cnt = ft_atoi(argv[0]);
	data->timer.die = ft_atoi(argv[1]);
	data->timer.eat = ft_atoi(argv[2]);
	data->timer.sleep = ft_atoi(argv[3]);
	if (eat_minimum)
		data->eat_minimum = ft_atoi(argv[4]);
	if (parse_data(data, eat_minimum))
		return (1);
	data->init = sem_start("init", 0, 0, false);
	data->messenger = sem_start("messenger", 0, 1, false);
	data->halt = sem_start("halt", 0, 0, false);
	data->deadlock = sem_start("dead", 0, 1, false);
	data->forks = sem_start("forks", 0, data->phil_cnt, false);
	data->mealsreached = malloc(sizeof(sem_t *) * data->phil_cnt);
	if (!data->mealsreached)
		return (1);
	data->pids = malloc(sizeof(int) * data->phil_cnt);
	if (!data->pids)
		return (1);
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   data.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/15 17:43:25 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 14:57:05 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <philo_two.h>

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
	sem_unlink("dead");
	data->deadlock = sem_open("dead", O_CREAT, 666, 1);
	if (!data->deadlock)
		return (1);
	sem_unlink("messenger");
	data->messenger = sem_open("messenger", O_CREAT, 666, 1);
	if (!data->messenger)
		return (1);
	sem_unlink("forks");
	data->forks = sem_open("forks", O_CREAT, 666, data->phil_cnt);
	if (!data->forks)
		return (1);
	return (0);
}

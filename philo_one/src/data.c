/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   data.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/15 17:43:25 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 14:14:58 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"

int				parse_data(t_data *data, int eat_minimum)
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

void			set_settings(t_data *data, char **argv, int eat_minimum)
{
	data->phil_cnt = ft_atoi(argv[0]);
	data->timer.die = ft_atoi(argv[1]);
	data->timer.eat = ft_atoi(argv[2]);
	data->timer.sleep = ft_atoi(argv[3]);
	if (eat_minimum)
		data->eat_minimum = ft_atoi(argv[4]);
}

int				init_data(t_data *data, int eat_minimum, char **argv)
{
	int	i;

	i = 0;
	set_settings(data, argv, eat_minimum);
	if (parse_data(data, eat_minimum))
		return (1);
	data->dead = 0;
	if (pthread_mutex_init(&data->messenger, NULL))
		return (1);
	data->messenger_init = 1;
	if (pthread_mutex_init(&data->deadlock, NULL))
		return (1);
	data->deadlock_init = 1;
	data->forks = malloc(sizeof(pthread_mutex_t) * data->phil_cnt);
	if (!data->forks)
		return (1);
	while (i < data->phil_cnt)
	{
		pthread_mutex_init(&data->forks[i], NULL);
		i++;
	}
	data->forks_init = 1;
	return (0);
}

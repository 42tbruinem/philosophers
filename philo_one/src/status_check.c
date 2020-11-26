/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   status_check.c                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/04 14:33:01 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 14:33:10 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <philo_one.h>

int				philosophers_meeting_eatrequirement(t_data *data)
{
	int i;

	i = 0;
	while (data->eat_minimum && i < data->phil_cnt)
	{
		pthread_mutex_lock(&data->phil[i].eatlock);
		if (data->phil[i].meals < data->eat_minimum)
		{
			pthread_mutex_unlock(&data->phil[i].eatlock);
			break ;
		}
		pthread_mutex_unlock(&data->phil[i].eatlock);
		i++;
	}
	return (i);
}

int				grimreaper(t_data *data)
{
	while (1)
	{
		pthread_mutex_lock(&data->deadlock);
		if (data->dead)
			return (0);
		pthread_mutex_unlock(&data->deadlock);
		if (philosophers_meeting_eatrequirement(data) == data->phil_cnt)
		{
			pthread_mutex_lock(&data->deadlock);
			return (0);
		}
		usleep(500);
	}
	return (0);
}

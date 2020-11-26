/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   manager.c                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/04 16:04:16 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 16:14:55 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <philo_three.h>

static void	exit_if_dead(t_phil *phil)
{
	unsigned long time;

	time = time_msec();
	sem_wait(phil->data->deadlock);
	if (time - phil->lasteat >= phil->data->timer.die)
	{
		message(phil, " died\n", 0);
		sem_post(phil->data->halt);
		exit(0);
	}
	else
	{
		sem_post(phil->action);
		sem_post(phil->data->deadlock);
	}
}

void		*manager(void *arg)
{
	t_phil			*phil;

	phil = arg;
	while (1)
	{
		sem_wait(phil->action);
		if (!phil->reached && phil->data->eat_minimum &&
			phil->meals >= phil->data->eat_minimum)
		{
			sem_post(phil->mealsreached);
			phil->reached = 1;
		}
		exit_if_dead(phil);
		usleep(500);
	}
	return (NULL);
}

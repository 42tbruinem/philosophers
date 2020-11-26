/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   threads.c                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/15 17:43:25 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 15:02:15 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <philo_two.h>

int			philo_meeting_eatrequirements(t_data *data)
{
	int i;

	i = 0;
	while (data->eat_minimum && i < data->phil_cnt)
	{
		sem_wait(data->phil[i].action);
		if (data->phil[i].meals < data->eat_minimum)
		{
			sem_post(data->phil[i].action);
			break ;
		}
		sem_post(data->phil[i].action);
		i++;
	}
	return (i);
}

int			weightwatcher(t_data *data)
{
	while (1)
	{
		sem_wait(data->deadlock);
		if (data->dead)
			return (0);
		sem_post(data->deadlock);
		if (philo_meeting_eatrequirements(data) == data->phil_cnt)
		{
			sem_wait(data->deadlock);
			break ;
		}
		usleep(500);
	}
	return (0);
}

static void	*manager(void *arg)
{
	t_phil			*phil;
	sem_t			*action;
	sem_t			*dead;
	unsigned long	time;

	phil = arg;
	action = phil->action;
	dead = phil->data->deadlock;
	while (1)
	{
		sem_wait(action);
		sem_wait(dead);
		time = time_msec();
		if (time - phil->lasteat >= phil->data->timer.die)
		{
			if (!phil->data->dead)
				message(phil, " died\n", 0);
			phil->data->dead++;
		}
		sem_post(dead);
		sem_post(action);
		usleep(500);
	}
	return (NULL);
}

static void	*simulate(void *arg)
{
	t_phil			*phil;

	phil = arg;
	update_lasteat(phil, time_msec());
	while (1)
	{
		message(phil, " is thinking\n", 1);
		get_forks(phil, phil->data->forks);
		update_lasteat(phil, time_msec());
		usleep(phil->data->timer.eat * 1000);
		drop_forks(phil->data->forks);
		message(phil, " is sleeping\n", 1);
		usleep(phil->data->timer.sleep * 1000);
	}
	return (NULL);
}

int			start_threads(t_data *data, t_phil *philosophers)
{
	int			i;
	pthread_t	thread;

	i = 0;
	data->starttime = time_msec();
	while (i < data->phil_cnt)
	{
		if (init_philo(&philosophers[i], data, i))
			return (1);
		if (pthread_create(&thread, NULL, simulate, &philosophers[i]))
			return (1);
		pthread_detach(thread);
		if (pthread_create(&thread, NULL, manager, &philosophers[i]))
			return (1);
		pthread_detach(thread);
		i++;
	}
	return (weightwatcher(data));
}

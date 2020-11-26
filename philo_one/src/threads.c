/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   threads.c                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/04 14:39:14 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 14:47:25 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <philo_one.h>

static void		*manager(void *arg)
{
	t_phil			*phil;
	pthread_mutex_t	*deadlock;
	pthread_mutex_t	*eatlock;
	unsigned long	time;

	phil = arg;
	deadlock = &phil->data->deadlock;
	eatlock = &phil->eatlock;
	while (1)
	{
		toggle_lock_pair(eatlock, deadlock, true);
		time = time_msec();
		if (time - phil->lasteat >= phil->data->timer.die)
		{
			if (!phil->data->dead)
				message(phil, " died\n", 0);
			phil->data->dead++;
			toggle_lock_pair(eatlock, deadlock, false);
			break ;
		}
		toggle_lock_pair(eatlock, deadlock, false);
		usleep(500);
	}
	return (NULL);
}

static void		*simulate(void *arg)
{
	t_phil			*phil;
	int				fork[2];

	phil = arg;
	update_last_eat(phil, time_msec());
	fork[LEFT] = phil->id - 1;
	fork[RIGHT] = (phil->id != phil->data->phil_cnt) ? phil->id : 0;
	while (1)
	{
		message(phil, " is thinking\n", 1);
		get_forks(phil, fork, phil->data->forks);
		update_last_eat(phil, time_msec());
		usleep(phil->data->timer.eat * 1000);
		drop_forks(fork, phil->data->forks);
		message(phil, " is sleeping\n", 1);
		usleep(phil->data->timer.sleep * 1000);
	}
	return (NULL);
}

int				start_threads(t_data *data, t_phil *philosophers)
{
	int			i;
	pthread_t	thread;

	i = 0;
	data->starttime = time_msec();
	while (i < data->phil_cnt)
	{
		init_philo(&philosophers[i], data, i);
		if (pthread_create(&thread, NULL, simulate, &philosophers[i]))
			return (1);
		pthread_detach(thread);
		if (pthread_create(&thread, NULL, manager, &philosophers[i]))
			return (1);
		pthread_detach(thread);
		i++;
		usleep(100);
	}
	return (grimreaper(data));
}

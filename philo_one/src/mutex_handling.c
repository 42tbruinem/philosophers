/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   mutex_handling.c                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/04 14:35:41 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 14:47:08 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <philo_one.h>

void			toggle_lock_pair(pthread_mutex_t *one, pthread_mutex_t *two,
								bool lock)
{
	if (lock)
	{
		pthread_mutex_lock(one);
		pthread_mutex_lock(two);
	}
	else
	{
		pthread_mutex_unlock(one);
		pthread_mutex_unlock(two);
	}
}

void			drop_forks(int *set, pthread_mutex_t *forks)
{
	pthread_mutex_unlock(&forks[set[RIGHT]]);
	pthread_mutex_unlock(&forks[set[LEFT]]);
}

void			get_forks(t_phil *phil, int *set, pthread_mutex_t *forks)
{
	pthread_mutex_lock(&forks[set[LEFT]]);
	message(phil, " has taken a fork\n", 1);
	pthread_mutex_lock(&forks[set[RIGHT]]);
	message(phil, " is eating\n", 1);
}

void			update_last_eat(t_phil *phil, unsigned long time)
{
	pthread_mutex_lock(&phil->eatlock);
	phil->meals++;
	phil->lasteat = time;
	pthread_mutex_unlock(&phil->eatlock);
}

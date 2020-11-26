/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   philosopher.c                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/15 17:43:25 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 14:39:08 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"

void			init_philo(t_phil *philosopher, t_data *data, int id)
{
	philosopher->id = id + 1;
	philosopher->data = data;
	philosopher->meals = -1;
	philosopher->lasteat = time_msec();
	pthread_mutex_init(&philosopher->eatlock, NULL);
	philosopher->eatlock_init = 1;
}

void			safe_init_philo(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->phil_cnt)
	{
		data->phil[i].eatlock_init = 0;
		i++;
	}
}

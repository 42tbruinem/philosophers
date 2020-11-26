/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   philosopher.c                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/04 15:38:33 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 16:37:38 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <philo_three.h>

void	init_philo(t_phil *philosopher, t_data *data)
{
	int		i;

	i = 0;
	while (i < data->phil_cnt)
	{
		philosopher[i].data = data;
		philosopher[i].id = i + 1;
		philosopher[i].meals = -1;
		philosopher[i].reached = 0;
		philosopher[i].lasteat = time_msec();
		philosopher[i].action = sem_start("action", i, 1, true);
		data->mealsreached[i] = sem_start("meals", i, 0, true);
		philosopher[i].mealsreached = data->mealsreached[i];
		i++;
	}
}

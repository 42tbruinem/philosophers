/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   philosopher.c                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/15 17:43:25 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 15:02:32 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <philo_two.h>

int	init_philo(t_phil *philosopher, t_data *data, int id)
{
	char	*semname;

	philosopher->data = data;
	philosopher->id = id + 1;
	philosopher->meals = -1;
	philosopher->lasteat = time_msec();
	semname = get_semname("action", id);
	if (!semname)
		return (1);
	sem_unlink(semname);
	philosopher->action = sem_open(semname, O_CREAT, 666, 1);
	if (!philosopher->action)
		return (1);
	free(semname);
	return (0);
}

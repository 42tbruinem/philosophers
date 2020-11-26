/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   processes.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/04 15:38:33 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 16:30:02 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <philo_three.h>

void	cleanup(t_data *data)
{
	sem_unlink("halt");
	sem_unlink("messenger");
	sem_unlink("forks");
	sem_unlink("dead");
	sem_wait(data->init);
	free(data->mealsreached);
	free(data->pids);
	free(data->phil);
	free(data);
}

void	kill_processes(t_data *data)
{
	int		i;
	pid_t	pid;

	i = 0;
	while (i < data->phil_cnt)
	{
		kill(data->pids[i], SIGTERM);
		i++;
	}
	i = 0;
	while (i < data->phil_cnt)
	{
		(void)waitpid(data->pids[i], &pid, 0);
		i++;
	}
	cleanup(data);
	exit(0);
}

void	simulate(t_phil *phil)
{
	pthread_t		thread;

	update_lasteat(phil, time_msec());
	pthread_create(&thread, NULL, manager, phil);
	pthread_detach(thread);
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
	exit(0);
}

void	start_processes(t_data *data, t_phil *philosophers)
{
	int			i;
	int			pid;
	pthread_t	thread;

	i = 0;
	init_philo(philosophers, data);
	data->starttime = time_msec();
	while (i < data->phil_cnt)
	{
		pid = fork();
		if (pid == -1)
			exit(1);
		if (!pid)
			simulate(&philosophers[i]);
		data->pids[i] = pid;
		i++;
	}
	pthread_create(&thread, NULL, mealcounter, data);
	pthread_detach(thread);
	sem_wait(data->halt);
	kill_processes(data);
}

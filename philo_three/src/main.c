/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/15 17:43:25 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/07/21 21:31:41 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"
#include <stdio.h>
#include <errno.h>
#include <semaphore.h>
#include <signal.h>

#define ERROR_ARGS "Wrong use of function\n"
#define ERROR_MEM "Failed to allocate memory\n"
#define ERROR_PTHREAD "Failed to create a thread\n"

unsigned long	time_msec(void)
{
	struct timeval	time;
	unsigned long	ret;

	gettimeofday(&time, NULL);
	ret = (time.tv_sec * 1000);
	ret += (time.tv_usec / 1000);
	return (ret);
}

void	ultoa(char *dest, unsigned long number, size_t *len)
{
	int				size;
	unsigned long	num;

	num = number;
	size = 1;
	while (num >= 10)
	{
		num /= 10;
		size++;
	}
	dest[size] = '\0';
	*len = size;
	while (size)
	{
		dest[size - 1] = (number % 10) + '0';
		number /= 10;
		size--;
	}
}

size_t	ft_strlen(char *str)
{
	size_t	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

size_t	ft_strcpy(char *src, char *dst)
{
	size_t	i;

	i = 0;
	while (src[i])
	{
		dst[i] = src[i];
		i++;
	}
	return (i);
}

int		ft_atoi(char *number)
{
	unsigned int	num;
	int				sign;
	size_t			i;

	i = 0;
	num = 0;
	while ((number[i] >= 9 && number[i] < 16) || number[i] == ' ')
		i++;
	sign = (number[i] == '-') ? -1 : 1;
	i += (number[i] == '-' || number[i] == '+');
	while (number[i] >= '0' && number[i] <= '9')
	{
		num = num * 10 + (number[i] - '0');
		i++;
	}
	return (num * sign);
}

int		error(t_data *data, char *errmsg)
{
	int	i;

	i = 0;
	while (i < data->phil_cnt)
	{
		sem_close(data->phil->action);
		i++;
	}
	sem_close(data->messenger);
	sem_close(data->forks);
	free(data->phil);
	free(data);
	write(1, errmsg, ft_strlen(errmsg));
	return (1);
}

int		init_data(t_data *data, int eat_minimum, char **argv)
{
	data->phil_cnt = ft_atoi(argv[0]);
	data->timer.die = ft_atoi(argv[1]);
	data->timer.eat = ft_atoi(argv[2]);
	data->timer.sleep = ft_atoi(argv[3]);
	sem_unlink("messenger");
	data->messenger = sem_open("messenger", O_CREAT, 666, 1);
	sem_unlink("halt");
	data->halt = sem_open("halt", O_CREAT, 666, 0);
	if (!data->messenger)
		exit(1);
	if (eat_minimum)
		data->eat_minimum = ft_atoi(argv[4]);
	data->pids = malloc(sizeof(int) * data->phil_cnt);
	if (!data->pids)
		exit(1);
	sem_unlink("forks");
	data->forks = sem_open("forks", O_CREAT, 666, data->phil_cnt);
	return (0);
}

void	write_len(char *str, size_t len)
{
	size_t	i;
	int		ret;

	i = 0;
	while (i != len)
	{
		ret = write(1, str + i, len - i);
		if (ret == -1)
			return ;
		i += ret;
	}
}

void	message(t_phil *phil, char *msg, int unlock)
{
	size_t	msglen;
	size_t	timelen;
	size_t	numlen;
	char	time_id[39];

	msglen = ft_strlen(msg);
	ultoa(time_id, time_msec() - phil->data->starttime, &timelen);
	time_id[timelen] = ' ';
	ultoa(time_id + timelen + 1, phil->id, &numlen);
	sem_wait(phil->data->messenger);
	write_len(time_id, numlen + timelen + 1);
	write_len(msg, msglen);
	if (unlock)
		sem_post(phil->data->messenger);
}

void	*manager(void *arg)
{
	t_phil			*phil;
	unsigned long	time;

	phil = arg;
	while (1)
	{
		sem_wait(phil->action);
		if (!phil->alreadyreached && phil->data->eat_minimum &&
			phil->meals >= phil->data->eat_minimum)
		{
			sem_post(phil->mealsreached);
			phil->alreadyreached = 1;
		}
		time = time_msec();
		if (time - phil->lasteat >= phil->data->timer.die)
		{
			message(phil, " died\n", 0);
			sem_post(phil->data->halt);
			exit(0);
		}
		else
			sem_post(phil->action);
		usleep(500);
	}
	return (NULL);
}

void	drop_forks(sem_t *forks)
{
	sem_post(forks);
	sem_post(forks);
}

void	get_forks(t_phil *phil, sem_t *forks)
{
	sem_wait(forks);
	message(phil, " has taken a fork\n", 1);
	sem_wait(forks);
	message(phil, " is eating\n", 1);
}

void	simulate(t_phil *phil)
{
	unsigned long	lasteat;
	pthread_t		thread;

	phil->lasteat = time_msec();
	pthread_create(&thread, NULL, &manager, phil);
	pthread_detach(thread);
	while (1)
	{
		message(phil, " is thinking\n", 1);
		get_forks(phil, phil->data->forks);
		lasteat = time_msec();
		sem_wait(phil->action);
		phil->meals++;
		phil->lasteat = lasteat;
		sem_post(phil->action);
		usleep(phil->data->timer.eat * 1000);
		drop_forks(phil->data->forks);
		message(phil, " is sleeping\n", 1);
		usleep(phil->data->timer.sleep * 1000);
	}
	exit(0);
}

void	init_philo(t_phil *philosopher, t_data *data)
{
	int	i;

	i = 0;
	while (i < data->phil_cnt)
	{
		philosopher[i].data = data;
		philosopher[i].id = i + 1;
		philosopher[i].meals = 0;
		philosopher[i].alreadyreached = 0;
		sem_unlink("action");
		philosopher[i].action = sem_open("action", O_CREAT, 666, 1);
		sem_unlink("meals");
		philosopher[i].mealsreached = sem_open("meals", O_CREAT, 666, 0);
		i++;
	}
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
	sem_unlink("halt");
	sem_unlink("messenger");
	sem_unlink("forks");
	free(data->phil);
	free(data);
	exit(0);
}

void	*mealcounter(void *arg)
{
	t_data	*data;
	int		i;

	i = 0;
	data = arg;
	while (i < data->phil_cnt)
	{
		sem_wait(data->phil[i].mealsreached);
		i++;
	}
	kill_processes(data);
	return (NULL);
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
	pthread_create(&thread, NULL, &mealcounter, data);
	pthread_detach(thread);
	sem_wait(data->halt);
	kill_processes(data);
}

int		main(int argc, char **argv)
{
	t_data	*data;
	t_phil	*philosophers;

	data = malloc(sizeof(t_data));
	if (!data)
		return (error(data, ERROR_MEM));
	memset(data, 0, sizeof(t_data));
	if (argc != 5 && argc != 6)
		return (error(data, ERROR_ARGS));
	if (init_data(data, (argc == 6), &argv[1]))
		return (error(data, ERROR_MEM));
	philosophers = malloc(sizeof(t_phil) * data->phil_cnt);
	if (!philosophers)
		return (error(data, ERROR_MEM));
	data->phil = philosophers;
	start_processes(data, philosophers);
	return (0);
}

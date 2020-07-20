/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/15 17:43:25 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/07/20 19:10:27 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"
#include <stdio.h>

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
		pthread_mutex_destroy(&data->forks[i]);
		pthread_mutex_destroy(&data->phil->action);
		i++;
	}
	pthread_mutex_destroy(&data->messenger);
	free(data->forks);
	free(data);
	write(1, errmsg, ft_strlen(errmsg));
	return (1);
}

int		init_data(t_data *data, int eat_minimum, char **argv)
{
	int	i;

	i = 0;
	data->phil_cnt = ft_atoi(argv[0]);
	data->timer.die = ft_atoi(argv[1]);
	data->timer.eat = ft_atoi(argv[2]);
	data->timer.sleep = ft_atoi(argv[3]);
	pthread_mutex_init(&data->messenger, NULL);
	if (eat_minimum)
		data->eat_minimum = ft_atoi(argv[4]);
	data->forks = malloc(sizeof(pthread_mutex_t) * data->phil_cnt);
	if (!data->forks)
		return (1);
	while (i < data->phil_cnt)
	{
		pthread_mutex_init(&data->forks[i], NULL);
		i++;
	}
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
	pthread_mutex_lock(&phil->data->messenger);
	write_len(time_id, numlen + timelen + 1);
	write_len(msg, msglen);
	if (unlock)
		pthread_mutex_unlock(&phil->data->messenger);
}

int		grimreaper(t_data *data)
{
	int		i;

	while (1)
	{
		if (data->dead)
			break ;
		i = 0;
		while (data->eat_minimum && i < data->phil_cnt)
		{
			pthread_mutex_lock(&data->phil[i].action);
			if (data->phil[i].meals < data->eat_minimum)
			{
				pthread_mutex_unlock(&data->phil[i].action);
				break ;
			}
			pthread_mutex_unlock(&data->phil[i].action);
			i++;
		}
		if (i == data->phil_cnt)
			return (0);
		usleep(500);
	}
	return (0);
}

void	*manager(void *arg)
{
	t_phil			*phil;
	unsigned long	time;

	phil = arg;
	while (1)
	{
		pthread_mutex_lock(&phil->action);
		time = time_msec();
		if (phil->data->dead || time - phil->lasteat >= phil->data->timer.die)
		{
			if (!phil->data->dead)
				message(phil, " died\n", 0);
			phil->data->dead++;
		}
		else
			pthread_mutex_unlock(&phil->action);
		usleep(500);
	}
	return (NULL);
}

void	drop_forks(int *set, pthread_mutex_t *forks)
{
	pthread_mutex_unlock(&forks[set[LEFT]]);
	pthread_mutex_unlock(&forks[set[RIGHT]]);
}

void	get_forks(t_phil *phil, int *set, pthread_mutex_t *forks)
{
	pthread_mutex_lock(&forks[set[LEFT]]);
	pthread_mutex_lock(&phil->action);
	message(phil, " has taken a fork\n", 1);
	pthread_mutex_unlock(&phil->action);
	pthread_mutex_lock(&forks[set[RIGHT]]);
	pthread_mutex_lock(&phil->action);
	message(phil, " is eating\n", 1);
	pthread_mutex_unlock(&phil->action);
}

void	*simulate(void *arg)
{
	t_phil			*phil;
	int				fork[2];
	unsigned long	lasteat;

	phil = arg;
	phil->lasteat = time_msec();
	fork[LEFT] = phil->id - 1;
	fork[RIGHT] = (phil->id != phil->data->phil_cnt) ? phil->id : 0;
	while (1)
	{
		message(phil, " is thinking\n", 1);
		get_forks(phil, fork, phil->data->forks);
		lasteat = time_msec();
		pthread_mutex_lock(&phil->action);
		phil->meals++;
		phil->lasteat = lasteat;
		pthread_mutex_unlock(&phil->action);
		usleep(phil->data->timer.eat * 1000);
		drop_forks(fork, phil->data->forks);
		message(phil, " is sleeping\n", 1);
		usleep(phil->data->timer.sleep * 1000);
	}
	return (NULL);
}

void	init_philo(t_phil *philosopher, t_data *data, int id)
{
	philosopher->id = id + 1;
	philosopher->data = data;
	pthread_mutex_init(&philosopher->action, NULL);
}

int	start_threads(t_data *data, t_phil *philosophers)
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
	}
	return (grimreaper(data));
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
	if (start_threads(data, philosophers))
		return (error(data, ERROR_PTHREAD));
	free(data);
	return (0);
}

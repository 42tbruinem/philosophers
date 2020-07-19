/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/15 17:43:25 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/07/19 17:30:18 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"
#include <stdio.h>

#define ERROR_ARGS "Wrong use of function\n"
#define ERROR_MEM "Failed to allocate memory\n"
#define ERROR_PTHREAD "Failed to create a thread\n"

unsigned int	time_msec(void)
{
	struct timeval	time;
	unsigned int	ret;

	gettimeofday(&time, NULL);
	ret = (time.tv_sec * 1000);
	ret += (time.tv_usec / 1000);
	return (ret);
}

void	uitoa(char *dest, unsigned int number, size_t *len)
{
	int				size;
	unsigned int	num;

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

void	putuint(unsigned int num)
{
	char			number[12];
	unsigned int	tmp;
	int				size;

	tmp = num;
	size = 0;
	while (tmp >= 10)
	{
		tmp /= 10;
		size++;
	}
	tmp = size;
	number[size] = 0;
	while (size >= 0)
	{
		number[size] = (num % 10) + '0';
		num /= 10;
		size--;
	}
	size = 0;
	while (size != tmp + 1)
		size += write(1, number + size, (tmp + 1) - size);
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
	while (number[i] >= 9 && number[i] < 16 || number[i] == ' ')
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
	size_t	i;

	i = 0;
	free(data->forks);
	free(data->threads);
	while (errmsg[i])
		i++;
	write(1, errmsg, i);
	return (1);
}

int		init_data(t_data *data, int eat_minimum, char **argv)
{
	size_t	i;

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

	i = 0;
	while (i != len)
		i += write(1, str + i, len - i);
}

void	message(t_phil *phil, char *msg)
{
	size_t	msglen;
	size_t	timelen;
	size_t	numlen;
	char	time_id[27];

	msglen = ft_strlen(msg);
	uitoa(time_id, time_msec() - phil->data->starttime, &timelen);
	time_id[timelen] = ' ';
	uitoa(time_id + timelen + 1, phil->id, &numlen);
	pthread_mutex_lock(&phil->data->messenger);
	write_len(time_id, numlen + timelen + 1);
	write_len(msg, msglen);
	pthread_mutex_unlock(&phil->data->messenger);
}

//this one makes it so that main waits for the simulation to end
void	*grimreaper(void *arg)
{
	int		i;
	t_data	*data;

	data = arg;
	while (1)
	{
		if (data->dead)
			break ;
		if (data->eat_minimum)
		{
			i = 0;
			while (i < data->phil_cnt)
			{
				if (data->phil[i].meals != data->eat_minimum)
					break ;
				i++;
			}
			if (i == data->phil_cnt)
				break ;
		}
	}
	return (NULL);
}

void	*manager(void *arg)
{
	t_phil			*phil;
	unsigned int	time;

	phil = arg;
	while (1)
	{
		time = time_msec();
		pthread_mutex_lock(&phil->action);
		if (time - phil->lasteat >= phil->data->timer.die)
		{
			if (!phil->data->dead)
				message(phil, " died\n");
			phil->data->dead++;
			break ;
		}
		pthread_mutex_unlock(&phil->action);
		usleep(500);
	}
	return (NULL);
}

void	drop_forks(int *set, pthread_mutex_t *forks)
{
	pthread_mutex_unlock(&forks[set[RIGHT]]);
	pthread_mutex_unlock(&forks[set[LEFT]]);
}

void	get_forks(t_phil *phil, int *set, pthread_mutex_t *forks)
{
	pthread_mutex_lock(&forks[set[LEFT]]);
	message(phil, " has taken a fork\n");
	pthread_mutex_lock(&forks[set[RIGHT]]);
	message(phil, " is eating\n");
}

void	*simulate(void *arg)
{
	t_phil			*phil;
	int				fork[2];
	unsigned int	lasteat;

	phil = arg;
	phil->lasteat = time_msec();
	fork[LEFT] = phil->id - 1;
	fork[RIGHT] = (phil->id != phil->data->phil_cnt) ? phil->id : 0;
	while (1)
	{
		message(phil, " is thinking\n");
		get_forks(phil, fork, phil->data->forks);
		lasteat = time_msec();
		pthread_mutex_lock(&phil->action);
		phil->meals++;
		phil->lasteat = lasteat;
		pthread_mutex_unlock(&phil->action);
		usleep(phil->data->timer.eat * 1000);
		drop_forks(fork, phil->data->forks);
		message(phil, " is sleeping\n");
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
	size_t		i;
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
	if (pthread_create(&thread, NULL, grimreaper, data))
		return (1);
	pthread_join(thread, NULL);
	return (0);
}

int		main(int argc, char **argv)
{
	t_data	data;
	t_phil	*philosophers;

	memset(&data, 0, sizeof(t_data));
	if (argc != 5 && argc != 6)
		return (error(&data, ERROR_ARGS));
	if (init_data(&data, (argc == 6), &argv[1]))
		return (error(&data, ERROR_MEM));
	philosophers = malloc(sizeof(t_phil) * data.phil_cnt);
	if (!philosophers)
		return (error(&data, ERROR_MEM));
	data.phil = philosophers;
	if (start_threads(&data, philosophers))
		return (error(&data, ERROR_PTHREAD));
	return (0);
}

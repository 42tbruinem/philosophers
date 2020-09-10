/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/15 17:43:25 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/07/21 16:05:02 by tbruinem      ########   odam.nl         */
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
	if (!data)
		return (1);
	while (i < data->phil_cnt)
	{
		if (data->forks_init)
			pthread_mutex_destroy(&data->forks[i]);
		if (data->phil && data->phil[i].eatlock_init)
			pthread_mutex_destroy(&data->phil->eatlock);
		i++;
	}
	if (data->messenger_init)
		pthread_mutex_destroy(&data->messenger);
	if (data->deadlock_init)
		pthread_mutex_destroy(&data->deadlock);
	free(data->phil);
	free(data->forks);
	free(data);
	write(1, errmsg, ft_strlen(errmsg));
	return (1);
}

int		parse_data(t_data *data, int eat_minimum)
{
	if (data->phil_cnt <= 0 || data->phil_cnt >= 200)
		return (1);
	if (eat_minimum && data->eat_minimum <= 0)
		return (1);
	if (data->timer.die < 60)
		return (1);
	if (data->timer.eat < 60)
		return (1);
	if (data->timer.sleep < 60)
		return (1);
	return (0);
}

void	set_settings(t_data *data, char **argv, int eat_minimum)
{
	data->phil_cnt = ft_atoi(argv[0]);
	data->timer.die = ft_atoi(argv[1]);
	data->timer.eat = ft_atoi(argv[2]);
	data->timer.sleep = ft_atoi(argv[3]);
	if (eat_minimum)
		data->eat_minimum = ft_atoi(argv[4]);
}

int		init_data(t_data *data, int eat_minimum, char **argv)
{
	int	i;

	i = 0;
	set_settings(data, argv, eat_minimum);
	if (parse_data(data, eat_minimum))
		return (1);
	data->dead = 0;
	if (pthread_mutex_init(&data->messenger, NULL))
		return (1);
	data->messenger_init = 1;
	if (pthread_mutex_init(&data->deadlock, NULL))
		return (1);
	data->deadlock_init = 1;
	data->forks = malloc(sizeof(pthread_mutex_t) * data->phil_cnt);
	if (!data->forks)
		return (1);
	while (i < data->phil_cnt)
	{
		pthread_mutex_init(&data->forks[i], NULL);
		i++;
	}
	data->forks_init = 1;
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
		i = 0;
		pthread_mutex_lock(&data->deadlock);
		if (data->dead)
			return (0);
		pthread_mutex_unlock(&data->deadlock);
		while (data->eat_minimum && i < data->phil_cnt)
		{
			pthread_mutex_lock(&data->phil[i].eatlock);
			if (data->phil[i].meals < data->eat_minimum)
			{
				pthread_mutex_unlock(&data->phil[i].eatlock);
				break ;
			}
			pthread_mutex_unlock(&data->phil[i].eatlock);
			i++;
		}
		if (i == data->phil_cnt)
		{
			pthread_mutex_lock(&data->deadlock);
			return (0);
		}
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
		pthread_mutex_lock(&phil->eatlock);
		pthread_mutex_lock(&phil->data->deadlock);
		time = time_msec();
		if (time - phil->lasteat >= phil->data->timer.die)
		{
			if (!phil->data->dead)
				message(phil, " died\n", 0);
			phil->data->dead++;
			pthread_mutex_unlock(&phil->eatlock);
			pthread_mutex_unlock(&phil->data->deadlock);
			break ;
		}
		pthread_mutex_unlock(&phil->eatlock);
		pthread_mutex_unlock(&phil->data->deadlock);
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
	message(phil, " has taken a fork\n", 1);
	pthread_mutex_lock(&forks[set[RIGHT]]);
	message(phil, " is eating\n", 1);
}

void update_last_eat(t_phil *phil, unsigned long time)
{
	pthread_mutex_lock(&phil->eatlock);
	phil->meals++;
	phil->lasteat = time;
	pthread_mutex_unlock(&phil->eatlock);
}

void	*simulate(void *arg)
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

void	init_philo(t_phil *philosopher, t_data *data, int id)
{
	philosopher->id = id + 1;
	philosopher->data = data;
	philosopher->meals = -1;
	philosopher->lasteat = time_msec();
	pthread_mutex_init(&philosopher->eatlock, NULL);
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
		usleep(100);
	}
	return (grimreaper(data));
}

void	safe_init_philo(t_data *data)
{
	int	i;
	
	i = 0;
	while (i < data->phil_cnt)
	{
		data->phil[i].eatlock_init = 0;
		i++;
	}
}

int		main(int argc, char **argv)
{
	t_data	*data;

	data = malloc(sizeof(t_data));
	if (!data)
		return (error(data, ERROR_MEM));
	memset(data, 0, sizeof(t_data));
	if (argc != 5 && argc != 6)
		return (error(data, ERROR_ARGS));
	if (init_data(data, (argc == 6), &argv[1]))
		return (error(data, ERROR_MEM));
	data->phil = malloc(sizeof(t_phil) * data->phil_cnt);
	if (!data->phil)
		return (error(data, ERROR_MEM));
	safe_init_philo(data);
	if (start_threads(data, data->phil))
		return (error(data, ERROR_PTHREAD));
	free(data->phil);
	free(data);
	return (0);
}

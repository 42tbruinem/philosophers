/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/15 17:43:25 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/07/16 21:06:31 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <stdlib.h>

enum	e_action
{
	EAT,
	SLEEP,
	THINK,
};

enum	e_side
{
	RIGHT,
	LEFT,
};

typedef struct	s_timer
{
	int	die;
	int	eat;
	int	sleep;
}				t_timer;

typedef struct	s_data
{
	unsigned int	starttime;
	t_phil			*phil;
	int				dead;
	int				phil_cnt;
	pthread_mutex_t	*forks;
	pthread_t		*threads;
	pthread_mutex_t	messenger;
	t_timer			timer;
	int				eat_minimum;
}				t_data;

typedef struct	s_phil
{
	t_data			*data;
	int				meals;
	pthread_mutex_t	action;
	unsigned int	lasteat;
	int				id;
}				t_phil;

#define ERROR_ARGS "Wrong use of function\n"
#define ERROR_MEM "Failed to allocate memory\n"
#define ERROR_PTHREAD "Failed to create a thread\n"

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
	write(1, number, tmp + 1);
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
	data->phil_cnt = ft_atoi(argv[0]);
	data->timer.die = ft_atoi(argv[1]);
	data->timer.eat = ft_atoi(argv[2]);
	data->timer.sleep = ft_atoi(argv[3]);
	if (eat_minimum)
		data->eat_minimum = ft_atoi(argv[4]);
	data->forks = malloc(sizeof(pthread_mutex_t) * data->phil_cnt);
	if (!data->forks)
		return (1);
	return (0);
}

unsigned int	time_msec(void)
{
	struct timeval	time;
	unsigned int	ret;

	gettimeofday(&time, NULL);
	ret = (time.tv_sec * 1000);
	ret += (time.tv_usec / 1000);
	return (ret);
}

unsigned int	time_sec(void)
{
	struct timeval	time;
	unsigned int	ret;

	gettimeofday(&time, NULL);
	ret = time.tv_sec;
	ret += (time.tv_usec / 1000000);
	return (ret);
}

void	*grimreaper(t_data *data)
{
	int	i;

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

void	*manager(t_phil *phil)
{
	while (1)
	{
		if (time_msec() - phil->lasteat >= phil->data->timer.die)
		{
			pthread_mutex_lock(&phil->action);
			message(phil, " died\n");
			phil->data->dead++;
		}
	}
}

void	drop_forks(int *set, pthread_mutex_t *forks)
{
	pthread_mutex_unlock(&forks[set[RIGHT]]);
	pthread_mutex_unlock(&forks[set[LEFT]]);
}

void	get_forks(t_phil *phil, int *set, pthread_mutex_t *forks)
{
	pthread_mutex_lock(&forks[set[RIGHT]]);
	message(phil, " has taken a fork\n");
	pthread_mutex_lock(&forks[set[LEFT]]);
	message(phil, " is eating\n");
}

void	message(t_phil *phil, char *msg)
{
	pthread_mutex_lock(&phil->data->messenger);
	putuint(time_msec() - phil->data->starttime);
	write(1, " ", 1);
	putuint(phil->id);
	write(1, msg, ft_strlen(msg));
	pthread_mutex_unlock(&phil->data->messenger);
}

void	*simulate(t_phil *phil)
{
	int				fork[2];
	unsigned int	lasteat;

	fork[RIGHT] = (phil->id > 1) ? (phil->id - 1) : phil->data->phil_cnt - 1;
	fork[LEFT] = (phil->id != phil->data->phil_cnt) ? phil->id + 1 : 0;
	phil->lasteat = time_msec();
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
	philosopher->id = id;
	philosopher->data = data;
}

int	start_threads(t_data *data, t_phil *philosophers)
{
	size_t		i;
	pthread_t	thread;

	i = 0;
	data->starttime = time_msec();
	while (i < data->phil_cnt)
	{
		init_philo(&philosophers[i], &data, i);
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

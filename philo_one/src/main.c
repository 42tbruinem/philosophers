/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/15 17:43:25 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/07/15 20:41:58 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>

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
	int				philo_count;
	pthread_mutex_t	*forks;
	pthread_t		*threads;
	pthread_mutex_t	messager;
	pthread_mutex_t	action;
	t_timer			timer;
	int				eat_minimum;
}				t_data;

typedef struct	s_philo
{
	t_data		*data;
	int			lastaction;
	int			id;
}				t_philo;

#define ERROR_ARGS "Wrong use of function\n"
#define ERROR_MEM "Failed to allocate memory\n"
#define ERROR_PTHREAD "Failed to create a thread\n"

char	*uitoa(unsigned int num)
{
	char			*number;
	unsigned int	tmp;
	int				size;

	tmp = num;
	size = 0;
	while (tmp >= 10)
	{
		tmp /= 10;
		size++;
	}
	number = malloc(sizeof(char) * (size + 1));
	if (!number)
		return (NULL);
	number[size] = 0;
	while (size)
	{
		number[size - 1] = num % 10;
		num /= 10;
		size--;
	}
	return (number);
}

int		atoi(char *number)
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
	data->philo_count = atoi(argv[0]);
	data->timer.die = atoi(argv[1]);
	data->timer.eat = atoi(argv[2]);
	data->timer.sleep = atoi(argv[3]);
	if (eat_minimum)
		data->eat_minimum = atoi(argv[4]);
	data->forks = malloc(sizeof(pthread_mutex_t) * data->philo_count);
	if (!data->forks)
		return (1);
	data->threads = malloc(sizeof(pthread_t) * data->philo_count);
	if (!data->threads)
	{
		free(data->forks);
		return (1);
	}
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

void	*simulate(t_philo *phil)
{
	pthread_mutex_t	fork[2];

	fork[RIGHT] = (phil->id > 1) ? phil->data->forks[(phil->id - 1)] :
				phil->data->forks[phil->data->philo_count - 1];
	fork[LEFT] = (phil->id != phil->data->philo_count) ?
				phil->data->forks[phil->id + 1] : phil->data->forks[0];
	phil->lastaction = time_msec();
	while (1)
	{
		if (phil->)


	}

}

void	init_philo(t_philo *philosopher, t_data *data, int id)
{
	philosopher->id = id;
	philosopher->data = data;
}

int		main(int argc, char **argv)
{
	t_data	data;
	t_philo	*philosophers;
	int		threadcnt;

	memset(&data, 0, sizeof(t_data));
	if (argc != 5 && argc != 6)
		return (error(&data, ERROR_ARGS));
	if (init_data(&data, (argc == 6), &argv[1]))
		return (error(&data, ERROR_MEM));
	philosophers = malloc(sizeof(t_philo) * data.philo_count);
	if (!philosophers)
		return (error(&data, ERROR_MEM));
	threadcnt = 0;
	while (threadcnt < data.philo_count)
	{
		init_philo(&philosophers[threadcnt], &data, threadcnt);
		if (pthread_create(&data.threads[threadcnt],
				NULL, simulate, &philosophers[threadcnt]))
			return (error(&data, ERROR_PTHREAD));
		threadcnt++;
	}
	threadcnt = 0;
	while (threadcnt < data.philo_count)
	{
		pthread_join(data.threads[threadcnt], NULL);
		threadcnt++;
	}
	return (0);
}

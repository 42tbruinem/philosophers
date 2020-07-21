/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/15 17:43:25 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/07/21 18:33:01 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"
#include <stdio.h>
#include <errno.h>

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

char	*get_semname(char *basename, unsigned long id)
{
	char	*new;
	size_t	bname_len;
	size_t	size;

	bname_len = ft_strlen(basename);
	new = malloc(sizeof(char) * bname_len + 1 + 20);
	if (!new)
		return (NULL);
	size = ft_strcpy(basename, new);
	new[size] = ' ';
	ultoa(new + size + 1, id, &size);
	new[bname_len + size + 1] = '\0';
	return (new);
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
	if (!data->messenger)
	{
		printf("errno: %s\n", strerror(errno));
		return (1);
	}
	if (eat_minimum)
		data->eat_minimum = ft_atoi(argv[4]);
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
			sem_wait(data->phil[i].action);
			if (data->phil[i].meals < data->eat_minimum)
			{
				sem_post(data->phil[i].action);
				break ;
			}
			sem_post(data->phil[i].action);
			i++;
		}
		if (i == data->phil_cnt)
			break ;
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
		sem_wait(phil->action);
		time = time_msec();
		if (phil->data->dead || time - phil->lasteat >= phil->data->timer.die)
		{
			if (!phil->data->dead)
				message(phil, " died\n", 0);
			phil->data->dead++;
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
	sem_wait(phil->action);
	message(phil, " has taken a fork\n", 1);
	sem_post(phil->action);
	sem_wait(forks);
	sem_wait(phil->action);
	message(phil, " is eating\n", 1);
	sem_post(phil->action);
}

void	*simulate(void *arg)
{
	t_phil			*phil;
	unsigned long	lasteat;

	phil = arg;
	phil->lasteat = time_msec();
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
	return (NULL);
}

void	init_philo(t_phil *philosopher, t_data *data, int id)
{
	char	*semname;

	philosopher->data = data;
	philosopher->id = id + 1;
	philosopher->meals = 0;
	semname = get_semname("action", id);
	if (!semname)
		return ;
	sem_unlink(semname);
	sem_open(semname, O_CREAT, 666, 1);
	free(semname);
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
	sem_unlink("messenger");
	sem_unlink("forks");
	free(philosophers);
	free(data);
	return (0);
}

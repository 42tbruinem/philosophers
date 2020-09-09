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

#include "philo_three.h"
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

char	*get_semname(char *basename, unsigned long id)
{
	char	*new;
	size_t	bname_len;
	size_t	size;

	bname_len = ft_strlen(basename);
	new = malloc(sizeof(char) * bname_len + 1 + 20);
	if (!new)
		return (new);
	size = ft_strcpy(basename, new);
	new[size] = ' ';
	ultoa(new + size + 1, id, &size);
	new[bname_len + size + 1] = '\0';
	return (new);
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
	free(data->phil);
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

int		init_data(t_data *data, int eat_minimum, char **argv)
{
	data->phil_cnt = ft_atoi(argv[0]);
	data->timer.die = ft_atoi(argv[1]);
	data->timer.eat = ft_atoi(argv[2]);
	data->timer.sleep = ft_atoi(argv[3]);
	if (eat_minimum)
		data->eat_minimum = ft_atoi(argv[4]);
	if (parse_data(data, eat_minimum))
		return (1);
	sem_unlink("messenger");
	sem_unlink("halt");
	sem_unlink("forks");
	sem_unlink("dead");
	sem_unlink("init");
	data->mealsreached = malloc(sizeof(sem_t*) * data->phil_cnt);
	if (!data->mealsreached)
		return (1);
	data->init = sem_open("init", O_CREAT, 666, 0);
	if (!data->init)
		return (1);
	data->messenger = sem_open("messenger", O_CREAT, 666, 1);
	if (!data->messenger)
		return (1);
	data->halt = sem_open("halt", O_CREAT, 666, 0);
	if (!data->halt)
		return(1);
	data->deadlock = sem_open("dead", O_CREAT, 666, 1);
	if (!data->deadlock)
		return(1);
	data->pids = malloc(sizeof(int) * data->phil_cnt);
	if (!data->pids)
		return(1);
	data->forks = sem_open("forks", O_CREAT, 666, data->phil_cnt);
	if (!data->forks)
		return(1);
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
			exit(1);
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
		if (!phil->reached && phil->data->eat_minimum &&
			phil->meals >= phil->data->eat_minimum)
		{
			sem_post(phil->mealsreached);
			phil->reached = 1;
		}
		time = time_msec();
		sem_wait(phil->data->deadlock);
		if (time - phil->lasteat >= phil->data->timer.die)
		{
			message(phil, " died\n", 0);
			sem_post(phil->data->halt);
			exit(0);
		}
		else
		{
			sem_post(phil->action);
			sem_post(phil->data->deadlock);
		}
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

void	update_lasteat(t_phil *phil, unsigned long time)
{
	sem_wait(phil->action);
	phil->meals++;
	phil->lasteat = time;
	sem_post(phil->action);
}

void	simulate(t_phil *phil)
{
	pthread_t		thread;

	update_lasteat(phil, time_msec());
	pthread_create(&thread, NULL, &manager, phil);
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

void	init_philo(t_phil *philosopher, t_data *data)
{
	int		i;
	char	*semname;

	i = 0;
	while (i < data->phil_cnt)
	{
		philosopher[i].data = data;
		philosopher[i].id = i + 1;
		philosopher[i].meals = -1;
		philosopher[i].reached = 0;
		philosopher[i].lasteat = time_msec();
		semname = get_semname("action", i);
		if (!semname)
			exit(1);
		sem_unlink(semname);
		philosopher[i].action = sem_open(semname, O_CREAT, 666, 1);
		if (!philosopher[i].action)
			exit(1);
		free(semname);
		semname = get_semname("meals", i);
		if (!semname)
			exit(1);
		sem_unlink(semname);
		philosopher[i].mealsreached = sem_open(semname, O_CREAT, 666, 0);
		if (!philosopher[i].mealsreached)
			exit(1);
		data->mealsreached[i] = philosopher[i].mealsreached;
		free(semname);
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
	sem_unlink("dead");
	sem_wait(data->init);
	free(data->pids);
	free(data->phil);
	free(data);
	exit(0);
}

void	*mealcounter(void *arg)
{
	t_data	*data;
	sem_t	**mealsreached;
	sem_t	*halt;
	int		phil_cnt;
	int		i;

	i = 0;
	data = arg;
	mealsreached = data->mealsreached;
	phil_cnt = data->phil_cnt;
	halt = data->halt;
	sem_post(data->init);
	while (i < phil_cnt)
	{
		sem_wait(mealsreached[i]);
		i++;
	}
	sem_post(halt);
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
	start_processes(data, data->phil);
	return (0);
}

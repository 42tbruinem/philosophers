/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   philo_loop.c                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/17 19:28:03 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/07/17 19:33:49 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/time.h>
#include <string.h>

unsigned int	time_msec(void)
{
	struct timeval	time;
	unsigned int	ret;

	gettimeofday(&time, NULL);
	ret = (time.tv_sec * 1000);
	ret += (time.tv_usec / 1000);
	return (ret);
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
	write(1, number, tmp + 1);
}

void	message(char *msg, unsigned int start)
{
	putuint(time_msec() - start);
	write(1, msg, strlen(msg));
}

int		main(void)
{
	unsigned int	start;
	int				time_sleep;
	int				time_eat;

	time_sleep = 200;
	time_eat = 200;
	start = time_msec();
	while (1)
	{
		message(" 1 is thinking\n", start);
		message(" 1 has taken a fork\n", start);
		message(" 1 is eating\n", start);
		usleep(time_eat * 1000);
		message(" 1 is sleeping\n", start);
		usleep(time_sleep * 1000);
	}
	return (0);
}

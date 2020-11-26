/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   utils.c                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/15 17:43:25 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 15:03:41 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <philo_two.h>

unsigned long	time_msec(void)
{
	struct timeval	time;
	unsigned long	ret;

	gettimeofday(&time, NULL);
	ret = (time.tv_sec * 1000);
	ret += (time.tv_usec / 1000);
	return (ret);
}

void			ultoa(char *dest, unsigned long number, size_t *len)
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

size_t			ft_strlen(char *str)
{
	size_t	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

size_t			ft_strcpy(char *src, char *dst)
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

int				ft_atoi(char *number)
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

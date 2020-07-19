/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   uitoa_test.c                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/19 16:44:33 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/07/19 16:48:44 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

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

int		main(void)
{
	unsigned int	i;
	char			num[13];
	size_t			numlen;

	i = 0;
	while (1)
	{
		uitoa(num, i, &numlen);
		write(1, num, numlen);
		write(1, "\n", 1);
		i++;
		usleep(100000);
	}
	return (0);
}

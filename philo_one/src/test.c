/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   test.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/16 11:57:53 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/07/16 12:10:25 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

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

int		main(void)
{
	int	i;

	i = 0;
	while (1)
	{
		putuint(i);
		write(1, "\n", 1);
		i++;
	}
	return (0);
}

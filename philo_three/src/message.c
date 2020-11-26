/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   message.c                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/04 15:38:33 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 15:57:47 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <philo_three.h>

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

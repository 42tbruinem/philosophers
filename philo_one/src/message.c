/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   message.c                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/04 14:37:03 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 14:37:52 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <philo_one.h>

static void		write_len(char *str, size_t len)
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

void			message(t_phil *phil, char *msg, int unlock)
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

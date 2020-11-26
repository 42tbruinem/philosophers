/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   semaphore.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/15 17:43:25 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 15:00:18 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <philo_two.h>

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

void	update_lasteat(t_phil *phil, unsigned long time)
{
	sem_wait(phil->action);
	phil->meals++;
	phil->lasteat = time;
	sem_post(phil->action);
}

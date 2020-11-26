/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   semaphore.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/04 15:38:33 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 16:17:36 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <philo_three.h>

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

sem_t	*sem_start(char *name, int num, int val, bool append)
{
	sem_t	*newsem;

	if (append)
	{
		name = get_semname(name, num);
		if (!name)
			exit(1);
	}
	sem_unlink(name);
	newsem = sem_open(name, O_CREAT, 666, val);
	if (!newsem)
		exit(1);
	if (append)
		free(name);
	return (newsem);
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

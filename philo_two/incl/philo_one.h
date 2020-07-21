/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   philo_one.h                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/16 21:11:34 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/07/21 21:17:24 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_ONE_H
# define PHILO_ONE_H

# include <unistd.h>
# include <semaphore.h>
# include <pthread.h>
# include <string.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <stdlib.h>

enum	e_side
{
	RIGHT,
	LEFT,
};

typedef struct	s_timer
{
	unsigned long	die;
	unsigned long	eat;
	unsigned long	sleep;
}				t_timer;

typedef struct s_data	t_data;

typedef struct	s_phil
{
	t_data			*data;
	int				meals;
	sem_t			*action;
	unsigned long	lasteat;
	int				id;
}				t_phil;

struct			s_data
{
	unsigned long	starttime;
	int				*pids;
	t_phil			*phil;
	int				dead;
	sem_t			*halt;
	int				phil_cnt;
	sem_t			*forks;
	sem_t			*messenger;
	t_timer			timer;
	int				eat_minimum;
};

#endif

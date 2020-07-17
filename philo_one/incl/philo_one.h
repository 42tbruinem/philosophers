/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   philo_one.h                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/16 21:11:34 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/07/16 23:25:17 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_ONE_H
# define PHILO_ONE_H

#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <stdlib.h>

enum	e_action
{
	EAT,
	SLEEP,
	THINK,
};

enum	e_side
{
	RIGHT,
	LEFT,
};

typedef struct	s_timer
{
	int	die;
	int	eat;
	int	sleep;
}				t_timer;

typedef struct s_data	t_data;

typedef struct	s_phil
{
	t_data			*data;
	int				meals;
	pthread_mutex_t	action;
	pthread_mutex_t	halt;
	unsigned int	lasteat;
	int				id;
}				t_phil;

struct			s_data
{
	unsigned int	starttime;
	t_phil			*phil;
	int				dead;
	int				phil_cnt;
	pthread_mutex_t	*forks;
	pthread_t		*threads;
	pthread_mutex_t	messenger;
	t_timer			timer;
	int				eat_minimum;
};

#endif

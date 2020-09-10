/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   philo_one.h                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/16 21:11:34 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/07/20 19:08:39 by tbruinem      ########   odam.nl         */
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
	unsigned long	die;
	unsigned long	eat;
	unsigned long	sleep;
}				t_timer;

typedef struct s_data	t_data;

typedef struct	s_phil
{
	t_data			*data;
	int				meals;
	pthread_mutex_t	eatlock;
	int				eatlock_init;
	unsigned long	lasteat;
	int				id;
}				t_phil;

struct			s_data
{
	unsigned long	starttime;
	t_phil			*phil;
	int				dead;
	int				phil_cnt;
	pthread_mutex_t	*forks;
	int				forks_init;
	pthread_mutex_t	messenger;
	int				messenger_init;
	pthread_mutex_t deadlock;
	int				deadlock_init;
	t_timer			timer;
	int				eat_minimum;
};

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   philo_one.h                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/16 21:11:34 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 14:43:43 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_ONE_H
# define PHILO_ONE_H

# include <unistd.h>
# include <pthread.h>
# include <string.h>
# include <fcntl.h>
# include <sys/time.h>
# include <stdlib.h>
# include <stdbool.h>

# define ERROR_ARGS "Wrong use of function\n"
# define ERROR_MEM "Failed to allocate memory\n"
# define ERROR_PTHREAD "Failed to create a thread\n"

enum							e_side
{
	RIGHT,
	LEFT,
};

typedef struct					s_timer
{
	unsigned long				die;
	unsigned long				eat;
	unsigned long				sleep;
}								t_timer;

typedef struct s_data			t_data;

typedef struct					s_phil
{
	t_data						*data;
	int							meals;
	pthread_mutex_t				eatlock;
	int							eatlock_init;
	unsigned long				lasteat;
	int							id;
}								t_phil;

struct							s_data
{
	unsigned long				starttime;
	t_phil						*phil;
	int							dead;
	int							phil_cnt;
	pthread_mutex_t				*forks;
	int							forks_init;
	pthread_mutex_t				messenger;
	int							messenger_init;
	pthread_mutex_t				deadlock;
	int							deadlock_init;
	t_timer						timer;
	int							eat_minimum;
};

void							drop_forks(int *set, pthread_mutex_t *forks);
void							toggle_lock_pair(pthread_mutex_t *one,
								pthread_mutex_t *two, bool lock);
void							message(t_phil *phil, char *msg, int unlock);
size_t							ft_strlen(char *str);
int								parse_data(t_data *data, int eat_minimum);
void							set_settings(t_data *data, char **argv,
								int eat_minimum);
int								init_data(t_data *data, int eat_minimum,
								char **argv);
void							get_forks(t_phil *phil, int *set,
								pthread_mutex_t *forks);
void							update_last_eat(t_phil *phil,
								unsigned long time);
unsigned long					time_msec(void);
void							init_philo(t_phil *philosopher, t_data *data,
								int id);
void							safe_init_philo(t_data *data);
int								philosophers_meeting_eatrequirement(
								t_data *data);
int								ft_atoi(char *number);
int								grimreaper(t_data *data);
int								start_threads(t_data *data,
								t_phil *philosophers);
void							ultoa(char *dest, unsigned long number,
								size_t *len);

#endif

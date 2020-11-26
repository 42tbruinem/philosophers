/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   philo_two.h                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/16 21:11:34 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/11/04 14:54:09 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_TWO_H
# define PHILO_TWO_H

# include <unistd.h>
# include <semaphore.h>
# include <pthread.h>
# include <string.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <stdlib.h>
# include <errno.h>

# define ERROR_ARGS "Wrong use of function\n"
# define ERROR_MEM "Failed to allocate memory\n"
# define ERROR_PTHREAD "Failed to create a thread\n"

enum					e_side
{
	RIGHT,
	LEFT,
};

typedef struct			s_timer
{
	unsigned long		die;
	unsigned long		eat;
	unsigned long		sleep;
}						t_timer;

typedef struct s_data	t_data;

typedef struct			s_phil
{
	t_data				*data;
	int					meals;
	sem_t				*action;
	unsigned long		lasteat;
	int					id;
}						t_phil;

struct					s_data
{
	unsigned long		starttime;
	int					*pids;
	t_phil				*phil;
	int					dead;
	int					phil_cnt;
	sem_t				*halt;
	sem_t				*forks;
	sem_t				*messenger;
	sem_t				*deadlock;
	t_timer				timer;
	int					eat_minimum;
};

int						ft_atoi(char *number);
int						init_philo(t_phil *philosopher, t_data *data, int id);
void					message(t_phil *phil, char *msg, int unlock);
void					write_len(char *str, size_t len);
size_t					ft_strlen(char *str);
char					*get_semname(char *basename, unsigned long id);
int						parse_data(t_data *data, int eat_minimum);
void					drop_forks(sem_t *forks);
int						weightwatcher(t_data *data);
void					update_lasteat(t_phil *phil, unsigned long time);
int						start_threads(t_data *data, t_phil *philosophers);
int						init_data(t_data *data, int eat_minimum, char **argv);
void					get_forks(t_phil *phil, sem_t *forks);
int						philo_meeting_eatrequirements(t_data *data);
unsigned long			time_msec(void);
void					ultoa(char *dest, unsigned long number, size_t *len);
size_t					ft_strcpy(char *src, char *dst);

#endif

# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: tbruinem <tbruinem@student.codam.nl>         +#+                      #
#                                                    +#+                       #
#    Created: 2020/07/15 15:30:15 by tbruinem      #+#    #+#                  #
#    Updated: 2020/07/15 17:43:09 by tbruinem      ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

NAME =		philo_one
SRC_DIR =	./src
OBJ_DIR =	./obj
TAIL =		-pthread
INCL =		-I .
FLAGS =		-Wall -Wextra -Werror
SRC	=		main.c

SRC :=		$(SRC:%.c=)
OBJ =		$(SRC:%.c=%.o)

all: $(NAME)

obj/%.o: src/%.o
	@mkdir -p $($D)
	@echo "Compiling $@"
	$(CC) -c $(FLAGS) $(INCL) $< -o $@

$(NAME): $(OBJ)

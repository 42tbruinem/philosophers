# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: tbruinem <tbruinem@student.codam.nl>         +#+                      #
#                                                    +#+                       #
#    Created: 2020/07/15 15:30:15 by tbruinem      #+#    #+#                  #
#    Updated: 2020/11/04 15:06:39 by tbruinem      ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

NAME =		philo_two
SRC_DIR =	src
OBJ_DIR =	obj
TAIL =		-pthread
INCL =		-I incl/
FLAGS =		-Wall -Wextra -Werror
SRC	=		data.c \
			main.c \
			message.c \
			philosopher.c \
			semaphore.c \
			threads.c \
			utils.c

OBJ := $(addprefix $(OBJ_DIR)/, $(SRC:%.c=%.o))
SRC := $(addprefix $(SRC_DIR)/, $(SRC))

ifeq ($(DEBUG),1)
FLAGS += -g -fsanitize=address
endif

ifeq ($(DEBUG),2)
FLAGS += -g -fsanitize=thread
endif

all: $(NAME)

obj/%.o: src/%.c
	@mkdir -p $(@D)
	@echo "Compiling $@"
	@$(CC) -c $(FLAGS) $(INCL) $< $(TAIL) -o $@

$(NAME): $(OBJ)
	@$(CC) $(FLAGS) $(INCL) $(TAIL) $^ -o $@
	@echo "Succesfully compiled $(NAME)"

clean:
	@rm -rf $(OBJ)
	@echo "Done cleaning"

fclean: clean
	@rm -rf $(NAME)
	@echo "Done fcleaning"

re: fclean all

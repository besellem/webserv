# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/10/18 14:30:22 by besellem          #+#    #+#              #
#    Updated: 2021/10/19 14:24:38 by kaye             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# COMPILATION

CC		= clang++
CFLAGS 	= -Wall -Wextra -Werror -std=c++98
IFLAGS 	= -I./incs

# DIRECTORIES

BUILD 			:= .build
SRC_DIR 		:= srcs
OBJ_DIR 		:= $(BUILD)/obj
## all sub folder add here: (check exemple)
# SUB_DIR 		:= exemple
SUB_DIR			:= epoll
DIRS			:= $(OBJ_DIR) $(addprefix $(OBJ_DIR)/, $(SUB_DIR))

# FILES

NAME			:= webserv
SRC				:= main.cpp
## all sub file add here: (check exemple)
# SUB_SRC			:= exemple.cpp
SUB_SRC			:= epoll.cpp
# SRC				+= $(addprefix exemple/, $(SUB_SRC))
SRC				+= $(addprefix epoll/, $(SUB_SRC))
OBJ				:= $(SRC:%.cpp=$(OBJ_DIR)/%.o)

# COLORS

NONE			= \033[0m
CL_LINE			= \033[2K
B_BLACK			= \033[1;30m
B_RED			= \033[1;31m
B_GREEN			= \033[1;32m
B_YELLOW		= \033[1;33m
B_BLUE			= \033[1;34m
B_MAGENTA 		= \033[1;35m
B_CYAN 			= \033[1;36m

# MAKEFILE

$(NAME): $(OBJ)
	@printf "$(CL_LINE)"
	@echo "[1 / 1] - $(B_MAGENTA)$@$(NONE)"
	@$(CC) $(CFLAGS) $(OBJ) -o $@
	@echo "$(B_GREEN)Compilation done !$(NONE)"

all: $(NAME)

clean:
	@rm -Rf $(BUILD)
	@echo "$(B_RED)$(BUILD)$(NONE): $(B_GREEN)Delete$(NONE)"

fclean: clean
	@rm -Rf $(NAME)
	@echo "$(B_RED)$(NAME)$(NONE): $(B_GREEN)Delete$(NONE)"

re: fclean all

.PHONY: all clean fclean re

$(BUILD):
	@mkdir $@ $(DIRS)

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.cpp | $(BUILD)
	@printf "$(CL_LINE)Compiling srcs object : $(B_CYAN)$< $(NONE)...\r"
	@$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@
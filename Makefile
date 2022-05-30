# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nneronin <nneronin@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/07/26 11:13:50 by nneronin          #+#    #+#              #
#    Updated: 2022/05/30 11:37:40 by nneronin         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SHELL_NAME	= $(shell uname -s)
CPU = $(firstword $(subst -, ,$(shell gcc -dumpmachine)))

ifeq ($(CPU), mingw32)
	MAKE = mingw32-make
else
	MAKE = make
endif

RED			=	"\e[0;31m"
GREEN		=	"\e[0;32m"
YELLOW		=	"\e[0;33m"
BLUE		=	"\e[0;34m"
MAGENTA		=	"\e[0;35m"
CYAN		=	"\e[0;36m"
RESET		=	"\e[0m"

CFILES		=	main.c\
				layer_element_help.c\
				layer_element.c\
				layer.c\
				help.c\
				floodfill.c\
				events.c\
				brushes3.c\
				brushes2.c\
				brushes.c\
				brush_modifier_events.c

NAME		=	guimp
CDIR		=	./srcs
ODIR		=	./obj
INC			=	-I./
SRCS		=	$(addprefix $(CDIR)/,$(CFILES))
OBJ			=	$(addprefix $(ODIR)/,$(CFILES:.c=.o))
DEP			=	$(OBJ:.o=.d)

LIB_DIR		=	./libs

LIB_INC		=	-I $(LIB_DIR)/libft \
				-I $(LIB_DIR)/libpf \
				-I $(LIB_DIR)/libui \
				-I $(LIB_DIR)/liblg

LIBS		=	-L $(LIB_DIR)/libft -lft \
				-L $(LIB_DIR)/libpf -lpf \
				-L $(LIB_DIR)/libui -lui \
				-L $(LIB_DIR)/liblg -llg

ifeq ($(SHELL_NAME), Darwin)

LIB_INC		+=	-I ./libs/SDL_MAC/SDL2.framework/Headers -I ./libs/SDL_MAC/SDL2_image.framework/Headers -I ./libs/SDL_MAC/SDL2_ttf.framework/Headers -F ./libs/SDL_MAC/
LIBS		+=	-framework SDL2 -framework SDL2_image -framework SDL2_ttf -F ./libs/SDL_MAC/
CFLAGS		=	-Wall -Wextra -MMD -O3 -Werror -Wfatal-errors# -g -fsanitize=address

else

LIB_INC		+=	-I ./libs/SDL_WIN/SDL2_includes
LIBS		+=	-L ./libs/SDL_WIN/ -lSDL2 -lSDL2_ttf -lSDL2_image
CFLAGS		=	-Wall -Wextra -MMD -Wno-unused-variable -O3 -flto #-Werror -Wfatal-errors

endif

all: make_libs $(ODIR) $(NAME)
	@printf $(GREEN)"\e[J[INFO] $(NAME) ready!\n"$(RESET)

make_libs:
	@$(MAKE) -C ./libs/libft -j6
	@$(MAKE) -C ./libs/libpf -j6
	@$(MAKE) -C ./libs/liblg -j6
	@$(MAKE) -C ./libs/libui -j6 

-include $(DEP)

$(ODIR):
	@mkdir -p $@

$(NAME): $(OBJ)
	@gcc $(CFLAGS) $(OBJ) $(LIB_INC) $(INC) $(LIBS) -o $(NAME)

$(ODIR)/%.o: $(CDIR)/%.c
	@gcc -c $< -o $@ $(CFLAGS) $(LIB_INC) $(INC)
	@printf $(YELLOW)"\e[JCompiling $<\n\e[F"$(RESET)

clean:
	@printf $(CYAN)"[INFO] $(NAME) cleaned!\n"$(RESET)
	@rm -rf $(ODIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: clean, all, re, fclean
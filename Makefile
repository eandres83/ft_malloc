ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so

SYMLINK = libft_malloc.so

OBJ_DIR = obj
INC_DIR = include

SRCS = src/ft_malloc.c \
	src/ft_malloc_utils.c \
	src/ft_free.c \
	src/ft_realloc.c \
	src/show_alloc_mem.c \

OBJS = $(patsubst src/%.c, $(OBJ_DIR)/%.o, $(SRCS))

CC = cc
CFLAGS = -g -Wall -Werror -Wextra -fPIC

INCLUDES = -I $(INC_DIR)

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Compilando libreria compartida $(NAME)..."
	$(CC) $(CFLAGS) -shared -o $(NAME) $(OBJS)
	@echo "Creando enlace simbolico $(SYMLINK)..."
	ln -sf $(NAME) $(SYMLINK)

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME) $(SYMLINK)

re: fclean all

.PHONY: all clean fclean re

NAME = libasm.a
CC = clang
ASM = nasm
CFLAGS = -Wall -Wextra -Werror
ASMFLAGS = -f elf64

SRC = src/ft_strlen.s \
      src/ft_strcpy.s \
      src/ft_strcmp.s \
      src/ft_strdup.s \
      src/ft_read.s \
      src/ft_write.s

OBJ = $(SRC:.s=.o)

TEST = test
TEST_SRC = test.c

all: $(NAME) $(TEST)

$(NAME): $(OBJ)
	ar rcs $(NAME) $(OBJ)

%.o: %.s libasm.h
	$(ASM) $(ASMFLAGS) $< -o $@

$(TEST): $(NAME) $(TEST_SRC)
	$(CC) $(CFLAGS) $(TEST_SRC) -L. -lasm -o $(TEST)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME) $(TEST)

re: fclean all

.PHONY: all clean fclean re

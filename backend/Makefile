SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CC = gcc
CFLAGS = -I $(INCLUDE_DIR) -Werror

all:
	mkdir -p obj
	$(CC) -c $(SRC_DIR)/bitboard.c -o $(OBJ_DIR)/bitboard.o $(CFLAGS)
	$(CC) -c $(SRC_DIR)/chess.c -o $(OBJ_DIR)/chess.o $(CFLAGS)
	$(CC) -c $(SRC_DIR)/list.c -o $(OBJ_DIR)/list.o $(CFLAGS)

clean:
	rm -rf obj/* bin/*
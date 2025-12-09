SRC_DIR = backend/src
INCLUDE_DIR = backend/include
BIN_DIR = backend/bin

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CC = gcc
CFLAGS = -I $(INCLUDE_DIR)

all:
	$(CC) -c backend/src/bitboard.c -o backend/obj/bitboard.o $(CFLAGS)
	$(CC) -c backend/src/chess.c -o backend/obj/chess.o $(CFLAGS)
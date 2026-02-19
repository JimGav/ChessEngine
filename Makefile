SRC_DIR = backend/src
INCLUDE_DIR = backend/include
BIN_DIR = backend/bin
OBJ_DIR = backend/obj
LIB_DIR = backend/lib

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CC = gcc
CFLAGS = -I $(INCLUDE_DIR) -Wall -g -Werror -fPIC

all:
	mkdir -p $(OBJ_DIR)
	$(CC) -c $(SRC_DIR)/bitboard.c -o $(OBJ_DIR)/bitboard.o $(CFLAGS)
	$(CC) -c $(SRC_DIR)/chess.c -o $(OBJ_DIR)/chess.o $(CFLAGS)
	$(CC) -c $(SRC_DIR)/list.c -o $(OBJ_DIR)/list.o $(CFLAGS)
	$(CC) -c $(SRC_DIR)/moves.c -o $(OBJ_DIR)/moves.o $(CFLAGS)
	$(CC) -c $(SRC_DIR)/sqr.c -o $(OBJ_DIR)/sqr.o $(CFLAGS)

	mkdir -p $(LIB_DIR)
	gcc $(OBJ) -shared -o $(LIB_DIR)/engine.so

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR)

run:
	python3 main.py
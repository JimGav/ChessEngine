SRC_DIR = backend/src
INCLUDE_DIR = backend/include
BIN_DIR = backend/bin
OBJ_DIR = backend/obj
LIB_DIR = backend/lib
TEST_DIR = backend/tests

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CC = gcc
CFLAGS = -I $(INCLUDE_DIR) -Wall -g -Werror -fPIC

all:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(LIB_DIR)
	mkdir -p $(BIN_DIR)
	
	$(CC) -c $(SRC_DIR)/bitboard.c -o $(OBJ_DIR)/bitboard.o $(CFLAGS)
	$(CC) -c $(SRC_DIR)/chess.c -o $(OBJ_DIR)/chess.o $(CFLAGS)
	$(CC) -c $(SRC_DIR)/list.c -o $(OBJ_DIR)/list.o $(CFLAGS)
	$(CC) -c $(SRC_DIR)/moves.c -o $(OBJ_DIR)/moves.o $(CFLAGS)
	$(CC) -c $(SRC_DIR)/sqr.c -o $(OBJ_DIR)/sqr.o $(CFLAGS)
	$(CC) -c $(SRC_DIR)/interface.c -o $(OBJ_DIR)/interface.o $(CFLAGS)

	$(CC) $(OBJ) -shared -o $(LIB_DIR)/engine.so

	$(CC) $(OBJ) $(TEST_DIR)/perft.c -o $(BIN_DIR)/perft -I $(INCLUDE_DIR)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR)

run:
	python3 main.py

test:
	$(BIN_DIR)/perft 10

CC = gcc
SRC = src/*.c
TARGET = yacc

all: 
	$(CC) $(SRC) -o $(TARGET)

clean:
	rm $(TARGET)

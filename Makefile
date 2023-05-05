CC = gcc
SRC = src/*.c
TARGET = yacc
LFLAGS = -lSDL2
all: 
	$(CC) $(SRC) $(LFLAGS) -o $(TARGET)

clean:
	rm $(TARGET)

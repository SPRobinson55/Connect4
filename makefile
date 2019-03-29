CC=gcc
CFLAGS=-lncurses

c4: main.o
	$(CC) -o c4.exe main.o $(CFLAGS)

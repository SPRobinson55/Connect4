CC=gcc
CFLAGS=-lncurses

c4.exe: main.o
	$(CC) -Wall -o c4.exe main.o $(CFLAGS)


#Compiler
CC = gcc


all: lab1

lab1: lab1.o message.o
	$(CC) -o lab1 lab1.o message.o

message.o: message.c
	$(CC) -c message.c

lab1.o: lab1.c
	$(CC) -c lab1.c

clean:
	rm -rf *.o lab1

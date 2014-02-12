CC = gcc
ARG = -lpthread

all: t1 t2 t3 ex1

ex1: 
	$(CC) -o ex1 $(ARG) ex1.c

t1: 
	$(CC) -o t1 $(ARG) t1.c
	
t2: 
	$(CC) -o t2 $(ARG) t2.c

t3: 
	$(CC) -o t3 $(ARG) t3.c
	
clean:
	rm -rf t1 t2 t3 ex1 *.o

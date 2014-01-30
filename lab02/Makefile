CC = gcc

all: processTree fork1 fork2 waitSync timeSlice killTest execFunction mainReturn

processTree:
	$(CC) -o processTree processTree.c

fork1:
	$(CC) -o fork1 fork1.c

fork2: fork2.c
	$(CC) -o fork2 fork2.c

waitSync: waitSync.c
	$(CC) -o waitSync waitSync.c

timeSlice:
	$(CC) -o timeSlice timeSlice.c

killTest:
	$(CC) -o killTest killTest.c

execFunction:
	$(CC) -o execFunction execFunction.c

mainReturn:
	$(CC) -o mainReturn mainReturn.c

zombieMaker:
	$(CC) -o zombieMaker zombieMaker.c

clean:
	rm -rf processTree fork1 fork2  waitSync timeSlice killTest execFunction mainReturn zombieMaker *.o

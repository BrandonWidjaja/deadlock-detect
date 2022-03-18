CC=gcc
CFLAGS=-Wall
EXE_DETECT=detect

detect: detect.c
	$(CC) $(CFLAGS) -o $(EXE_DETECT) detect.c

clean:
	rm -f *.o $(EXE_DETECT)

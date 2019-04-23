.PHONY: all clean
XX = gcc -g3
all:regular
regular:regular.o
	$(XX) regular.o -o regular
.c.o:
	$(XX) -c $*.c -o $*.o
clean:
	rm *.o
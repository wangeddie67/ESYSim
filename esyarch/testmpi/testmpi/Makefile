CC=/usr/lch/simplescalar/bin/sslittle-na-sstrix-gcc
#CC = /usr/bin/mips-linux-gcc
CFLAGS= -static -w -I/usr/lch/simplescalar/lib/gcc-lib/sslittle-na-sstrix/2.7.2.3/include
PROGN = fftmpi
all:$(PROGN)
$(PROGN):API.o $(PROGN).o
	$(CC) $(CFLAGS) API.o $(PROGN).o -o $(PROGN)
API.o:API.s
	$(CC) $(CFLAGS) -c API.s
$(PROGN).o:$(PROGN).c
	$(CC) $(CFLAGS) -c $(PROGN).c -o $(PROGN).o
	$(CC) $(CFLAGS) -E $(PROGN).c -o $(PROGN).i
	$(CC) $(CFLAGS) -S $(PROGN).i -o $(PROGN).s
	$(CC) $(CFLAGS) -c $(PROGN).s -o $(PROGN).o
clean:
	rm -rf *.i $(PROGN).s *.o *.elf MPI *~

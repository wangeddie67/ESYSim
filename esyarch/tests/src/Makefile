CC=/home/ctb/simplescalar/sslittle-na-sstrix/bin/gcc
CFLAGS=-g -O3

all: anagram test-printf test-fmath test-math test-llong test-lswlr

anagram: anagram.c
	$(CC) $(CFLAGS) -o anagram anagram.c

test-printf: test-printf.c
	$(CC) $(CFLAGS) -o test-printf test-printf.c

test-fmath: test-fmath.c
	$(CC) $(CFLAGS) -o test-fmath test-fmath.c

test-math: test-math.c
	$(CC) $(CFLAGS) -o test-math test-math.c

test-llong: test-llong.c
	$(CC) $(CFLAGS) -o test-llong test-llong.c

test-lswlr: test-lswlr.c
	$(CC) $(CFLAGS) -o test-lswlr test-lswlr.c


distclean:
	-make clean

clean:
	rm -f anagram test-printf test-fmath test-math test-llong test-lswlr test-as *.[oia] core *~


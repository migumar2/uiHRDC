CC= g++
CFLAGS = -m64 -O9  -DENCODE_DIFFERENCES

all: clean pfd_test

pfd_test: pfd_coding.o
	$(CC) $(CFLAGS) pfd_test.c pfd_coding.o -o pfd_test


pfd_coding.o:
	$(CC) $(CFLAGS) pfd_coding.c -c
	

clean: 
	rm -f *.o pfd_test

tar: 
	tar czvf pfd_test.tar.gz Makefile *.c *.h

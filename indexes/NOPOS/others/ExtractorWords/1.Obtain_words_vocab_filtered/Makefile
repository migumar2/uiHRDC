SRCDIR = ./src

CC          = gcc
CFLAGS      = -m64 -O9
CFLAGS2     = -Wall -O9

all:  extractor cleanO

clean:
	rm -rf *~ *% *.o core *.bck  EXTRACTWORDS

cleanO:
	rm -rf *.o 

extractor:   BufferedReader.o MemoryManager.o valstring.o hash.o 
	$(CC) $(CFLAGS) valstring.o BufferedReader.o MemoryManager.o hash.o -o EXTRACTWORDS  $(SRCDIR)/extractor.c -lm 

BufferedReader.o:  
	$(CC) $(CFLAGS) -c $(SRCDIR)/BufferedReader.c

	
MemoryManager.o:  
	$(CC) $(CFLAGS) -c $(SRCDIR)/MemoryManager.c

valstring.o:  
	$(CC) $(CFLAGS) -c $(SRCDIR)/valstring.c

hash.o:  
	$(CC) $(CFLAGS) -c $(SRCDIR)/hash.c
	

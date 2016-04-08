SRCDIR = ./src

CC          = gcc
CFLAGS      =  -g -O0 -m32
CFLAGS2     = -Wall -O9

all:  extractor cleanO

clean:
	rm -f *~ *% *.o core *.bck  EXTRACTPHRASES  

cleanO:
	rm -f *.o  

extractor:     valstring.o  hashStopWords.o
	$(CC) $(CFLAGS) valstring.o hashStopWords.o -o EXTRACTPHRASES  $(SRCDIR)/extractor.c -lm 

hashStopWords.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/hashStopWords.c

valstring.o:  
	$(CC) $(CFLAGS) -c $(SRCDIR)/valstring.c



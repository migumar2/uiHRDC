SRCDIR = ./src


CC          = gcc
CFLAGS      =   -O0 -g -m64  -DM64
//CFLAGS      =   -O0 -g -m64  -DM32
CFLAGSX      =  -g -O0 -m32
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

tar:
	tar czvf extractor.words.norepeat.noEspaces.noStopwords.tar.gz src Makefile README go*


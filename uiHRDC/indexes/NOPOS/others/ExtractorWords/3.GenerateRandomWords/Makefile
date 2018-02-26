SRCDIR = .

CC          = gcc 
CFLAGS      =  -O9 -m32
CFLAGS2      = -Wall -O9

all:  randomchooser

 randomchooser:   
	$(CC) $(CFLAGS)  -o RANDOMCHOOSER $(SRCDIR)/randomPatternsChooser.c -lm 

clean:
	rm -rf *~ *% *.o core *.bck  RANDOMCHOOSER

SRCDIR = ./src
SRCDIRUTILS = utils
SRCDIRLINKER = linker
SRCDIRILISTS = ilists
CC          = g++

CFLAGS     = -O9 -m64
#CFLAGS      = -g -O0 -m64

#DEBUGOPT  =-DDEBUG_ON
DEBUGOPT   =-DDEBUG_OFF 

all: exes  cleanO

###  builds the index ----------------------------------------
index: ilspire07.o  basics.o  sort_alg.o
	ar rc ilspire07.a ilspire07.o  basics.o	sort_alg.o
	rm *.o

###  links the index to obtain the exes ----------------------
exes: ilspire07gol.o  basics.o  
	ar rc il_1.4golomb_nosampling_lenlist_64bit.a ilspire07gol.o  basics.o sort_alg.o
	$(CC) $(DEBUGOPT) -o myIL $(SRCDIRLINKER)/build_il.c  il_1.4golomb_nosampling_lenlist_64bit.a $(CFLAGS)		
	$(CC) $(DEBUGOPT) -o myILSearch $(SRCDIRLINKER)/search_il.c il_1.4golomb_nosampling_lenlist_64bit.a  $(CFLAGS)		
	rm *.o

##  ----------------------------------------------------------

ilspire07gol.o:  basics.o sort_alg.o
	 $(CC) $(CFLAGS) -c  $(SRCDIR)/ilspire07gol.c 
	 
ilspire07gol:  basics.o  sort_alg.o
	 $(CC) -DFACADEWITHMAIN $(CFLAGS) $(DEBUGOPT)  basics.o sort_alg.o  -o EXTRACTPOSTINGS $(SRCDIR)/ilspire07gol.c -lm


sort_alg.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/sort_alg.c
		 
	 
basics.o: basics.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/basics.c
	
basics.c: basics.h
#	touch $(SRCDIR)/$(SRCDIRUTILS)/basics.c	

basics.h: 
#	touch $(SRCDIR)/$(SRCDIRUTILS)/basics.h


cleanO: 
	rm -f *.o
	
clean:
	rm -rf *~ *% *.o core *.bak il_1.4golomb_nosampling_lenlist_64bit.a myIL myILSearch

tar:
	# datetex=ilspire07golBitmap[$(date +%F_%H_%M_%S)].tar.gz 
	# tar czvf "$datetex" Makefile src linker scripts README patts 
	tar czvf ilspire07golBitmap.tar.gz Makefile src linker scripts README patts 
	
bz:
	# datetex=ilspire07golBitmap[$(date +%F_%H_%M_%S)].tar.bz2
	# tar jcvf  "$datetex" Makefile src linker scripts README patts 
	tar jcvf ilspire07golBitmap.tar.bz2 Makefile src linker scripts README patts 


##  valgrind  --leak-check=full --show-reachable=yes ./myIL ../crr.post crr "samplerate=8;filename=output"
##  valgrind  --leak-check=full --show-reachable=yes ./myILSearch crr I 20 2 "int2=svs;fsearch=dec" < patterns.file
##

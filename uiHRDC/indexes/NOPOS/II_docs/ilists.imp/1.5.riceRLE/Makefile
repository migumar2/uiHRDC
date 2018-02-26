SRCDIR = ./src
SRCDIRUTILS = utils
SRCDIRLINKER = linker
SRCDIRILISTS = ilists
CC          = g++

#CFLAGS     = -O9 -m64 
export CFLAGS   = -O9 -Wall   -D_FORTIFY_SOURCE=0 

#DEBUGOPT  =-DDEBUG_ON
DEBUGOPT   =-DDEBUG_OFF 


LIB_IL_INDEX_RICE = il_1.5.riceRLE_64bit.a
#LIB_IL_INDEX_RICE = ilspire07gol.a

all: exes  cleanO

###  builds the index ----------------------------------------
index: ilspire07.o  basics.o  sort_alg.o
	ar rc $(LIB_IL_INDEX_RICE) ilspire07.o  basics.o	sort_alg.o
	rm *.o

###  links the index to obtain the exes ----------------------
exes: ilspire07gol.o  basics.o  
	ar rc $(LIB_IL_INDEX_RICE) ilspire07gol.o  basics.o sort_alg.o
	$(CC) $(DEBUGOPT) -o myIL $(SRCDIRLINKER)/build_il.c $(LIB_IL_INDEX_RICE) $(CFLAGS)		
	$(CC) $(DEBUGOPT) -o myILSearch $(SRCDIRLINKER)/search_il.c $(LIB_IL_INDEX_RICE)  $(CFLAGS)		
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
	rm -rf *~ *% *.o core *.bak $(LIB_IL_INDEX_RICE) myIL myILSearch

tar:
	# datetex=il_riceRLE[$(date +%F_%H_%M_%S)].tar.gz 
	# tar czvf "$datetex" Makefile src linker scripts README patts go*
	tar czvf il_riceRLE.tar.gz Makefile src linker scripts README patts  go*
	
bz:
	# datetex=il_riceRLE[$(date +%F_%H_%M_%S)].tar.bz2
	# tar jcvf  "$datetex" Makefile src linker scripts README patts go*
	tar jcvf il_riceRLE.tar.bz2 Makefile src linker scripts README patts go*


##  valgrind  --leak-check=full --show-reachable=yes ./myIL ../crr.post crr "samplerate=8;filename=output"
##  valgrind  --leak-check=full --show-reachable=yes ./myILSearch crr I 20 2 "int2=svs;fsearch=dec" < patterns.file
##

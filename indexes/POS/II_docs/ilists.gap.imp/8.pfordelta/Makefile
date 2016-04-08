SRCDIR = ./src
SRCDIRUTILS = utils
SRCDIRLINKER = linker
SRCDIRILISTS = ilists
CC          = g++

export CFLAGS   = -O9  -m64    -D_FORTIFY_SOURCE=0 
export CFLAGSWALL = -Wall   ## skipping in pfordelta.o launches warnings!

#CFLAGS     = -O9 -m32
#CFLAGS     = -O9 
#CFLAGS      = -g -O0 -m32

#DEBUGOPT  =-DDEBUG_ON
DEBUGOPT   =-DDEBUG_OFF 

#LIB_IL_INDEX_PFORDELTA = il_pfordelta.a
LIB_IL_INDEX_PFORDELTA=il_8.pfordelta_64bit.a

all: exes  cleanO

###  builds the index ---------------------------------------- 
index: ilspire07.o  basics.o  sort_alg.o
	@echo "Compiling sources to create the library $(LIB_IL_INDEX_PFORDELTA)"
	ar rc $(LIB_IL_INDEX_PFORDELTA) ilspire07.o basics.o sort_alg.o pfd_coding.o
	rm -f *.o

###  links the index to obtain the exes ----------------------
exes: ilspire07.o  basics.o  
	@echo "Now building myIL && myILSearch testing programs"
	ar rc $(LIB_IL_INDEX_PFORDELTA) ilspire07.o basics.o sort_alg.o pfd_coding.o
	$(CC) $(DEBUGOPT) -o myIL $(SRCDIRLINKER)/build_il.c  $(LIB_IL_INDEX_PFORDELTA) $(CFLAGS)		
	$(CC) $(DEBUGOPT) -o myILSearch $(SRCDIRLINKER)/search_il.c $(LIB_IL_INDEX_PFORDELTA)  $(CFLAGS)		
	rm *.o


##  ----------------------------------------------------------

ilspire07.o:  basics.o sort_alg.o pfd_coding.o
	 $(CC) $(CFLAGS) $(CFLAGSWALL) -c  $(SRCDIR)/ilspire07.c 
	 
ilspire07:  basics.o 
	 $(CC) -DFACADEWITHMAIN $(CFLAGS) $(CFLAGSWALL) $(DEBUGOPT)  basics.o sort_alg.o pfd_coding.o -o EXTRACTPOSTINGS $(SRCDIR)/ilspire07.c -lm
searcherFacade:  basics.o  
	 $(CC) -DSEARCHERFACADEWITHMAIN $(CFLAGS) $(CFLAGSWALL) $(DEBUGOPT)  basics.o sort_alg.o pfd_coding.o -o SEARCHERFACADE $(SRCDIR)/ilspire07.c -lm
	 

sort_alg.o: 
	$(CC) $(CFLAGS) $(CFLAGSWALL) -c $(SRCDIR)/$(SRCDIRUTILS)/sort_alg.c
		 
	 
basics.o: 
	$(CC) $(CFLAGS) $(CFLAGSWALL) -c $(SRCDIR)/$(SRCDIRUTILS)/basics.c
	
pfd_coding.o:
	$(CC) $(CFLAGS)  -c $(SRCDIR)/ilists/pfd_coding.c


cleanO: 
	rm -f *.o
	
clean:
	rm -rf *~ *% *.o core *.bak $(LIB_IL_INDEX_PFORDELTA) myIL myILSearch

tar:
	# datetex=il_pfordelta[$(date +%F_%H_%M_%S)].tar.gz 
	# tar czvf "$datetex" Makefile src linker scripts README patts go*
	tar czvf il_pfordelta.tar.gz Makefile src linker scripts README patts go*
	
bz:
	# datetex=il_pfordelta[$(date +%F_%H_%M_%S)].tar.bz2
	# tar jcvf  "$datetex" Makefile src linker scripts README patts go*
	tar jcvf il_pfordelta.tar.bz2 Makefile src linker scripts README patts go*


##  valgrind  --leak-check=full --show-reachable=yes ./myIL ../crr.post crr "samplerate=8;filename=output"
##  valgrind  --leak-check=full --show-reachable=yes ./myILSearch crr I 20 2 "int2=svs;fsearch=dec" < patterns.file
##

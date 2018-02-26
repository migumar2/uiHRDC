SRCDIR = ./src
SRCDIRUTILS = utils
SRCDIRLINKER = linker
SRCDIRILISTS = ilists
CC          = g++

export CFLAGS   = -O3   -m64 -msse4   -D_FORTIFY_SOURCE=0 
export CFLAGSWALL = -Wall   ## skipping in QMXCODING.o launches warnings!

#CFLAGS     = -O9 -m32
#CFLAGS     = -O9 
#CFLAGS      = -g -O0 -m32

#DEBUGOPT  =-DDEBUG_ON
DEBUGOPT   =-DDEBUG_OFF 

LIB_IL_INDEX_QMXCODING=il_9.QMXCODING_64bit.a


all: exes  cleanO

###  builds the index ---------------------------------------- 
index: ilspire07.o  basics.o  sort_alg.o
	@echo "Compiling sources to create the library $(LIB_IL_INDEX_QMXCODING)"
	ar rc $(LIB_IL_INDEX_QMXCODING) ilspire07.o basics.o sort_alg.o qmx_coding.o
	rm -f *.o

###  links the index to obtain the exes ----------------------
exes: ilspire07.o  basics.o  
	@echo "Now building myIL && myILSearch testing programs"
	ar rc $(LIB_IL_INDEX_QMXCODING) ilspire07.o basics.o sort_alg.o qmx_coding.o
	$(CC) $(DEBUGOPT) -o myIL $(SRCDIRLINKER)/build_il.c  $(LIB_IL_INDEX_QMXCODING) $(CFLAGS)		
	$(CC) $(DEBUGOPT) -o myILSearch $(SRCDIRLINKER)/search_il.c $(LIB_IL_INDEX_QMXCODING)  $(CFLAGS)		
	rm *.o


##  ----------------------------------------------------------

ilspire07.o:  basics.o sort_alg.o qmx_coding.o
	 $(CC) $(CFLAGS) $(CFLAGSWALL) -c  $(SRCDIR)/ilspire07.c 
	 
ilspire07:  basics.o 
	 $(CC) -DFACADEWITHMAIN $(CFLAGS) $(CFLAGSWALL) $(DEBUGOPT)  basics.o sort_alg.o qmx_coding.o -o EXTRACTPOSTINGS $(SRCDIR)/ilspire07.c -lm
searcherFacade:  basics.o  
	 $(CC) -DSEARCHERFACADEWITHMAIN $(CFLAGS) $(CFLAGSWALL) $(DEBUGOPT)  basics.o sort_alg.o qmx_coding.o -o SEARCHERFACADE $(SRCDIR)/ilspire07.c -lm
	 

sort_alg.o: 
	$(CC) $(CFLAGS) $(CFLAGSWALL) -c $(SRCDIR)/$(SRCDIRUTILS)/sort_alg.c
		 
	 
basics.o: 
	$(CC) $(CFLAGS) $(CFLAGSWALL) -c $(SRCDIR)/$(SRCDIRUTILS)/basics.c
	
qmx_coding.o:
	$(CC) $(CFLAGS)  -c $(SRCDIR)/ilists/qmx_coding.c


cleanO: 
	rm -f *.o
	
clean:
	rm -rf *~ *% *.o core *.bak $(LIB_IL_INDEX_QMXCODING) myIL myILSearch

tar:
	# datetex=il_QMXCODING[$(date +%F_%H_%M_%S)].tar.gz 
	# tar czvf "$datetex" Makefile src linker scripts README patts go*
	tar czvf il_QMXCODING.tar.gz Makefile src linker scripts README patts go*
	
bz:
	# datetex=il_QMXCODING[$(date +%F_%H_%M_%S)].tar.bz2
	# tar jcvf  "$datetex" Makefile src linker scripts README patts go*
	tar jcvf il_QMXCODING.tar.bz2 Makefile src linker scripts README patts go*


##  valgrind  --leak-check=full --show-reachable=yes ./myIL ../crr.post crr "samplerate=8;filename=output"
##  valgrind  --leak-check=full --show-reachable=yes ./myILSearch crr I 20 2 "int2=svs;fsearch=dec" < patterns.file
##

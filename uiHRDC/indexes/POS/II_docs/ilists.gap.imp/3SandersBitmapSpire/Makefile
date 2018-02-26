SRCDIR = ./src
SRCDIRUTILS = utils
SRCDIRLINKER = linker
SRCDIRILISTS = ilists
CC          = g++
CFLAGS     = -O0 -g -m64 -DNDEBUG -D_FORTIFY_SOURCE=0  
#CFLAGS      = -g -O0
#CFLAGS      = -g -Wall

#DEBUGOPT  =-DDEBUG_ON
DEBUGOPT   =-DDEBUG_OFF

LIB_IL_IDX = il_3.sandersBitmapByteCodes_64bit.a


all: exes  cleanO

###  builds the index ----------------------------------------
index: ilsandersbc.o  basics.o  sort_alg.o bc.o
	ar rc ilsandersbc.a ilsandersbc.o  basics.o	sort_alg.o bc.o
	rm *.o

###  links the index to obtain the exes ----------------------
exes: ilsandersbc.o  basics.o  sort_alg.o  bc.o
	ar rc ilsandersbc.a ilsandersbc.o  basics.o sort_alg.o bc.o
	$(CC) $(DEBUGOPT) -o myIL $(SRCDIRLINKER)/build_il.c  ilsandersbc.a $(CFLAGS)		
	$(CC) $(DEBUGOPT) -o myILSearch $(SRCDIRLINKER)/search_il.c ilsandersbc.a  $(CFLAGS)	
	
	mv ilsandersbc.a  $(LIB_IL_IDX)
		
	rm *.o


##  ----------------------------------------------------------

ilsandersbc.o:  basics.o sort_alg.o bc.o
	 $(CC) $(CFLAGS) -c  $(SRCDIR)/ilsandersbc.c 
	 
ilsandersbc:  basics.o 
	 $(CC) -DFACADEWITHMAIN $(CFLAGS) $(DEBUGOPT)  basics.o sort_alg.o bc.o  -o EXTRACTPOSTINGS $(SRCDIR)/ilsandersbc.c -lm


sort_alg.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/sort_alg.c

bc.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRILISTS)/bc.c		 
	 
basics.o: basics.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/basics.c
	
basics.c: basics.h
#	touch $(SRCDIR)/$(SRCDIRUTILS)/basics.c	

basics.h: 
#	touch $(SRCDIR)/$(SRCDIRUTILS)/basics.h


cleanO: 
	rm -f *.o
	
clean:
	rm -rf *~ *% *.o core *.bak ilsandersbc.a myIL myILSearch

tar:
	# datetex=ilsandersbc[$(date +%F_%H_%M_%S)].tar.gz 
	# tar czvf "$datetex" Makefile src linker scripts README patts samplePosting
	tar czvf ilsandersbc.tar.gz Makefile src linker 
	
bz:
	# datetex=ilsandersbc[$(date +%F_%H_%M_%S)].tar.bz2
	# tar jcvf  "$datetex" Makefile src linker scripts README patts samplePosting
	tar jcvf ilsandersbc.tar.bz2 Makefile src linker


##  valgrind  --leak-check=full --show-reachable=yes ./myIL ../crr.post crr "samplerate=8;filename=output"
##  valgrind  --leak-check=full --show-reachable=yes ./myILSearch crr I 20 2 "int2=svs;fsearch=dec" < patterns.file
##

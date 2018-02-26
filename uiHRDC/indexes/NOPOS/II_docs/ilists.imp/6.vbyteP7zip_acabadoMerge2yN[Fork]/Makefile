SRCDIR = ./src
SRCDIRUTILS = utils
SRCDIRLINKER = linker
SRCDIRILISTS = ilists
#LIBLZMAFARIPATH = lzmalib/farilzma.a

LIBLZMAFARIPATH = lzmalib/farilzma_simplified64bit.a
#LIBLZMAFARIPATH = lzmalib/farilzma_simplified.a


LIBILINDEX = il_6.vbyte_lzma_64bit.a   ##library that is created.
#LIBILINDEX = il_6.vbyte_lzma_32bit.a   ##library that is created.

CC          = g++

#CFLAGS     = -O9 -m32  -D_FORTIFY_SOURCE=0
CFLAGS     = -O9 -m64  -D_FORTIFY_SOURCE=0
#CFLAGS     = -O0 -g3 -m64  -D_FORTIFY_SOURCE=0
#CFLAGS     = -O9 
#CFLAGS      = -g -O0 -m32

#DEBUGOPT  =-DDEBUG_ON
DEBUGOPT   =-DDEBUG_OFF 


all: exes  cleanO
	@cd  lzmalib/lzma.sourceCode && sh compile.sh



###  builds the index ----------------------------------------
index: ilspire07.o  basics.o  sort_alg.o
	##ar rc $(LIBILINDEX) ilspire07.o  basics.o sort_alg.o
	cp $(LIBLZMAFARIPATH) $(LIBILINDEX)
	ar rc $(LIBILINDEX) ilspire07.o  basics.o sort_alg.o
	rm *.o

###  links the index to obtain the exes ----------------------
exes: ilspire07.o  basics.o  
	##ar rc $(LIBILINDEX) ilspire07.o  basics.o sort_alg.o
	cp $(LIBLZMAFARIPATH) $(LIBILINDEX)
	ar rc $(LIBILINDEX) ilspire07.o  basics.o sort_alg.o

	$(CC) $(DEBUGOPT) -o myIL $(SRCDIRLINKER)/build_il.c  $(LIBILINDEX) $(CFLAGS)		
	$(CC) $(DEBUGOPT) -o myILSearch $(SRCDIRLINKER)/search_il.c $(LIBILINDEX)  $(CFLAGS)		
	rm *.o


##  ----------------------------------------------------------

ilspire07.o:  basics.o sort_alg.o
	 $(CC) $(CFLAGS) -c  $(SRCDIR)/ilspire07.c 
	 
ilspire07:  basics.o 
	 $(CC) -DFACADEWITHMAIN $(CFLAGS) $(DEBUGOPT)  basics.o sort_alg.o  -o EXTRACTPOSTINGS $(SRCDIR)/ilspire07.c -lm
searcherFacade:  basics.o  
	 $(CC) -DSEARCHERFACADEWITHMAIN $(CFLAGS) $(DEBUGOPT)  basics.o sort_alg.o -o SEARCHERFACADE $(SRCDIR)/ilspire07.c -lm
	 

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
	rm -rf *~ *% *.o core *.bak il_vbyte_nosampling.a myIL myILSearch

tar:
	# datetex=il_vbytep7zip[$(date +%F_%H_%M_%S)].tar.gz 
	# tar czvf "$datetex" Makefile src linker scripts README patts samplePosting lzmalib
	tar czvf il_vbytep7zip.tar.gz Makefile src linker scripts README patts lzmalib
	
bz:
	# datetex=il_vbytep7zip[$(date +%F_%H_%M_%S)].tar.bz2
	# tar jcvf  "$datetex" Makefile src linker scripts README patts samplePosting
	tar jcvf il_vbytep7zip.tar.bz2 Makefile src linker scripts README patts samplePosting


##  valgrind  --leak-check=full --show-reachable=yes ./myIL ../crr.post crr "samplerate=8;filename=output"
##  valgrind  --leak-check=full --show-reachable=yes ./myILSearch crr I 20 2 "int2=svs;fsearch=dec" < patterns.file
##

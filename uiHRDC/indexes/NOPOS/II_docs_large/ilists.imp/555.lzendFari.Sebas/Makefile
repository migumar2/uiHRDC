SRCDIR = ./src
SRCDIRLINKER = linker
SRCDIRILISTS = ilists
CC          = g++

export CFLAGS  = -O9  -m64     -D_FORTIFY_SOURCE=0 
#export CFLAGS  = -O0  -m64 -g  -D_FORTIFY_SOURCE=0 
#export CFLAGS  = -O9  -m32    # -D_FORTIFY_SOURCE=0 

#DEBUGOPT  =-DDEBUG_ON
DEBUGOPT   =-DDEBUG_OFF 

LIB_IL_INDEX_LZEND = il_lzend.a

all: exes  cleanO

###  builds the index ----------------------------------------
index:  
	@echo "Compiling sources to create the library $(LIB_IL_INDEX_LZEND)"
	@cd $(SRCDIR) && make index
	@cp $(SRCDIR)/$(LIB_IL_INDEX_LZEND) .
	@echo "Compiling finished."
	

###  links the index to obtain the exes ----------------------
exes: index cleanO
	@echo "Now building myIL && myILSearch testing programs"
	$(CC) $(DEBUGOPT) -o myIL $(SRCDIRLINKER)/build_il.c  $(LIB_IL_INDEX_LZEND) $(CFLAGS)		
	$(CC) $(DEBUGOPT) -o myILSearch $(SRCDIRLINKER)/search_il.c $(LIB_IL_INDEX_LZEND)  $(CFLAGS)		

##  ----------------------------------------------------------	 

cleanO: 
	rm -f *.o
	
clean:
	@cd $(SRCDIR) && make clean
	rm -rf *~ *% *.o core *.bak $(LIB_IL_INDEX_LZEND) myIL myILSearch

tar:
	# datetex=il_lzend555[$(date +%F_%H_%M_%S)].tar.gz 
	# tar czvf "$datetex" Makefile src linker libdivsufsort-2.0.1 scripts patts README go*
	tar czvf il_lzend.tar.gz Makefile src linker libdivsufsort-2.0.1 scripts patts README go*
	
bz:
	# datetex=il_lzend555[$(date +%F_%H_%M_%S)].tar.bz2
	# tar jcvf  "$datetex" Makefile src linker libdivsufsort-2.0.1 scripts patts README  go*
	tar jcvf il_lzend.tar.bz2 Makefile src linker libdivsufsort-2.0.1 scripts patts README  go*


##  valgrind  --leak-check=full --show-reachable=yes ./myIL ../crr.post crr "samplerate=8;filename=output"
##  valgrind  --leak-check=full --show-reachable=yes ./myILSearch crr I 20 2 "int2=svs;fsearch=dec" < patterns.file
##

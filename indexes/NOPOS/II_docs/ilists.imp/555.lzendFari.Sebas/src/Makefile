SRCDIR = ./src
CC       = g++
GCC      = g++

ifndef CFLAGS     ##possibly already defined by the "main Makefile".
	CFLAGS     = -O9 -g -m64 -DNDEBUG -D_FORTIFY_SOURCE=0  # -Wall  -DWITHMAIN
endif

#DEBUGOPT  =-DDEBUG_ON
DEBUGOPT   =-DDEBUG_OFF 

OBJS = extractor.o extractor77.o extractorend.o deltacodes.o extractorend2.o LZparser.o LZ77.o utils.o RMQ.o RMQ_succinct.o treelist.o static_bitsequence.o static_bitsequence_brw32.o static_permutation.o static_permutation_mrrr.o static_bitsequence_builder_brw32.o perm.o LZend.nuevo.o LZend.o LZ77_2.o ilist.o

TMPDIR_FOR_AR_COMP = tmptmp.lzend.sebas.2011.01.17.tmp
LIB_IL_INDEX_LZEND = il_5.lzend_64bit.a
#LIB_IL_INDEX_LZEND = il_5.lzend_64bit_debug.a

all: index cleanO

.cpp.o:
	$(CC) $(CFLAGS) -c $<
	
###  builds the index ----------------------------------------
index: libdivsufsort.a $(OBJS)
	@echo "[AR] Building the library: " $(LIBINDEX) 
	@rm -rf $(TMPDIR_FOR_AR_COMP)
	@mkdir $(TMPDIR_FOR_AR_COMP)

	ar rc $(LIB_IL_INDEX_LZEND) $(OBJS)
    
	@echo "[AR] Adding the objects in: libdivsufsort.a" 
	@cp libdivsufsort.a $(TMPDIR_FOR_AR_COMP) ##extract objects from the libdivsufsort.a lib
	@cd $(TMPDIR_FOR_AR_COMP) && ar x libdivsufsort.a

	@ar q $(LIB_IL_INDEX_LZEND) $(TMPDIR_FOR_AR_COMP)/*.o
	@echo "The library $(LIB_IL_INDEX_LZEND) contains now the following objects:" && ar t $(LIB_IL_INDEX_LZEND)

	@rm -rf $(TMPDIR_FOR_AR_COMP)

###  links the index to obtain the exes ----------------------	
###-----------------------------------------------------------

cleanO: 
	rm -f *.o
	
clean:
	rm -f *~ *% *.o core *.bak $(LIB_IL_INDEX_LZEND) 

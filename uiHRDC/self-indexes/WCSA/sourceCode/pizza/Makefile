SRCDIR = ./src
CC          = g++
LIBINDEX	= swcsa.a
SEARCHEXE	= SEARCHwcsa
BUILDEXE	= BUILDwcsa

ifndef CFLAGS     ##possibly already defined by the "main Makefile".
	CFLAGS = -O9  -m64 -D_FORTIFY_SOURCE=0  
	##CFLAGS      = -O9 -m32 
	##CFLAGS      = -g -m32 -O0
endif


all: clean $(BUILDEXE) $(SEARCHEXE) cleanO show
	
$(BUILDEXE): 
	$(CC) $(CFLAGS) -o $(BUILDEXE) build_index.c  $(LIBINDEX)

$(SEARCHEXE): MemoryManagerPIZZA.o BufferedReaderPIZZA.o hashPIZZA.o 
	 $(CC) $(CFLAGS) -o  $(SEARCHEXE) run_queries.c $(LIBINDEX) MemoryManagerPIZZA.o hashPIZZA.o BufferedReaderPIZZA.o  
	 

hashPIZZA.o: MemoryManagerPIZZA.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/hashPIZZA.c 

MemoryManagerPIZZA.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/MemoryManagerPIZZA.c 

BufferedReaderPIZZA.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/BufferedReaderPIZZA.c 


show:
	###
	###type "BUILDwcsa" to create the index and "SEARCHwcsa" to search on it
	###

cleanO: 
	rm -f *.o
	
clean:
	rm -rf *~ *% *.o core *.bak  $(SEARCHEXE) $(BUILDEXE)



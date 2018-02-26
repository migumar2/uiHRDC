SRCDIR = ./src
SRCDIRUTILS = utils
SRCDIRPIZZA = ./pizza

## choose sorting algorithm to build the suffix arrays (uncomment one line)
## larson-sadakane alg.
SRCDIRCSA   = intIndex_larson
## qsort.
#SRCDIRCSA   = intIndex_qsort

CC          = g++

export CFLAGS  = -O9  -m64 -D_FORTIFY_SOURCE=0 
#export CFLAGS      = -O0 -m32 -pg
#export CFLAGS      = -g -m32 -O0

LIBINDEX	= swcsa.a
LIBINTINDEX	= icsa.a


all: clean  wcsa  buildParser buildIntIndex buildStats buildUncompress pizzaMake cleanO

wcsa: intIndexPackage buildFacade.o parameters.o hash.o valstring.o MemoryManager.o basics.o \
		bitmap.o huffDec.o huff.o fileInfo.o #icsa.o 
	ar rc $(LIBINTINDEX) parameters.o buildFacade.o hash.o valstring.o MemoryManager.o basics.o \
		bitmap.o huffDec.o huff.o fileInfo.o 
	#ar q $(LIBINDEX) icsa.o psiHuffmanRLE.o psiDeltaCode.o psiGonzalo.o  ##they are included by icsa.a
	mv $(LIBINTINDEX) $(LIBINDEX)
	cp $(LIBINDEX) $(SRCDIRPIZZA)


buildParser:  wcsa
	$(CC)  $(CFLAGS) -o BUILDPARSER $(SRCDIR)/buildParser.c $(LIBINDEX)	

buildIntIndex:  wcsa
	$(CC)  $(CFLAGS) -o BUILDINTINDEX $(SRCDIR)/buildIntIndex.c $(LIBINDEX)	

buildStats:  wcsa
	$(CC)  $(CFLAGS) -o STATS $(SRCDIR)/buildStats.c $(LIBINDEX)	


buildFacade:  wcsa
	$(CC) -DFACADEWITHMAIN $(CFLAGS) -o BUILDALL $(SRCDIR)/buildAll.c $(LIBINDEX)

buildUncompress:  wcsa
	$(CC) -DFACADEWITHMAIN $(CFLAGS) -o BUILDUNCOMPRESS $(SRCDIR)/buildUncompress.c $(LIBINDEX)


buildFacade.o: parameters.o hash.o valstring.o MemoryManager.o basics.o bitmap.o \
		huffDec.o huff.o fileInfo.o  $(LIBINTINDEX)
	 $(CC) $(CFLAGS) -c  $(SRCDIR)/buildFacade.c 

#######################################################################
#Compiles also search programs in pizza folder

pizzaMake:
	cd  $(SRCDIRPIZZA) && $(MAKE) -w
	echo "[Compiling and linking ./pizza search-programs]"	

################# SELF INDEX ON INTEGERS ##############################
intIndexPackage:
	cd  $(SRCDIR)/$(SRCDIRCSA) && $(MAKE) -w
	@echo "[copying the int-index lib into current dir]"
	@cp $(SRCDIR)/$(SRCDIRCSA)/$(LIBINTINDEX) .

#intIndex: icsa.o parameters.o basics.o huff.o bitmap.o psiHuffmanRLE.o psiDeltaCode.o psiGonzalo.o
#	ar rc $(LIBINTINDEX) icsa.o psiHuffmanRLE.o psiDeltaCode.o psiGonzalo.o  
#			#not including "parameters.o basics.o bitmap.o huff.o" as they are included by wcsa
#			#they are already included into the library by the presentation layer.
#
#icsa.o: parameters.o basics.o bitmap.o huff.o psiHuffmanRLE.o psiDeltaCode.o psiGonzalo.o 
#	 $(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSA)/icsa.c 
#
#psiHuffmanRLE.o: huff.o
#	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSA)/psiHuffmanRLE.c 
#
#psiDeltaCode.o:
#	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSA)/psiDeltaCode.c 
#
#psiGonzalo.o: huff.o
#	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSA)/psiGonzalo.c 
#

####################### UTILS MODULES #################################

parameters.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/parameters.c 

fileInfo.o:  
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/fileInfo.c
	
hash.o: MemoryManager.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/hash.c
 

MemoryManager.o:
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/MemoryManager.c
	
valstring.o:  
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/valstring.c	

huff.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/huff.c

huffDec.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/huffDec.c


basics.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/basics.c

bitmap.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/bitmap.c


############################ CLEANING #################################

cleanO: 
	rm -f *.o
	
clean:
	cd  $(SRCDIR)/$(SRCDIRCSA) && $(MAKE) clean -w
	rm -rf *~ *% *.o core *.bak $(LIBINTINDEX) $(LIBINDEX) $(SRCDIRPIZZA)/$(LIBINDEX) BUILDFACADE BUILDALL BUILDINTINDEX BUILDPARSER STATS



############################ PACKAGING ###############################

tar:
	#datetex=swcsa[$(date +%F_%H_%M_%S)].tar.gz 
	#tar czvf "$datetex"  pizza src Makefile* README various go* 
	tar czvf swcsa.tar.gz pizza src Makefile* README various go* 

## How to 
## valgrind --leak-check=full --show-reachable=yes ./BUILDALL /sitio/textos/CR.TXT ./indexes/cr_index "sA=32;sAinv=32;sPsi=32; nsHuff=16;psiSF=4"


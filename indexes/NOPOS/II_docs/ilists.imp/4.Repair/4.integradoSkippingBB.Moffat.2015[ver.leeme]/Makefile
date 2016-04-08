#Plese note:BUID_INDEX (srcREPAIR/build_Index) does not work if compiled with -m64!
#           Set -m32 to compile it.

CC	= g++
CPP	= g++
export CPPFLAGS = -O9 -m64 -DNDEBUG # -D_FORTIFY_SOURCE=0  -Wall 

SRCDIRLINKER = linker

#DEBUGOPT  =-DDEBUG_ON   ##Writes out logging data (for search_il.c)
DEBUGOPT   =-DDEBUG_OFF 


DIR_REPAIR = ./srcREPAIR
IL_LIST_LIB_A = il_repair.a


all: exes  cleanO sampleK

###  builds the index ----------------------------------------
indexREPAIR: 
	@cd  $(DIR_REPAIR) && $(MAKE) -w

###  links the index to obtain the exes ----------------------
exes: indexREPAIR cleanO
		
	$(CC) $(DEBUGOPT) -o myIL $(SRCDIRLINKER)/build_il.c  $(DIR_REPAIR)/$(IL_LIST_LIB_A) $(CPPFLAGS)		
	$(CC) $(DEBUGOPT) -o myILSearch $(SRCDIRLINKER)/search_il.c $(DIR_REPAIR)/$(IL_LIST_LIB_A) $(CPPFLAGS)	

	cp $(DIR_REPAIR)/$(IL_LIST_LIB_A) il_4.3.1.repair_skipping_moffat_64bit.a	
	
##  ----------------------------------------------------------


sampleK:
	gcc -o sampleK sampleK.c

cleanO: 
	rm -f *.o
	
clean:
	rm -rf *.o  myIL myILSearch
	@cd  $(DIR_REPAIR) && $(MAKE) clean -w

tar:
	# datetex=ilspire07ReducedBitmap[$(date +%F_%H_%M_%S)].tar.gz 
	# tar czvf "$datetex" Makefile src linker scripts README patts samplePosting
	tar czvf 4.3.1.il.integradoSkippingMoffat.tar.gz Makefile src srcREPAIR linker 	 patts go* 
	
bz:
	# datetex=ilspire07ReducedBitmap[$(date +%F_%H_%M_%S)].tar.bz2
	# tar jcvf  "$datetex" Makefile src linker scripts README patts samplePosting
	tar jcvf 4.3.1.il.integradoSkippingMoffat.tar.bz2 Makefile src srcREPAIR linker 	 patts go* 


##  valgrind  --leak-check=full --show-reachable=yes ./myIL ../crr.post crr "samplerate=8;filename=output"
##  valgrind  --leak-check=full --show-reachable=yes ./myILSearch crr I 20 2 "int2=svs;fsearch=dec" < patterns.file
##

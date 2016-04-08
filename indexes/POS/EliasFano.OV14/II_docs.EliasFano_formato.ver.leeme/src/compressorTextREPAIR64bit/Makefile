CPP=g++
CC =g++

LIBCOMPRESSOR = compressorText.a

ifndef CFLAGS     ##possibly already defined by the "main Makefile".
	CFLAGS = -O0 -g  -Wall -DNDEBUG -m64
	##CFLAGS      = -O9 -m32 
	##CFLAGS      = -g -m32 -O0
endif

OBJECTS_REPAIR=RePair/arrayg.o RePair/basicsR.o RePair/hash.o RePair/heap.o RePair/records.o RePair/dictionary.o RePair/IRePair.o
OBJECTS_UTILS=utils/Array.o utils/deltacodes.o utils/cppUtils.o 
OBJECTS=$(OBJECTS_REPAIR) $(OBJECTS_UTILS) interfazTextCompressorDocs.o RePairDocCompressor.o
BINS=test

#OBJECTS=compressor.o bitrankw32int.o  RepairPost.o 
#BINS=test


all: ini cleanO clean $(OBJECTS) lib $(BINS) 
	@echo " [MSG] Done building test"
	#@ar rc $(LIBCOMPRESSOR) $(OBJECTS)
	@echo " [AR ] Library contains files: " && ar t $(LIBCOMPRESSOR)
	@echo " [EXE] check with: ./test <input file text> <input file docbeginings> <output file>"
	@echo "   valgrind   ./test /sitio/textos/FT91.TXT /sitio/textos/FT91.TXT.DOCBOUNDARIES outfilename.txt"
	@echo "\n"
lib:
	@echo " [MSG] Done building test"
	@ar rc $(LIBCOMPRESSOR) $(OBJECTS)
	#@echo " [AR ] Library contains files: " && ar t $(LIBCOMPRESSOR)
	#@echo " [EXE] check with: ./test <input file text> <input file docbeginings> <output file>"
	#@echo "   valgrind   ./test /sitio/textos/FT91.TXT /sitio/textos/FT91.TXT.DOCBOUNDARIES outfilename.txt"
	@echo "\n"

ini:
	@echo " [LIB] Compiling files to create library $(LIBCOMPRESSOR)"
	@echo " [MSG] Compilation options $(CFLAGS)"
	
%.o: %.c 
	@echo " [C] Compiling $<"
	@$(CPP) $(CFLAGS) -c $< -o $@
	
%.o: %.cpp 
	@echo " [C++] Compiling $<"
	@$(CPP) $(CFLAGS) -c $< -o $@

test: test.o
	@echo " [BLD] Building test Compressor (exe): ./test"
	@$(CPP) $(CFLAGS) $(OPT_FLAGS) -o test test.o $(LIBCOMPRESSOR)

testold: test.o
	@echo " [BLD] Building test Compressor (exe): ./test"
	@$(CPP) $(CFLAGS) $(OPT_FLAGS) -o test test.o $(OBJECTS)


clean:
	@echo " [CLN] Removing test & objects"
	@rm -f $(BINS) $(TBINS) 
	@rm -f $(OBJECTS) $(TOBJECTS)
	@rm -f $(LIBCOMPRESSOR)

cleanO:
	@echo " [CLN] Removing objects"
	@rm -f $(OBJECTS) 
	@rm -f test.o


tar:
	@echo " [PACKAGING] Compressing the source code"
	@tar czvf compressorplain.tar.gz *.cpp *.h Makefile  go*
	


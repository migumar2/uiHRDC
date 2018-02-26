CPP=g++
CC =g++

LIBCOMPRESSOR = compressorText.a

ifndef CFLAGS     ##possibly already defined by the "main Makefile".
	CFLAGS = -O3 -Wall -DNDEBUG  -m64
	##CFLAGS      = -O9 -m32 
	##CFLAGS      = -g -m32 -O0
endif

OBJECTS=extractor.o extractor77.o utils.o extractorend.o deltacodes.o extractorend2.o LZparser.o LZ77.o  RMQ.o RMQ_succinct.o treelist.o static_bitsequence.o static_bitsequence_brw32.o static_permutation.o static_permutation_mrrr.o static_bitsequence_builder_brw32.o perm.o LZend.nuevo.o LZend.o LZ77_2.o compressor.o
BINS=testCompressor


LIBCOMPRESSOR = compressorText.a
TMPDIR_FOR_AR_COMP = tmptmp.lzend.sebas.2011.01.17.tmp



all: ini clean $(OBJECTS) lib $(BINS) 
	@echo " [MSG] Done building test"
	@echo " [AR ] Library contains files: " && ar t $(LIBCOMPRESSOR)
	@echo " [EXE] check with: ./test <input file text> <input file docbeginings> <output file>"
	@echo "   valgrind   ./test /sitio/textos/FT91.TXT /sitio/textos/FT91.TXT.DOCBOUNDARIES outfilename.txt"
	@echo "\n"

all_old: $(OBJECTS) $(BINS) 
	@echo "[MSG] Done building test"
	ar rc $(LIBCOMPRESSOR) $(OBJECTS)
	@echo "[AR ] Library contains files: " && ar t $(LIBCOMPRESSOR)


ini:
	@echo " [LIB] Compiling files to create library $(LIBCOMPRESSOR)"
	@echo " [MSG] Compilation options $(CFLAGS)"

lib: $(OBJECTS)
	@echo ""
	@echo "[AR] Building the library: " $(LIBINDEX) 
	@rm -rf $(TMPDIR_FOR_AR_COMP)
	@mkdir $(TMPDIR_FOR_AR_COMP)

	@echo "[AR] Adding LZEND objects"
	@ar rc $(LIBCOMPRESSOR) $(OBJECTS)

	@echo "[AR] Adding the objects in: libdivsufsort.a" 
	@cp libdivsufsort.a $(TMPDIR_FOR_AR_COMP) ##extract objects from the libdivsufsort.a lib
	@cd $(TMPDIR_FOR_AR_COMP) && ar x libdivsufsort.a

	@ar q $(LIBCOMPRESSOR) $(TMPDIR_FOR_AR_COMP)/*.o
	@echo "The library $(LIBCOMPRESSOR) contains now the following objects:" && ar t $(LIBCOMPRESSOR)

	@rm -rf $(TMPDIR_FOR_AR_COMP)


.cpp.o:
	$(CC) $(CFLAGS) -c $<

clean:
	@echo " [CLN] Removing test & objects"
	##rm -f $(BINS) $(LIBCOMPRESSOR) $(OBJECTS) test.o core vgcore* *~
	rm -f $(BINS) $(LIBCOMPRESSOR) $(OBJECTS) test.o core vgcore* *~

.PHONY: sa
sa: 
	make -C sa

testCompressor: test.o $(OBJECTS) sa 
	@echo "[BLD] Building testCompressor (exe): ./testCompressor"
	#ar x libdivsufsort.a
	#$(CC) $(CFLAGS) *.o  -o testCompressor
	#$(CC) $(CFLAGS) *.o libdivsufsort.a -o testCompressor
	$(CC) $(CFLAGS)   -o testCompressor test.cpp $(LIBCOMPRESSOR)
	rm -f $(OBJECTS)

cleanO:
	@echo " [CLN] Removing objects"
	@rm -f $(OBJECTS) 

test:
	@echo "[EXE] check with: ./test <input file text> <input file docbeginings> <output file>"
	@echo "valgrind   ./test /sitio/textos/FT91.TXT /sitio/textos/FT91.TXT.DOCBOUNDARIES outfilename.txt"
	@echo "\n"
	valgrind ./testCompressor /sitio/textos/FT91.TXT /sitio/textos/FT91.TXT.DOCBOUNDARIES outfilename.txt


tar:
	@echo " [PACKAGING] Compressing the source code"
	@tar czvf compressorlzend.tar.gz *.cpp *.h Makefile  go* sa lib*

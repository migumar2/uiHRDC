

SRCDIR      = ./src
SRCDIRUTILS = utils
CC          = g++
#CC          = /usr/bin/g++-4.7     ##NECESARIO PARA QMX_BIS


#CFLAGS      = -O0 -g -m64
export CFLAGS  = -O9 -m64 # -D_FORTIFY_SOURCE=0 
#export CFLAGS   = -O3 -msse4 -mavx -std=c++11  -Weffc++  -ggdb -DDEBUG=1 -D_GLIBCXX_DEBUG -Wall -Wextra 
#export CFLAGS  = -O0 -g -m64 # -D_FORTIFY_SOURCE=0 
#CFLAGS      = -O9 -m64
#CFLAGS      = -O9 -m32
#CFLAGS      = -g -O0 


## The implementation of the POSTING LISTS COMPRESSION structures
LIBS = ./lib

##Temporary dir used for extracting (AR x) the objects in the textCompression.a lib
TMPDIR_FOR_AR = ./tmpdirxxxxxxxxxxxxxxxx2011.01.09_AR

##==========================================================================
## Set the strategy to represent the inverted lists.
##==========================================================================

ifeq ($(INDEX_TYPE),RICE)  ## Golomb_Rice codes without any kind of 2-level sampling. [BUT WITH BITMAPS!]
  #LIBINTINDEX = $(LIBS)/il_1.4golomb.a  
  LIBINTINDEX = $(LIBS)/il_1.4golomb_nosampling_lenlist_64bit.a
	##opts BUILD: "lenBitmapDiv=1" || SEARCH: "int2=merge;intn=svs;fsearch=seqM"
  INDEX_NAME = RICE
else ifeq ($(INDEX_TYPE),MOFFAT_RICE)  ##Rice-codes with Moffat-type sampling [and with BITMAPS]
  #LIBINTINDEX = $(LIBS)/il_1.2.moffatBitmapSpireGolomb.a  
  LIBINTINDEX = $(LIBS)/il_1.2.moffatBitmapSpireGolomb_64bit.a
	##opts BUILD: "samplerate=4;lenBitmapDiv=1" || SEARCH: "int2=svs;intn=svs;fsearch=expM"
  INDEX_NAME = RICE_MOFFAT
else ifeq ($(INDEX_TYPE),SANDERS_RICE)  ##Rice-codes with Sanders-type sampling [and with BITMAPS]
  #LIBINTINDEX = $(LIBS)/il_1.3.sandersBitmapGolomb.a        
  LIBINTINDEX = $(LIBS)/il_1.3.sandersBitmapGolomb_64bit.a
  #LIBINTINDEX = $(LIBS)/il_1.3.sandersBitmapGolomb_64bit_debug.a
	##opts BUILD: "samplerate=4;lenBitmapDiv=1" || SEARCH: "int2=lookup;intn=lookup"
  INDEX_NAME = RICE_SANDERS



else ifeq ($(INDEX_TYPE),VBYTE) ##Vbyte codes without any kind of 2-level sampling. [BUT WITH BITMAPS!]
  # LIBINTINDEX = $(LIBS)/il_2.1.vbyte_bitmap_nosampling_32bit.a
  #LIBINTINDEX = $(LIBS)/il_2.1.vbyte_bitmap_nosampling_64bit.a
  LIBINTINDEX = $(LIBS)/il_2.0.vbyte_64bit.a
  #LIBINTINDEX = $(LIBS)/il_2.1.vbyte_bitmap_nosampling_64bit_debug.a
  #LIBINTINDEX = $(LIBS)/il_2.1.vbyte_nosampling64bit.a
  #LIBINTINDEX = $(LIBS)/il_2.1.vbyte_nosampling.a
  #LIBINTINDEX = $(LIBS)/il_2.1vbyte.a 
	##opts BUILD: "no params" || SEARCH: "int2=merge;intn=svs;fsearch=seqM"
  INDEX_NAME = VBYTE
  
else ifeq ($(INDEX_TYPE),MOFFAT_VBYTE)  ##Vbyte codes with Moffat-type sampling [and with BITMAPS]
  ##LIBINTINDEX = $(LIBS)/il_2moffatBitmapSpirebyteCodes.a  
  LIBINTINDEX = $(LIBS)/il_2.moffatBitmapSpireVbyte_64bit.a
	##opts BUILD: "samplerate=4;lenBitmapDiv=1" || SEARCH: "int2=svs;intn=svs;fsearch=expM"
  INDEX_NAME = VBYTE_MOFFAT
else ifeq ($(INDEX_TYPE),SANDERS_VBYTE)  ##Vbyte codes with Sanders-type sampling [and with BITMAPS]
  #LIBINTINDEX = $(LIBS)/il_3.sandersBitmapbyteCodes.a     
  LIBINTINDEX = $(LIBS)/il_3.sandersBitmapByteCodes_64bit.a
	##opts BUILD: "samplerate=4;lenBitmapDiv=1" || SEARCH: "int2=lookup;intn=lookup"
  INDEX_NAME = VBYTE_SANDERS

else ifeq ($(INDEX_TYPE),VBYTE_LZMA)  ##Vbyte codes + LZMA when it improves compression of lists
  #LIBINTINDEX = $(LIBS)/il_6.vbyte_lzma.a
  #LIBINTINDEX = $(LIBS)/il_6.vbyte_lzma64bit.a   ##header reduced. No dict_size, bytecoded(uncompressed size) 
  LIBINTINDEX = $(LIBS)/il_6.vbyte_lzma_64bit.a  ##header reduced. No dict_size, bytecoded(uncompressed size)
  #LIBINTINDEX = $(LIBS)/il_6.vbyte_lzma_32bit.a  ##header reduced. No dict_size, bytecoded(uncompressed size)
	##opts BUILD: "path2lzmaencoder=./lib/lzmaEncoder; minbcssize=10" || SEARCH: "int2=merge;intn=merge"
  INDEX_NAME = VBYTE_LZMA

else ifeq ($(INDEX_TYPE),REPAIR_SKIPPING)  ##Repair with skipping data to speed up intersections.
  #LIBINTINDEX = $(LIBS)/il_4.3.repair_skipping_32bit.a  ##Repair-skipping, lenPost[] enabled.
  #LIBINTINDEX = $(LIBS)/il_4.3.repair_skipping_64bit.a  ##Repair-skipping, lenPost[] enabled.
  LIBINTINDEX = $(LIBS)/il_4.3.repair_skipping_64bit_2015.a  ##Repair-skipping, lenPost[] enabled.
  
  #LIBINTINDEX = $(LIBS)/il_4.3.repair_skipping_64bit_debug.a  ##Repair-skipping, lenPost[] enabled.
 	##opts BUILD: "filename=outFile; lenBitmapDiv=1; maxMemRepair=500000000; replacesPerIter = 1000; repairBreak=58.0; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap; path2build_index=./lib/build_index32b; path2compress_dictionary=./lib/compressDictionary"
 	##opts SEARCH: "int2=skip;intn=skip"
  INDEX_NAME = REPAIR_SKIP


## SEARCH OPTIONS "SAMPLEK" are set by doing "" echo "sampleK=16" > IDX_BASENAME.const.samplek "" before running the searcher_program
## SEARCH OPTIONS "SAMPLEK" are set by doing "" echo "sampleK=64" > IDX_BASENAME.const.samplek "" before running the searcher_program
else ifeq ($(INDEX_TYPE),REPAIR_SKIPPING_MOFFAT)  ##Repair with skipping data to speed up intersections, AND CM-sampling
  LIBINTINDEX = $(LIBS)/il_4.3.1.repair_skipping_moffat_64bit.a  ##Repair-skipping-CM, lenPost[] enabled.
  #LIBINTINDEX = $(LIBS)/il_4.3.1.repair_skipping_moffat_64bit_debug.a  ##Repair-skipping-CM,, lenPost[] enabled.
 	##opts BUILD: "filename=outFile; lenBitmapDiv=1; maxMemRepair=500000000; replacesPerIter = 1000; repairBreak=58.0; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap; path2build_index=./lib/build_index32b; path2compress_dictionary=./lib/compressDictionary"
 	##opts SEARCH: "int2=skipE;intn=skipE"    ##see note about the "sampleK=16" parameter above.
  INDEX_NAME = REPAIR_SKIP_MOFFAT

## SEARCH OPTIONS "SAMPLEK (B)" are set by doing "" echo "sampleK=16" > IDX_BASENAME.const.samplek "" before running the searcher_program
## SEARCH OPTIONS "SAMPLEK (B)" are set by doing "" echo "sampleK=64" > IDX_BASENAME.const.samplek "" before running the searcher_program
else ifeq ($(INDEX_TYPE),REPAIR_SKIPPING_SANDERS)  ##Repair with skipping data to speed up intersections, AND CM-sampling
  LIBINTINDEX = $(LIBS)/il_4.3.2.repair_skipping_sanders_64bit.a  ##Repair-skipping-lookup, lenPost[] enabled.
  #LIBINTINDEX = $(LIBS)/il_4.3.2.repair_skipping_sanders_64bit_debug.a  ##Repair-skipping-lookup,, lenPost[] enabled.
 	##opts BUILD: "filename=outFile; lenBitmapDiv=1; maxMemRepair=500000000; replacesPerIter = 1000; repairBreak=58.0; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap; path2build_index=./lib/build_index32b; path2compress_dictionary=./lib/compressDictionary"
 	##opts SEARCH: "int2=lookup;intn=lookup"    ##see note about the "sampleK=16" parameter above.
  INDEX_NAME = REPAIR_SKIP_SANDERS




else ifeq ($(INDEX_TYPE),REPAIR_NOSKIPPING)  ##Repair with no skipping data.
  #LIBINTINDEX = $(LIBS)/il_4.2.repair_32bit.a  ##Repair-NO-skipping, lenPost[] enabled.
  LIBINTINDEX = $(LIBS)/il_4.2.repair_64bit.a  ##Repair-NO-skipping, lenPost[] enabled.
  #LIBINTINDEX = $(LIBS)/il_4.2.repair_64bit_debug.a  ##Repair-No-skipping, lenPost[] enabled.
 	##opts BUILD: "filename=outFile; lenBitmapDiv=1; maxMemRepair=500000000; replacesPerIter = 1000; repairBreak=58.0; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap; path2build_index=./lib/build_index32b; path2compress_dictionary=./lib/compressDictionary"
 	##opts SEARCH: "int2=merge;intn=merge"
  INDEX_NAME = REPAIR_NOSKIP

else ifeq ($(INDEX_TYPE),LZEND_IL)  ##lzend compression of inverted lists.
  #LIBINTINDEX = $(LIBS)/il_5.lzend_32bit.a  ##Lzend.
  LIBINTINDEX =  $(LIBS)/il_5.lzend_64bit.a  ##Lzend.
  #LIBINTINDEX =  $(LIBS)/il_5.lzend_64bit_debug.a  ##Lzend.
 	##opts BUILD: "noopts"
 	##opts SEARCH:"noopts"
  INDEX_NAME = LZEND_IL

else ifeq ($(INDEX_TYPE),SIMPLE9)  ##Simple9 encoding
  #LIBINTINDEX = $(LIBS)/il_7.simple9.a
  LIBINTINDEX = $(LIBS)/il_7.simple9_64bit.a
	##opts BUILD: "noparams"
 	##opts SEARCH: "int2=mergeS9;intn=mergeS9"   <-- a 10% faster +-
 	##opts SEARCH: "int2=merge;intn=merge"
  INDEX_NAME = SIMPLE9

else ifeq ($(INDEX_TYPE),SIMPLE9_ESC)  ##Simple9 encoding
  LIBINTINDEX = $(LIBS)/il_7.1.simple9escape.64bit.a
	##opts BUILD: "noparams"
 	##opts SEARCH: "int2=merge;intn=merge"
  INDEX_NAME = SIMPLE9_ESC
  
else ifeq ($(INDEX_TYPE),PFORDELTA)  ## vbyte + pfordelta-opt encoding
  #LIBINTINDEX = $(LIBS)/il_8.pfordelta.a
  LIBINTINDEX = $(LIBS)/il_8.pfordelta_64bit.a
	##opts BUILD: "pfdThreshold=128"
 	##opts SEARCH: "int2=merge;intn=merge"	
  INDEX_NAME = PFORDELTA

else ifeq ($(INDEX_TYPE),QMX_CODING)  ## QMX-CODING (no padding space is included in "size"
  LIBINTINDEX = $(LIBS)/il_9.QMXCODING_64bit.a
	##opts BUILD: "none"
 	##opts SEARCH: "int2=merge;intn=merge"	
  INDEX_NAME = QMXCODING



else 
  LIBINTINDEX = NULL
endif


##==========================================================================
## Set the strategy to compress the text collection.
##==========================================================================

COMPRESSOR_TEXT_LIB = compressorText.a

ifeq ($(TEXT_COMPRESSOR),PLAIN)  
	## The text is kept uncompressed.
	COMPRESSOR_TEXT_LIB_DIR = ./src/compressorTextPlain64bit/
	COMPRESSOR_NAME = PLAIN

else ifeq ($(TEXT_COMPRESSOR),REPAIR) 
	## The text is compressed with Repair
	COMPRESSOR_TEXT_LIB_DIR = ./src/compressorTextREPAIR64bit/
	COMPRESSOR_NAME = REPAIR_T

else ifeq ($(TEXT_COMPRESSOR),LZEND)  
	## The text is compressed with Lzend
	COMPRESSOR_TEXT_LIB_DIR = ./src/compressorTextLZEND64bit/
	COMPRESSOR_NAME = LZEND_T
	
else ifeq ($(TEXT_COMPRESSOR),NOTEXT)  
	## The text is not kept.
	COMPRESSOR_TEXT_LIB_DIR = ./src/compressorTextNull64bit/
	COMPRESSOR_NAME = NOTEXT
	
else 
  COMPRESSOR_NAME = NULL
endif



##==========================================================================

BUILD_NAME =  BUILD_$(INDEX_NAME)_$(COMPRESSOR_NAME)
SEARCH_NAME= SEARCH_$(INDEX_NAME)_$(COMPRESSOR_NAME)

##==========================================================================

## temporal package needed during linking stage.

	LIBINTINDEXTMP	= il_implementation_tmp.a

## the file needed for "linking" directory .. 
LIBINDEX = wordii.a


all: info clean checkOptionsIlList iiblocks buildII buildSearcher cleanO

info:
	@echo "Compiling sources to create a II-words doc-oriented (with non-positional data)"
	@echo "compilation options: $(CFLAGS)"
	@echo ""




checkOptionsIlList:
  ifeq ($(LIBINTINDEX),NULL) 
	$(info )
	$(info "  **********************************************************")
	$(info "  NO IMPLEMENTATION FOR POSTING-LIST COMPRESSION WAS CHOSEN!")
	$(info "  Set (or uncomment in the Makefile) one of the following options")
	$(info "      export INDEX_TYPE=RICE  ")
	$(info "      export INDEX_TYPE=MOFFAT_RICE  ")
	$(info "      export INDEX_TYPE=SANDERS_RICE  ")
	$(info "      export INDEX_TYPE=VBYTE  ")
	$(info "      export INDEX_TYPE=MOFFAT_VBYTE  ")
	$(info "      export INDEX_TYPE=SANDERS_VBYTE  "  )
	$(info "      export INDEX_TYPE=VBYTE_LZMA  "  )
	$(info "      export INDEX_TYPE=REPAIR_SKIPPING  "  )
	$(info "      export INDEX_TYPE=REPAIR_SKIPPING_MOFFAT  "  )
	$(info "      export INDEX_TYPE=REPAIR_SKIPPING_SANDERS  "  )
	$(info "      export INDEX_TYPE=REPAIR_NOSKIPPING  "  )
	$(info "      export INDEX_TYPE=LZEND_IL  "  )
	$(info "      export INDEX_TYPE=SIMPLE9  "  )
	$(info "      export INDEX_TYPE=PFORDELTA  "  )	
	$(info "      export INDEX_TYPE=QMX_CODING  "  )	
	$(info "  ")
	$(info "  ... and re-make.")
	$(info "  **********************************************************")
	$(error "exitting...")
  else 
   $(info ===========================================================)  
  	$(info INDEX_TYPE (inverted lists) set to: $(INDEX_NAME))
  endif

  ifeq ($(TEXT_COMPRESSOR),NULL) 
	$(info )
	$(info "  **********************************************************")
	$(info "  NO IMPLEMENTATION FOR TEXT COMPRESSOR WAS CHOSEN!")
	$(info "  Set (or uncomment in the Makefile) one of the following options")
	$(info "      export TEXT_COMPRESSOR=PLAIN  ")
	$(info "      export TEXT_COMPRESSOR=REPAIR  ")
	$(info "      export TEXT_COMPRESSOR=LZEND  ")
	$(info "  ")
	$(info "  ... and re-make.")
	$(info "  **********************************************************")
	$(error "exitting...")
  else 
   $(info ===========================================================)
  	$(info TEXT_COMPRESSOR was set to: $(TEXT_COMPRESSOR))
  endif  
  
   $(info ===========================================================)
   $(info  COMPILING SOURCES TO CREATE: $(BUILD_NAME) and $(SEARCH_NAME) )
   $(info ===========================================================)


###############################################################

######## MAKE ON THE TEXTCOMPRESSION DIR ######################
compressorText:
	@echo ""
	@echo " [MAK] Building the library: " $(COMPRESSOR_TEXT_LIB_DIR)$(COMPRESSOR_TEXT_LIB)
	@cd  $(COMPRESSOR_TEXT_LIB_DIR) && $(MAKE) -w
	
######## LINKS ALL THE OBJECTS TO CREATE THE II-DOCS INDEX EXES########
iiblocks: buildFacade.o hashWords.o valstring.o basics_wi.o fileInfo.o  mergeoccs2docs.o compressorText
	@echo ""
	@echo "[AR] Building the library: " $(LIBINDEX) 
	@rm -rf $(TMPDIR_FOR_AR)
	@mkdir $(TMPDIR_FOR_AR)
	
	@ar rc $(LIBINDEX) buildFacade.o hashWords.o valstring.o basics_wi.o fileInfo.o mergeoccs2docs.o
	
	@echo "[AR] Adding the objects in: " $(LIBINTINDEX)
	@cp $(LIBINTINDEX) $(TMPDIR_FOR_AR)/$(LIBINTINDEXTMP)   ##extract objects from the inv.list-compression lib.
	@cd $(TMPDIR_FOR_AR) && ar x $(LIBINTINDEXTMP) 


	@echo "[AR] Adding the objects in: " $(COMPRESSOR_TEXT_LIB_DIR)$(COMPRESSOR_TEXT_LIB)
	@cp $(COMPRESSOR_TEXT_LIB_DIR)$(COMPRESSOR_TEXT_LIB) $(TMPDIR_FOR_AR) ##extract objects from the COMPRESSOR_TEXT_LIB
	@cd $(TMPDIR_FOR_AR) && ar x $(COMPRESSOR_TEXT_LIB)
	
	@ar q $(LIBINDEX) $(TMPDIR_FOR_AR)/*.o
	@echo "The library $(LIBINDEX) contains now the following objects:" && ar t $(LIBINDEX)

	@rm -rf $(TMPDIR_FOR_AR)
	

buildII: iiblocks
	@echo "[BLD] Creating the building program: " $(BUILD_NAME)
	@$(CC)  -DFACADEWITHMAIN $(CFLAGS)  -o $(BUILD_NAME) $(SRCDIR)/buildII.c $(LIBINDEX)	

buildSearcher: iiblocks
	@echo "[BLD] Creating the searcher program: " $(SEARCH_NAME)
	@$(CC)  -DSEARCHERFACADEWITHMAIN $(CFLAGS)  -o $(SEARCH_NAME) $(SRCDIR)/buildSearcher.c $(LIBINDEX) 	

###############################################################

buildFacade.o: hashWords.o valstring.o basics_wi.o fileInfo.o  mergeoccs2docs.o
	@echo " [C++] Compiling $(SRCDIR)/buildFacade.c"
	@$(CC) $(CFLAGS)  -c  $(SRCDIR)/buildFacade.c 


fileInfo.o:  
	@echo " [C++] Compiling $(SRCDIR)/$(SRCDIRUTILS)/fileInfo.c"
	@$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/fileInfo.c
	
filterWord.o:  
	@echo " [C++] Compiling $(SRCDIR)/$(SRCDIRUTILS)/filterWord.c"
	@$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/filterWord.c

hashWords.o: 
	@echo " [C++] Compiling $(SRCDIR)/$(SRCDIRUTILS)/hashWords.c"
	@$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/hashWords.c
		
valstring.o:  
	@echo " [C++] Compiling $(SRCDIR)/$(SRCDIRUTILS)/valstring.c"
	@$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/valstring.c	

basics_wi.o: basics_wi.c
	@echo " [C++] Compiling $(SRCDIR)/$(SRCDIRUTILS)/basics_wi.c"
	@$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/basics_wi.c

mergeoccs2docs.o:
	@echo " [C++] Compiling $(SRCDIR)/$(SRCDIRUTILS)/mergeoccs2docs.c"
	@$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/mergeoccs2docs.c

## now unused####
kBitArray.o: basics.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/kBitArray.c
#################

	
basics_wi.c: basics_wi.h
#	touch $(SRCDIR)/$(SRCDIRUTILS)/basics.c	

basics_wi.h: 
#	touch $(SRCDIR)/$(SRCDIRUTILS)/basics_wi.h


cleanO: 
	@echo "[CLN] Cleaning the objects."
	@rm -f *.o
	
clean:
	@echo "[CLN] Cleaning the objects, library, and exes."
	@rm -rf *~ *% *.o core *.bak $(LIBINDEX)  $(BUILD_NAME) $(SEARCH_NAME)

tar:
	#datetex=ii[$(date +%F_%H_%M_%S)].tar.gz 
	#tar czvf "$datetex" Makefile src lib go*
	tar czvf ii.tar.gz src Makefile compile.sh src lib go*


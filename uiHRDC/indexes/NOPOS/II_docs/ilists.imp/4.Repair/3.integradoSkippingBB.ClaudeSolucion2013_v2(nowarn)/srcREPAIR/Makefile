CPP=g++
CC =g++

IL_LIST_LIB_A = il_repair.a

ifndef CPPFLAGS     ##possibly already defined by the "main Makefile".
	CPPFLAGS = -O9  -DNDEBUG -m64 -Wall -D_FORTIFY_SOURCE=0   
	
	#CPPFLAGS=-g3 -Wall
	#CPPFLAGS=-pg -Wall
	#CPPFLAGS=-O9 -Wall -DNDEBUG -m32 -D_FORTIFY_SOURCE=0
	#CPPFLAGS=-g -O0 -Wall -DNDEBUG -m32	
endif


OBJECTS=Heap.o HashTablePos.o HashTablePairs.o RePair.o bitrankw32int.o bitmapw32int.o  RepairPost.o ilistimp.o sort_alg_Rep.o
BINS=build_index build_index_gonzalo compressDictionary convert convertRand build_bitmap rebuildPost search_re 

OBJECTSLIB= bitrankw32int.o bitmapw32int.o RepairPost.o ilistimp.o sort_alg_Rep.o

SRC= ./src

%.o: $(SRC)/%.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

all: clean $(OBJECTS) $(TOBJECTS) $(BINS) buildlib cleanO

buildlib: $(OBJECTSLIB)
	@ar rc $(IL_LIST_LIB_A) $(OBJECTSLIB)

compressDictionary:
	$(CPP) $(CPPFLAGS) -o compressDictionary $(SRC)/compressDictionary.cpp $(OBJECTS)


build_index:
	$(CPP) $(CPPFLAGS) -o build_index $(SRC)/build_index.cpp $(OBJECTS)

build_index_gonzalo:
	$(CPP) $(CPPFLAGS) -o build_index_gonzalo $(SRC)/build_index_gonzalo.cpp $(OBJECTS)

build_bitmap:
	$(CPP) $(CPPFLAGS) -o build_bitmap $(SRC)/build_bitmap.cpp $(OBJECTS)


convert:
	$(CPP) $(CPPFLAGS) -o convert $(SRC)/convert.cpp $(OBJECTS)

convertRand:
	$(CPP) $(CPPFLAGS) -o convertRand $(SRC)/convertRand.cpp $(OBJECTS)

rebuildPost:
	$(CPP) $(CPPFLAGS) -o rebuildPost $(SRC)/rebuildPost.cpp $(OBJECTS)

search_re: 
	$(CPP) $(CPPFLAGS) -o search_re $(SRC)/search_re.cpp $(OBJECTSLIB)

clean:
	rm -f $(BINS) $(TBINS) 
	rm -f $(OBJECTS) $(TOBJECTS)
	rm -f $(IL_LIST_LIB_A)

cleanO:
	rm -f $(OBJECTS) 

tarX: 
	tar czvf rpgraphUNI.tar.gz *.cpp *.h Makefile README scripts
	
	
tar:
	# datetex=rpgraphUNI[$(date +%F_%H_%M_%S)].tar.gz 
	# tar czvf "$datetex"  *.cpp *.h Makefile README scripts
	tar czvf rpgraphUNI.tar.gz *.cpp *.h Makefile README scripts

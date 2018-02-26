CPP=g++
FLAGS=-O9 -DNDEBUG -I ../libcds/includes/
#FLAGS=-O0 -g -DNDEBUG -I ../libcds/includes/

LDFLAGS=-lstdc++
LIB=../libcds/lib/libcds.a

OBJECTS_REPAIR=IRePair/arrayg.o IRePair/basics.o IRePair/dictionary.o IRePair/hash.o IRePair/heap.o IRePair/records.o IRePair/IRePair.o
OBJECTS_BINREL=BinRel/BinaryRelation.o BinRel/LabeledBinaryRelation.o
OBJECTS_UTILS=utils/MemoryManager.o utils/HashWM.o utils/valstring.o utils/WordsManager.o utils/SuffixArray.o utils/deltacodes.o

OBJECTS=$(OBJECTS_REPAIR) $(OBJECTS_BINREL) $(OBJECTS_UTILS) wRePairSLPIndex.o wSLPIndex.o
BIN=wSLPIndex

%.o: %.cpp
	@echo " [C++] Compiling $<"
	@$(CPP) $(CPPFLAGS) $(LDFLAGS) $(FLAGS) -c $< -o $@

all: 	$(OBJECTS) $(BIN)

wSLPIndex:	
	$(CPP) $(CPPFLAGS) $(LDFLAGS) $(FLAGS) -o wSLPIndex $(OBJECTS) ${LIB}

clean:
	@echo " [CLN] Removing object files"
	@rm -f $(OBJECTS) $(BIN) *~

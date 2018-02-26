CPP=g++
FLAGS=-O9 -DNDEBUG -I ../libcds/includes/
LDFLAGS=-lstdc++
LIB=../libcds/lib/libcds.a

OBJECTS_SAMPLINGS=Samplings/VByte2.o Samplings/CSD.o Samplings/CSD_HTFC.o Samplings/sampling.o
OBJECTS_REPAIR=RePair/arrayG.o RePair/basics.o RePair/hash.o RePair/heap.o RePair/records.o RePair/dictionary.o RePair/RePair.o
OBJECTS_UTILS=Utils/SuffixArray.o Utils/deltacodes.o
OBJECTS_BINREL=BinRel/BinaryRelation.o BinRel/LabeledBinaryRelation.o

OBJECTS=$(OBJECTS_SAMPLINGS) $(OBJECTS_REPAIR) $(OBJECTS_UTILS) $(OBJECTS_BINREL) RePairSLPIndex.o SLPIndex.o
BIN=SLPIndex

%.o: %.cpp
	@echo " [C++] Compiling $<"
	@$(CPP) $(CPPFLAGS) $(LDFLAGS) $(FLAGS) -c $< -o $@

all: 	$(OBJECTS) $(BIN)

SLPIndex:	
	$(CPP) $(CPPFLAGS) $(LDFLAGS) $(FLAGS) -o SLPIndex $(OBJECTS) ${LIB}

clean:
	@echo " [CLN] Removing object files"
	@rm -f $(OBJECTS) $(BIN) *~


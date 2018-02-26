CC = g++
#CFLAGS = -g -pg
CFLAGS = -O9 -g -Wall -pedantic
OBJS = utils.o mapper.o static_selfindex.o static_selfindex_none.o static_selfindex_lz77.o LZparser.o LZ77.o LZend.o treelist.o static_bitsequence.o static_bitsequence_brw32.o  static_bitsequence_builder_brw32.o static_permutation.o static_permutation_mrrr.o RMQ.o RMQ_succinct.o perm.o deltacodes.o patricia.o patricia_node.o bp_hb.o dfuds.o static_doublebitmap.o static_doublebitmap_s.o hash.o utils_index.o range.o bitmap.o basics_range.o static_range.o static_range_none.o static_range_chazelle.o static_sequence_wt.o wt_node.o perm2.o static_permutation_mrrr2.o
LIBS = libdivsufsort.a

EXEC = testindex

.cpp.o:
	$(CC) $(CFLAGS) -c $<

all: $(OBJS) $(EXEC)

test: $(OBJS) $(LIBS) test.cpp 
	$(CC) $^ -o $@

testindex: $(OBJS) $(LIBS) test_index.cpp 
	$(CC) $^ -o $@

clean:
	rm -f $(EXEC)

cleanall: clean
	rm -f core vgcore* $(OBJS)  

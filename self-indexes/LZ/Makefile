CC = g++
CFLAGS =-O9 -Wall -DNDEBUG
OBJS = utils.o mapper.o static_selfindex.o static_selfindex_none.o static_selfindex_lz77.o LZparser.o LZ77.o LZend.o treelist.o static_bitsequence.o static_bitsequence_brw32.o  static_bitsequence_builder_brw32.o static_permutation.o static_permutation_mrrr.o RMQ.o RMQ_succinct.o perm.o deltacodes.o patricia.o patricia_node.o bp_hb.o dfuds.o static_doublebitmap.o static_doublebitmap_s.o hash.o utils_index.o range.o static_range.o static_range_none.o static_range_chazelle.o static_sequence_wt.o wt_node.o perm2.o static_permutation_mrrr2.o static_selfindex_lzend.o directcodes.o bitrankw32int.o
LIBS = libdivsufsort.a

EXEC = $(BIN_DIR)/build_lz77 $(BIN_DIR)/load $(BIN_DIR)/extract $(BIN_DIR)/locate $(BIN_DIR)/build_lzend $(BIN_DIR)/exist $(BIN_DIR)/locatemax
EXEC_DIR = tests
BIN_DIR = bin

.cpp.o:
	$(CC) $(CFLAGS) -c $<

all: $(OBJS) $(EXEC)

$(BIN_DIR)/build_none: $(OBJS) $(LIBS) $(EXEC_DIR)/build_noneindex.cpp 
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/build_lz77: $(OBJS) $(LIBS) $(EXEC_DIR)/build_lz77index.cpp 
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/build_lzend: $(OBJS) $(LIBS) $(EXEC_DIR)/build_lzendindex.cpp 
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/load: $(OBJS) $(LIBS) $(EXEC_DIR)/load.cpp 
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/locate: $(OBJS) $(LIBS) $(EXEC_DIR)/locate.cpp 
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/locatemax: $(OBJS) $(LIBS) $(EXEC_DIR)/locate_max.cpp 
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/exist: $(OBJS) $(LIBS) $(EXEC_DIR)/exist.cpp 
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/extract: $(OBJS) $(LIBS) $(EXEC_DIR)/extract.cpp 
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/count: $(OBJS) $(LIBS) $(EXEC_DIR)/count.cpp 
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(EXEC)

cleanall: clean
	rm -f core vgcore* $(OBJS)  

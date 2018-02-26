PROG = lzmadec                          # original version
PROGFARI = lzmadecFARI                  # with some "simplifications" by fari
PROGFARITEST = farilzmaTest				# version by fari.

CXX = g++ 
RM = rm -f


CFLAGS = -c -O9 -m32 -Wall -pedantic -D _LZMA_PROB32 
EFLAGS = -O9  -m32

##CFLAGS = -c -O0 -g -m32 -Wall -pedantic -D _LZMA_PROB32 
##EFLAGS = -O0 -g -m32


OBJS     =  LzmaTest.o     LzmaDecode.o  
OBJSFARI =  LzmaTestFARI.o LzmaDecode.o 

OBJSFARITEST =  LzmaDecode.o  farilzma.o  farilzmaTest.o   ##
OBJSFARILIB =  LzmaDecode.o  farilzma.o                    ##

## all: $(PROG)  $(PROGFARITEST) $(PROGFARI)
all: $(PROGFARITEST) 

$(PROG): $(OBJS)
	$(CXX) $(EFLAGS) -o $(PROG) $(LDFLAGS) $(OBJS) 

$(PROGFARI): $(OBJSFARI)
	$(CXX) $(EFLAGS) -o $(PROGFARI) $(LDFLAGS) $(OBJSFARI) 

$(PROGFARITEST): $(OBJSFARITEST)
	$(CXX) $(EFLAGS) -o $(PROGFARITEST) $(LDFLAGS) $(OBJSFARITEST) 
	ar rc farilzma.a $(OBJSFARILIB)

LzmaTest.o: LzmaTest.c
	$(CXX) $(CFLAGS) LzmaTest.c

LzmaTestFARI.o: LzmaTestFARI.c
	$(CXX) $(CFLAGS) LzmaTestFARI.c


####

farilzmaTest.o: farilzmaTest.c
	$(CXX) $(CFLAGS) farilzmaTest.c

farilzma.o: 
	$(CXX) $(CFLAGS) farilzma.c

####

LzmaDecode.o: LzmaDecode.c
	$(CXX) $(CFLAGS) LzmaDecode.c

tar:
	tar czvf lzmadec.tar.gz  *.c *.h Makefile* makefile* go*
	
	
clean:
	-$(RM) $(PROG) $(OBJS) $(OBJSFARI) $(OBJSFARITEST)


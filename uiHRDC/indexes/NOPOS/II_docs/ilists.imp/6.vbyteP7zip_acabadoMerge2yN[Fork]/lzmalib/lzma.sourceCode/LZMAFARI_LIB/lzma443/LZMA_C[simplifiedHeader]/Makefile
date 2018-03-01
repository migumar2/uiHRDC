PROG = lzmadec                          # original version
PROGFARI = lzmadecFARI                  # with some "simplifications" by fari
PROGFARITEST = farilzmaTest				# version by fari.

CXX = g++ 
RM = rm -f


ifndef CMODE     ##possibly already defined by the "main Makefile".
	CMODE = -m32
	##CFLAGS      = -O9 -m32 
	##CFLAGS      = -g -m32 -O0
endif

CFLAGS = -c -O9 -Wall -pedantic -D _LZMA_PROB32 
EFLAGS =    -O9 

#CFLAGS = -c -O9 -m32 -Wall -pedantic -D _LZMA_PROB32 
#EFLAGS =    -O9 -m32

##CFLAGS = -c -O0 -g -m32 -Wall -pedantic -D _LZMA_PROB32 
##EFLAGS = -O0 -g -m32


OBJS     =  LzmaTest.o     LzmaDecode.o  
OBJSFARI =  LzmaTestFARI.o LzmaDecode.o 

OBJSFARITEST =  LzmaDecode.o  farilzma.o  farilzmaTest.o   ##
OBJSFARILIB =  LzmaDecode.o  farilzma.o                    ##

## all: $(PROG)  $(PROGFARITEST) $(PROGFARI)
all: clean $(PROGFARITEST) 

$(PROG): $(OBJS)
	$(CXX) $(EFLAGS) $(CMODE) -o $(PROG) $(LDFLAGS) $(OBJS) 

$(PROGFARI): $(OBJSFARI)
	$(CXX) $(EFLAGS) $(CMODE) -o $(PROGFARI) $(LDFLAGS) $(OBJSFARI) 

$(PROGFARITEST): $(OBJSFARITEST)
	$(CXX) $(EFLAGS) $(CMODE) -o $(PROGFARITEST) $(LDFLAGS) $(OBJSFARITEST) 
	ar rc farilzma.a $(OBJSFARILIB)

LzmaTest.o: LzmaTest.c
	$(CXX) $(CFLAGS) $(CMODE) LzmaTest.c

LzmaTestFARI.o: LzmaTestFARI.c
	$(CXX) $(CFLAGS) $(CMODE) LzmaTestFARI.c


####

farilzmaTest.o: farilzmaTest.c
	$(CXX) $(CFLAGS) $(CMODE) farilzmaTest.c

farilzma.o: 
	$(CXX) $(CFLAGS) $(CMODE) farilzma.c

####

LzmaDecode.o: LzmaDecode.c
	$(CXX) $(CFLAGS) $(CMODE) LzmaDecode.c

tar:
	tar czvf lzmadec.tar.gz  *.c *.h Makefile* makefile* go*
	
	
clean:
	-$(RM) $(PROG) $(OBJS) $(OBJSFARI) $(OBJSFARITEST)


CPP=gcc
CPPFLAGS=-O9 -DNDEBUG -m32 -w
#CPPFLAGS=-g3 -Wall -m32 -w
#OBJECTS=genIntervalsChars.o genIntervalsWords.o
BINS=genIntervalsChars genIntervalsWords genIntervalsCharsFraction genIntervalsWordsFraction

LAB1=" [C++]"
LAB2=" [BLD]"
LAB3=" [CLN]"
LAB4=" [DST]"

%.o: %.c
	@echo "$(LAB1) $<"
	@$(CPP) $(CPPFLAGS) -c $< -o $@

all: clean $(BINS)

genIntervalsChars:
	@echo "$(LAB2) Building genIntervalsChars"
	@$(CPP) $(CPPFLAGS) -o genIntervalsChars genIntervalsChars.c 

genIntervalsCharsFraction:
	@echo "$(LAB2) Building genIntervalsCharsFraction"
	@$(CPP) $(CPPFLAGS) -o genIntervalsCharsFraction genIntervalsCharsFraction.c 

genIntervalsWords:
	@echo "$(LAB2) Building genIntervalsWords"
	@$(CPP) $(CPPFLAGS) -o genIntervalsWords genIntervalsWords.c 

genIntervalsWordsFraction:
	@echo "$(LAB2) Building genIntervalsWordsFraction"
	@$(CPP) $(CPPFLAGS) -o genIntervalsWordsFraction genIntervalsWordsFraction.c 


clean:
	@echo "$(LAB3) Cleaning binary files"
	@rm -f $(BINS) $(TBINS) 
	@echo "$(LAB3) Cleaning object files"
	@rm -f $(OBJECTS) $(TOBJECTS)


tar: 
	@echo "$(LAB4) Generating genIntervals.tar.gz"
	@tar czf genIntervalsCharWords.tar.gz *.c Makefile README.chars README.words 



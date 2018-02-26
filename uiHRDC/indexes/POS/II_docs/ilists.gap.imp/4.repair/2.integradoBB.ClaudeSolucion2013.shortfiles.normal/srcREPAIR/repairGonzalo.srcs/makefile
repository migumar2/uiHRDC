CFLAGS =  -O3


#all: repair despair irepair idespair toint repair.tgz
all: irepair idespair repair.tgz clean0

repair.tgz: *.c *.h makefile COPYRIGHT README bal/*.c bal/*.h bal/makefile bal/README 
	tar -czvf repair.tgz *.c *.h makefile COPYRIGHT README bal/*.c bal/*.h bal/makefile bal/README

repair: repair.o array.o hash.o heap.o records.o basics.o makefile
	gcc $(CFLAGS) -o repair repair.o array.o hash.o heap.o records.o basics.o

repair.o: repair.c array.h hash.h heap.h records.h basics.h makefile
	gcc $(CFLAGS) -c repair.c

despair: despair.o basics.o makefile
	gcc $(CFLAGS) -o despair despair.o basics.o

despair.o: despair.c basics.h makefile
	gcc $(CFLAGS) -c despair.c

irepair: irepair.o array.o hash.o heap.o records.o basics.o makefile
	gcc $(CFLAGS) -o irepair irepair.o array.o hash.o heap.o records.o basics.o

irepair.o: irepair.c array.h hash.h heap.h records.h basics.h makefile
	gcc $(CFLAGS) -c irepair.c

idespair: idespair.o basics.o makefile
	gcc $(CFLAGS) -o idespair idespair.o basics.o

idespair.o: idespair.c basics.h makefile
	gcc $(CFLAGS) -c idespair.c

array.o: array.c array.h hash.h heap.h records.h basics.h makefile
	gcc $(CFLAGS) -c array.c

hash.o: hash.c array.h hash.h heap.h records.h basics.h makefile
	gcc $(CFLAGS) -c hash.c

heap.o: heap.c array.h hash.h heap.h records.h basics.h makefile
	gcc $(CFLAGS) -c heap.c

records.o: records.c array.h hash.h heap.h records.h basics.h makefile
	gcc $(CFLAGS) -c records.c

basics.o: basics.c basics.h makefile
	gcc $(CFLAGS) -c basics.c

toint: toint.c makefile
	gcc $(CFLAGS) -o toint toint.c
	
clean:
	rm -f repair despair irepair idespair toint *.o

clean0:
	rm -f *.o

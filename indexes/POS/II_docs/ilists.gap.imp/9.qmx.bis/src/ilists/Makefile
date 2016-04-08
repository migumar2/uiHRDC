
#
#	OS X and Linux Makefile
#

all: clean test	
	echo "COMPILANDO TODO"

test: compress_qmx.o
	g++ -O3 -msse4 qmx_test.c -DENCODE_DIFFERENCES compress_qmx.o -o test  
	#g++ -O3 -msse4 qmx_test.c  compress_qmx.o -o test  

compress_qmx :
	g++ -O3 -msse4 compress_qmx.c -o compress_qmx

compress_qmx.o:
	 g++ -O3  -msse4 compress_qmx.c -c 

clean :
	rm -f compress_qmx test *.o


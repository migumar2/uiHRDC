
#
# lzmaEncoder must be compiled in 32bit mode (default)
#
echo    "---------------------------------------------"
echo    "Compiling to obtain the program -LZMAencoder-"
echo    "---------------------------------------------"
        cd LZMAFARI_LIB/lzma465/CPP/7zip/Compress/LZMA_Alone
        make clean
        make            ##Makefile is prepared to generate 32-bit code.
	cd ../../../../../..
	cp LZMAFARI_LIB/lzma465/CPP/7zip/Compress/LZMA_Alone/lzma  lzmaEncoder
        #mv lzm LZMAencoder  ## THIS IS THE PROGRAM YOU NEED FOR THE API.



#
#  farilzma.a must be compiled in 64bit mode (-m64)
#  farilzmaTest exe, must be compiled in 32bit mode (-m32)
#  CMODE enviroment variable can be set properly before compilation.
#

echo    "---------------------------------------------------------------------"
echo    " Compiling To build a test -uncompressLZMA- and farilzmaTest software" 
echo    "---------------------------------------------------------------------"
export CMODE=-m32

        cd LZMAFARI_LIB/lzma443/
	ln -s LZMA_C\[simplifiedHeader\] LZMA_C
        cd LZMA_C
        # make clean
        make            ##Makefile uses $(CMODE) if defined, -m32 otherwise
	cd ../../..
	cp LZMAFARI_LIB/lzma443/LZMA_C/farilzma.a ./farilzma32.a
	cp LZMAFARI_LIB/lzma443/LZMA_C/farilzmaTest .


echo    "-------------------------------------------------------------------------"
echo    " Compiling to obtain package (farilzma.a) --> farilzma_simplified64bit.a-"
echo    "-------------------------------------------------------------------------"
export CMODE=-m64

        cd LZMAFARI_LIB/lzma443/
	ln -s LZMA_C\[simplifiedHeader\] LZMA_C
        cd LZMA_C
        # make clean
        make            ##Makefile uses $(CMODE) if defined, -m32 otherwise
	cd ../../..
	cp LZMAFARI_LIB/lzma443/LZMA_C/farilzma.a ./farilzma64.a
	#cp LZMAFARI_LIB/lzma443/LZMA_C/farilzmaTest .


unset CMODE

cp lzmaEncoder ..
cp farilzma32.a ../farilzma_simplified.a
cp farilzma64.a ../farilzma_simplified64bit.a


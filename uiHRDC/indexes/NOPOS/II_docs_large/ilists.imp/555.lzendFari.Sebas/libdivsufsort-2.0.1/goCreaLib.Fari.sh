make clean
make

cd lib
ar r libdivsufsort.a *.o
mv libdivsufsort.a ..
cd ..


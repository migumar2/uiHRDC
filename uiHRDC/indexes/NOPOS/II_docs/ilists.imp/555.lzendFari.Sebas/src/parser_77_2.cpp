#include "LZ77_2.h"
#include "LZend.h"
#include <iostream>

int main(int argc, char** argv){
    LZparser* p;
    p = new LZ77_2(argv[1]);
    unsigned int f = p->parse();
    std::cout<<"Parsed to: "<<f<<" factors"<<std::endl;
    std::cout<<"Bits needed:"<<p->bits_size<<std::endl;
    delete p;
    return EXIT_SUCCESS;
}

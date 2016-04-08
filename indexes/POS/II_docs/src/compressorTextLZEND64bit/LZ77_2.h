#ifndef __LZ77_2_H__
#define __LZ77_2_H__

#include "LZparser.h"
#include "RMQ.h"

class LZ77_2: public LZparser{
    public:
        LZ77_2(char* filename);
        virtual ~LZ77_2();
        /* generates the LZ parsing of filename and returns the number of phrases
         * creates 3 different files (arrays):
         *    filename.start (4*len bytes)   
         *    filename.len   (4*len bytes)
         *    filename.char  (1*len bytes) */
        unsigned int parse();
        unsigned int add_bits(unsigned int pos,unsigned int len);
    protected:
        unsigned int* sa;
        unsigned int* C;
        RMQ* rmq;
        void findInterval(unsigned int* start, unsigned int* end, unsigned char chr,unsigned int offset);
        unsigned int bsRight(unsigned int start, unsigned int end, unsigned char chr, unsigned int offset);
        unsigned int bsLeft(unsigned int start, unsigned int end, unsigned char chr, unsigned int offset);
};
#endif

#ifndef __LZ77_H__
#define __LZ77_H__

#include "LZparser.h"
#include "RMQ.h"

namespace lz77index{
namespace parsing{

class LZ77: public LZparser{
    public:
        LZ77(const char* filename);
        ~LZ77();
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

}
}
#endif

#ifndef __LZPARSER_H__
#define __LZPARSER_H__

#include <cstdio>

namespace lz77index{
namespace parsing{

class LZparser{
    public:
        LZparser(const char* filename);
        virtual ~LZparser();
        /* generates the LZ parsing of filename and returns the number of phrases
         * creates 3 different files (arrays):
         *    filename.start (4*len bytes)   
         *    filename.len   (4*len bytes)
         *    filename.char  (1*len bytes) */
        virtual unsigned int parse()=0;
        virtual unsigned int add_bits(unsigned int pos,unsigned int len)=0;
        unsigned int bits_size;
    protected:
        const char* filename;
        FILE* fp_start;
        FILE* fp_len;
        FILE* fp_char;
        unsigned char* text;
        unsigned int text_len;
        unsigned int factors;
        unsigned int getSigma();
        unsigned int sigma;
        unsigned int report(unsigned int start, unsigned int len, unsigned char chr, unsigned int pos);
        //void init(char* filename);
        void close();
        unsigned int log(unsigned int x);
        unsigned int log_sigma;
        unsigned int log_n;
};

}
}
#endif

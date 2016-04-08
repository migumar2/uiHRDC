#ifndef __EXTRACTOR_H__
#define __EXTRACTOR_H__

//#include "basics.h"
//#include "bitmap.h"
#include "deltacodes.h"

class Extractor{
    public:
        Extractor(char* filename, unsigned int sampling);//sampling for deltacoding
        virtual ~Extractor();
        virtual unsigned char* extract(unsigned int start, unsigned int end);
        virtual unsigned char* extractAll();
        virtual unsigned char getChar(unsigned int pos);
        virtual unsigned int size();
        unsigned int bits_size;
    protected:
        unsigned int text_len;
        unsigned int len;
        unsigned int* sources;
        unsigned char* chars;
        unsigned int* plen;
        unsigned int selects;
        DeltaCodes* dc;
        char* filename;
        unsigned int log(unsigned int x);
        unsigned int getSigma();
 
};
#endif

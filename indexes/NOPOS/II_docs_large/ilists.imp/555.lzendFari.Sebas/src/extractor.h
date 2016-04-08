#ifndef __EXTRACTOR_H__
#define __EXTRACTOR_H__

//#include "basics.h"
//#include "bitmap.h"
#include "deltacodes.h"
#include "basics.h"  //*añadido por fari 2013, para get_fieldLL incluido en subclases.

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
        unsigned char* chars;
        
        unsigned int* sources;
        unsigned int sourcesbits;  //bits per element in sources.
        unsigned int sourcesnints; //num of integers needed to represent any value from (distinct number of source values).
        
        unsigned int* plen;
        unsigned int selects;
        DeltaCodes* dc;
        char* filename;
        unsigned int log(unsigned int x);
        unsigned int getSigma();
 
};
#endif

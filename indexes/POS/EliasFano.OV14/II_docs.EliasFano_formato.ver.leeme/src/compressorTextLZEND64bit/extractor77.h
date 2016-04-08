#ifndef __EXTRACTOR77_H__
#define __EXTRACTOR77_H__

#include "extractor.h"

class Extractor77: public Extractor{
    public:
        Extractor77(char* filename, unsigned int sampling);
        ~Extractor77();
        virtual unsigned char* extract(unsigned int start, unsigned int end);
    protected:
        void charextract(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end);
        void charextract2(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end);
};

#endif

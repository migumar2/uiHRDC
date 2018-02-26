#ifndef __EXTRACTOREND2_H__
#define __EXTRACTOREND2_H__

#include "extractor.h"

class ExtractorEnd2: public Extractor{
    public:
        ExtractorEnd2(char* filename, unsigned int sampling);
        ~ExtractorEnd2();
        virtual unsigned char* extract(unsigned int start, unsigned int end);
        void charextract(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end);     
    protected:
        void charextract2(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end);
};

#endif

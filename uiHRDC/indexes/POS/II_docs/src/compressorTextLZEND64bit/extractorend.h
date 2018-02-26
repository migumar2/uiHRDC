#ifndef __EXTRACTOREND_H__
#define __EXTRACTOREND_H__

#include "extractor.h"

class ExtractorEnd: public Extractor{
    public:
        ExtractorEnd(char* filename, unsigned int sampling);
        ~ExtractorEnd();
        virtual unsigned char* extract(unsigned int start, unsigned int end);
    protected:
        void charextract(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end);     
        void charextract2(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end);
        unsigned int* sources_id;
};

#endif

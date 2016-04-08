#ifndef __EXTRACTOREND2_H__
#define __EXTRACTOREND2_H__

#include <stdlib.h>
#include "extractor.h"

class ExtractorEnd2: public Extractor{
    public:
        ExtractorEnd2(char* filename, unsigned int sampling);
        ~ExtractorEnd2();
        virtual unsigned char* extract(unsigned int start, unsigned int end);
        void charextract(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end);     
	
	unsigned int get_len(); /**fari 2013, only for statistics in ilist.cpp **/
	unsigned int get_sourcesbits_numbytes(); /**fari 2013, only for statistics in ilist.cpp **/
	unsigned int get_sourcesbitsperValue(); /**fari 2013, only for statistics in ilist.cpp **/
	DeltaCodes* get_DC();   /**fari 2013, only for statistics in ilist.cpp **/
	
    protected:
        void charextract2(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end);
};

#endif

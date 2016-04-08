#ifndef __LZend_H__
#define __LZend_H__

#include "LZ77.h"
#include "RMQ.h"

class LZend: public LZparser{
    public:
        LZend(char* filename);
        virtual ~LZend();
        /* generates the LZ parsing of filename and returns the number of phrases
         * creates 3 different files (arrays):
         *    filename.start (4*len bytes)   
         *    filename.len   (4*len bytes)
         *    filename.char  (1*len bytes) */
        unsigned int parse();
    private:
        unsigned int* inverseSA;
        unsigned int getInverseSA(unsigned int pos);
        int* sa;
        RMQ* rmq;
        unsigned int* sai;
        unsigned int* C;
        //unsigned char* L;
        int bucket_size;
        unsigned int* precomputed;
        void get_rows(int* nmin,int* nmax,int first,int last,unsigned char c,unsigned int* C);
        unsigned int Occ(unsigned char c,int pos);
        unsigned char L(int pos);
        unsigned int* precomputeOcc(int bucket_size,unsigned int len);
        //void check();
};

int getMax(unsigned int* sa, int start, int end);
#endif

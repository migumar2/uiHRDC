#ifndef __LZEND2_H__
#define __LZEND2_H__

#include "LZ77.h"
#include "RMQ.h"
#include "static_permutation.h"

class LZEnd2: public LZparser{
    public:
        LZEnd2(char* filename);
        virtual ~LZEnd2();
        /* generates the LZ parsing of filename and returns the number of phrases
         * creates 3 different files (arrays):
         *    filename.start (4*len bytes)   
         *    filename.len   (4*len bytes)
         *    filename.char  (1*len bytes) */
        unsigned int parse();
        unsigned int add_bits(unsigned int pos,unsigned int len);
    private:
        unsigned int* inverseSA;
        unsigned int SA(unsigned int pos);
        unsigned int SAI(unsigned int pos);
        int* sa;
        static_permutation* pi;
        RMQ* rmq;
        /*unsigned int* sai;*/
        unsigned int* C;
        unsigned char* La;
        int bucket_size;
        unsigned int* precomputed;
        unsigned int* precomputed1;
        unsigned short int* precomputed2;
        void get_rows(int* nmin,int* nmax,int first,int last,unsigned char c,unsigned int* C);
        unsigned int Occ(unsigned char c,int pos);
        unsigned char L(int pos);
        void precomputeOcc(unsigned int** precomputed1,unsigned short int**precomputed2,unsigned int len);
        unsigned int *precomputeOcc2(int bucket_size,unsigned int len);
        unsigned int Occ2(unsigned char c,int pos);
        //void check();
};

int getMax(unsigned int* sa, int start, int end);
#endif

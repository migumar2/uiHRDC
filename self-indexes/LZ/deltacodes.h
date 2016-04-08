#include <cstdio>

#ifndef __DELTACODES_H__
#define __DELTACODES_H__

namespace lz77index{

class DeltaCodes{
    public:
        DeltaCodes(unsigned int* array, unsigned int n, unsigned int sampling);
        ~DeltaCodes();
        unsigned int select(unsigned int total);
        /** computes select(total) and stores the next delta*/
        unsigned int select2(unsigned int total,unsigned int* delta);
        /** returns kind of select_0: it is actually the position of the one to the left of select_0(total) and the number of ones */
        unsigned int select0_rank1(unsigned int total,unsigned int* rank);
        unsigned int rank(unsigned int pos);
        unsigned int access(unsigned int pos);
        unsigned int rank_select(unsigned int pos, unsigned int* select);
        unsigned int size();
        unsigned int save(FILE* fp);
        static DeltaCodes* load(FILE* fp);
    private:
        unsigned int* deltacodes;
        unsigned int* sampled_pointer;
        unsigned int* sampled_total;
        unsigned int n;
        unsigned int sampling;
        unsigned int total_size;
        unsigned int decodeDelta(unsigned int* deltaCodes, unsigned int* pos);
        void encodeDelta(unsigned int val, unsigned int* deltaCodes, unsigned int* pos);
        unsigned int bits(unsigned int n);
        DeltaCodes();
};

}

#endif

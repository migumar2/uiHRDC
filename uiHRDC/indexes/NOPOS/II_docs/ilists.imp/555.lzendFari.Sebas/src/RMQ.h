#ifndef __RMQ_H__
#define __RMQ_H__
#include <iostream>
#include <cstdio>

#include "RMQ_succinct.h"

class RMQ{
    public:
        RMQ(int* array, unsigned int length);
        ~RMQ();
        unsigned int minpos(unsigned int start, unsigned int end);
        /*unsigned int save(FILE* fp);
        RMQ* load(FILE* fp);*/
    private: 
        RMQ();
        RMQ_succinct *RMQ_s;
        int* array;
        unsigned int length;
};
#endif

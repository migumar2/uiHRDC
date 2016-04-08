//RMQ_fischer.cpp
#include "RMQ.h"
#include <iostream>

RMQ::RMQ(int* array, unsigned int length){
    this->RMQ_s = NULL;
    if(length>=200){
        this->RMQ_s = new RMQ_succinct(array,length);
    }
    this->array = array;
    this->length = length;
}
RMQ::RMQ(){
    this->RMQ_s = NULL;
    this->array=NULL;
    this->length=0;
}
RMQ::~RMQ(){
    if(RMQ_s!=NULL)delete RMQ_s;
}
unsigned int RMQ::minpos(unsigned int start, unsigned int end){
    unsigned int minpos;
    if(end>=this->length || start<0)return ~0;
    if(this->RMQ_s!=NULL && end-start>=50)
        return this->RMQ_s->query(start, end);
    minpos = start;
    for(unsigned i=start+1;i<=end;i++){
        if(this->array[i]<this->array[minpos])minpos=i;
    }    
    return minpos;
}
/*unsigned int RMQ::save(FILE* fp){
    return 0;
}
RMQ* RMQ::load(FILE* fp){
    return NULL;
}*/

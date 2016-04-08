#include "LZ77_2.h"
#include <stdlib.h>

#include "utils.h"

LZ77_2::LZ77_2(char* filename):LZparser(filename){
    //this->init(filename);
    this->sa = NULL; 
    int sa_len;
    startTime();
    this->sa = getSA(this->text,this->text_len,filename,&sa_len);
    endTime();
    this->C = getC(this->sa,this->text,this->text_len);
    rmq = new RMQ((int*)this->sa,this->text_len);
}
LZ77_2::~LZ77_2(){
    delete rmq;
    if(this->sa!=NULL)
        free(this->sa);
    if(this->C!=NULL)
        free(this->C);
}
/*unsigned int rmq(unsigned int* sa, unsigned int start, unsigned int end){
    unsigned int min_pos=start;
    for(unsigned int i=start+1;i<=end;i++)
        if(sa[i]<sa[min_pos])min_pos=i;
    return min_pos;
}*/
/* generates the LZ parsing of filename and returns the number of phrases
 * creates 3 different files (arrays):
 *    filename.start (4*len bytes)   
 *    filename.len   (4*len bytes)
 *    filename.char  (1*len bytes) */
unsigned int LZ77_2::parse(){
    /*report(1,2,(unsigned char)'a');
    std::cout<<this->filename<<std::endl;
    this->close();
    return 1;*/
    startTime();
    unsigned int factor_pos=0;
    unsigned int factor_len;
    unsigned int a_factor_len;
    unsigned int source_start;
    unsigned int start,end, min;
    while(factor_pos<this->text_len){
        a_factor_len = 0;
        factor_len = 0;
        start = 1;
        end = 0;
        source_start = 0;
        do{
            findInterval(&start, &end, this->text[factor_pos+a_factor_len],a_factor_len);
            a_factor_len++;
            if(a_factor_len<=2)factor_len = a_factor_len;
            else if(a_factor_len%2 == 1 && a_factor_len/2 == factor_len - 1)factor_len = a_factor_len;
            min = sa[rmq->minpos(start,end)];
            if(start>end || min>=factor_pos || min+a_factor_len>factor_pos)break;//revisar la ultima condicion: es mayor o mayor o igual?
            source_start = min;
        }while(start<=end);
        this->report(source_start,factor_len,this->text[factor_pos+factor_len-1],factor_pos);
        factor_pos+=factor_len;
        printf("Flen:%d %d\n",factor_len,factor_pos);
    }
    this->close();
    endTime();
    return this->factors;
}
void LZ77_2::findInterval(unsigned int* start, unsigned int* end, unsigned char chr,unsigned int offset){
    if(*end<*start){
        *start = this->C[chr];
        *end = this->C[chr+1]-1;
    }else{
        *start = bsLeft(*start,*end,chr,offset);
        *end = bsRight(*start,*end,chr,offset);
    }
}
unsigned int LZ77_2::add_bits(unsigned int pos, unsigned int len){
    unsigned int b=0;
    b += this->log_sigma;//trailing character
    b += this->log(pos);//start of source
    b += 5;//len of source 
    return b;
}
unsigned int LZ77_2::bsRight(unsigned int start, unsigned int end, unsigned char chr, unsigned int offset){
    unsigned int low = start, high = end;
    while(low < high){
        unsigned int mid = (low + high + 1)/2;
        unsigned char midval = this->text[this->sa[mid]+offset];
        if(midval < chr){
            low = mid + 1;
        }else if(midval > chr){
            high = mid - 1;
        }else{ //midVal == c
            low = mid;
        }
    }
    if(this->text[this->sa[low]+offset]==chr)return low;
    return start;
}
unsigned int LZ77_2::bsLeft(unsigned int start, unsigned int end, unsigned char chr, unsigned int offset){
    unsigned int low = start, high = end;
    while(low < high){
        unsigned int mid = (low + high)/2;
        unsigned char midval = this->text[this->sa[mid]+offset];
        if(midval < chr){
            low = mid + 1;
        }else if(midval > chr){
            high = mid - 1;
        }else{ //midVal == c
            high = mid;
        }
    }
    if(this->text[this->sa[low]+offset]==chr)return low;
    return end+1;
}

#include "LZparser.h"
#include <cstring>
#include <iostream>

#include "utils.h"

namespace lz77index{
namespace parsing{

LZparser::~LZparser(){
    if(this->text!=NULL)
        delete [] this->text;
}

LZparser::LZparser(const char* filename){
    this->filename = filename;
    this->text = utils::readText(filename,&(this->text_len));
    char tmp[2048];
    strncpy(tmp, filename, 2048);
    strncat(tmp, ".start", 2048);
    this->fp_start = fopen(tmp, "w");
    strncpy(tmp, filename, 2048);
    strncat(tmp, ".len", 2048);
    this->fp_len = fopen(tmp, "w");
    strncpy(tmp, filename, 2048);
    strncat(tmp, ".char", 2048);
    this->fp_char = fopen(tmp, "w");
    this->factors = 0;
    this->bits_size = 3;
    this->sigma = this->getSigma();
    this->log_sigma = this->log(this->sigma-1);
    if(this->log_sigma!=8)this->bits_size += 8*this->sigma;
    this->log_n = this->log(this->text_len);
}

unsigned int LZparser::getSigma(){
    unsigned int sigma=0;    
    unsigned int count[256];
    for(unsigned int i=0;i<256;i++)count[i]=0;    
    for(unsigned int i=0;i<this->text_len;i++){
        count[text[i]]+=1;
    }
    for(unsigned int i=0;i<256;i++)if(count[i]!=0)sigma++;
    return sigma;
}
unsigned int LZparser::report(unsigned int start, unsigned int len, unsigned char chr, unsigned int pos){
    int res;
    res = fwrite(&start,sizeof(unsigned int),1,this->fp_start);
    if(res!=1)return 1;
    res = fwrite(&len,sizeof(unsigned int),1,this->fp_len);
    if(res!=1)return 1;
    res = fwrite(&chr,sizeof(unsigned char),1,this->fp_char);
    if(res!=1)return 1;
    this->factors++;
    this->bits_size += this->add_bits(pos,len);
    //std::cout<<start<<" "<<len<<" "<<chr<<std::endl;
    return 0;
}

void LZparser::close(){
    fclose(this->fp_start);
    fclose(this->fp_len);
    fclose(this->fp_char);
}
/*returns the number of bits needed to store a number between 0 and x-1*/
unsigned int LZparser::log(unsigned int x){
    int l=0;
    while(x>0){
        l++;
        x=x>>1;
    }
    return l;    
}

}
}


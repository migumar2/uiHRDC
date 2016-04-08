#include "extractor77.h"
#include <iostream>

Extractor77::Extractor77(char* filename, unsigned int sampling):Extractor(filename, sampling){
    this->bits_size += this->len*this->log(this->text_len-1);
    std::cerr<<"Total size in bits : "<<this->bits_size<<std::endl;
    std::cerr<<"Total size in bytes: "<<this->bits_size/8+(this->bits_size%8==0?0:1)<<std::endl;
}
Extractor77::~Extractor77(){
}
unsigned char* Extractor77::extract(unsigned int start, unsigned int end){
    if(end>=this->text_len){
        end=this->text_len-1;
    }
    unsigned char* answer = new unsigned char[end-start+1];
    //std::cerr<<start<<" "<<end<<std::endl;    
    charextract(answer,start,end,0,end-start);
    //std::cerr<<"selects: "<<this->selects<<std::endl;
    return answer;
}

void Extractor77::charextract(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end){
    //std::cerr<<end<<std::endl;
    while(1){//start<=end
        //if(start>end)return;
        unsigned int last_phrase = 0;
        unsigned int r = this->dc->rank_select(end,&last_phrase);
        this->selects++;
        if(last_phrase==end){
            //std::cout<<offset_end<<" "<<r-1<<" "<<this->chars[r-1]<<" s: "<<this->selects<<std::endl;
            answer[offset_end] = this->chars[r-1];
            if(start<end){
                end = end - 1;
                offset_end = offset_end - 1;
                //return charextract(answer,start,end-1,offset_start,offset_end-1);
            }else{
                return;
            }
        }else{
            //std::cout<<end+1<<std::endl;
            //std::cout<<"last: "<<last_phrase<<" r: "<<r<<std::endl;        
            //std:cerr<<"sel: "<<b<<" "<<r<<std::endl;
            if(start>last_phrase){
                start = this->sources[r] + (start - last_phrase) -1;
                end = this->sources[r] + (end - last_phrase) -1;
                //return charextract(answer, this->sources[r] + (start - last_phrase) -1, this->sources[r] + (end - last_phrase) -1, offset_start, offset_end);
            }else{
                charextract(answer, this->sources[r], this->sources[r] + (end - last_phrase) -1, offset_end - (end-last_phrase-1), offset_end);
                //std::cout<<offset_end - (end-last_phrase)<<" "<<this->chars[r-1]<<" s: "<<this->selects<<std::endl;
                answer[offset_end - (end-last_phrase)] = this->chars[r-1];
                if(start<last_phrase){
                    offset_end = offset_end - (end-last_phrase) - 1;
                    end = last_phrase - 1;
                    //return charextract(answer, start, last_phrase - 1, offset_start, offset_end - (end-last_phrase) - 1);
                }else{
                    return;
                }
            }
        }
    }
}

/*void Extractor77::charextract2(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end){
    if(start>end)return;
    if(bitget(this->b->data,end)){
        if(start<end)
            charextract2(answer,start,end-1,offset_start,offset_end-1);
        //std::cout<<offset_end<<" ";
        answer[offset_end] = this->chars[rank_1(b,end+1)-1];
    }else{
        unsigned int r = rank_1(b,end+1);
        unsigned int last_phrase = select_1(b,r);
        if(start>last_phrase){
            charextract2(answer,this->sources[r] + (start - last_phrase) -1, this->sources[r] + (end - last_phrase) -1, offset_start, offset_end);
        }else{
            if(start<last_phrase)
                charextract2(answer, start, last_phrase - 1, offset_start, offset_end - (end-last_phrase) - 1);
            //std::cout<<offset_end - (end- last_phrase)<<" ";
            answer[offset_end - (end-last_phrase)] = this->chars[r-1];
            charextract2(answer, this->sources[r], this->sources[r] + (end - last_phrase) -1, offset_end - (end-last_phrase-1), offset_end);
        }
    }
}*/

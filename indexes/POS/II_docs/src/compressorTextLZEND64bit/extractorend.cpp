#include "extractorend.h"
#include <iostream>

#define length(i,lp) (this->dc->select(i+1)-lp)
//#define length(i) (this->dc->getDelta(i))

ExtractorEnd::ExtractorEnd(char* filename, unsigned int sampling):Extractor(filename, sampling){
    this->bits_size += this->len*this->log(this->len-1);
    std::cerr<<"Total size in bits : "<<this->bits_size<<std::endl;
    std::cerr<<"Total size in bytes: "<<this->bits_size/8+(this->bits_size%8==0?0:1)<<std::endl;
    this->sources_id = new unsigned int[this->len];
    for(unsigned int i=0;i<this->len;i++){
        if(this->plen[i]==1){
            this->sources_id[i] = 0;
        }else{
            unsigned int r = this->dc->rank(this->sources[i]+this->plen[i]-2)-1;
            this->sources_id[i] = r;
        }
    }  
}
ExtractorEnd::~ExtractorEnd(){
    delete[] sources_id;
}
unsigned char* ExtractorEnd::extract(unsigned int start, unsigned int end){
    if(end>=this->text_len){
        end=this->text_len-1;
    }
    unsigned char* answer = new unsigned char[end-start+1];
    /*if(bitget(this->b->data,end)){
        charextract2(answer,start,end,0,end-start);
    }else{
        unsigned int r = rank_1(b,end+1);
        unsigned int last_phrase = select_1(b,r);
        charextract(answer,last_phrase+1,end,last_phrase-start+1,end-start);
        charextract2(answer,start,last_phrase,0,last_phrase-start);
    }*/
    charextract(answer,start,end,0,end-start);
    //std::cerr<<"selects: "<<this->selects<<std::endl;
    return answer;
}

void ExtractorEnd::charextract(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end){
    //if(start>end)return;
    unsigned int source;
    while(1){
        unsigned int last_phrase = 0;
        unsigned int r = this->dc->rank_select(end,&last_phrase);
        this->selects++;
        //std::cerr<<"Sel1"<<std::endl;        
        if(last_phrase == end){
            //std::cout<<offset_end<<" ";
            return charextract2(answer,start,end,offset_start,offset_end);
        }else{
            //std:cerr<<"sel: "<<b<<" "<<r<<std::endl;
            if(start>last_phrase){
                source = this->dc->select(this->sources_id[r] + 1) - (length(r,last_phrase)-2);
                //std::cerr<<"A"<<source<<" "<<this->sources[r]<<" :"<<r<<" "<<length(r)<<" "<<this->dc->select(this->sources_id[r] + 1)<<" "<<(this->sources_id[r] + 1)<<std::endl;
                //**start = this->sources[r] + (start - last_phrase) -1;
                start = source + (start - last_phrase) -1;
                //**end = this->sources[r] + (end - last_phrase) -1;
                end = source + (end - last_phrase) -1;
                //return charextract(answer, this->sources[r] + (start - last_phrase) -1, this->sources[r] + (end - last_phrase) -1, offset_start, offset_end);
            }else{
                source = this->dc->select(this->sources_id[r] + 1) - (length(r,last_phrase)-2);
                //std::cerr<<"B"<<source<<" "<<this->sources[r]<<" :"<<r<<" "<<length(r)<<" "<<this->dc->select(this->sources_id[r] + 1)<<" "<<(this->sources_id[r] + 1)<<std::endl;
                //**charextract(answer, this->sources[r], this->sources[r] + (end - last_phrase) -1, offset_end - (end-last_phrase-1), offset_end);
                charextract(answer, source, source + (end - last_phrase) -1, offset_end - (end-last_phrase-1), offset_end);
                //std::cout<<offset_end - (end-last_phrase)<<" ";
                return charextract2(answer, start, last_phrase, offset_start, offset_end - (end-last_phrase));
            }
        }
    }
}

void ExtractorEnd::charextract2(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end){
    //std::cout<<offset_end<<" ";
    unsigned int source;
    while(1){
        if(end==0){
            answer[offset_end]=this->chars[0];
            return;
        }
        unsigned int last_phrase = 0;
        unsigned int r = this->dc->rank_select(end-1,&last_phrase);
        this->selects++;
        //std::cerr<<"Sel2"<<std::endl;
        //std::cerr<<"Outputting: "<<offset_end<<std::endl;
        answer[offset_end] = this->chars[r];
        //std::cout<<start<<" "<<end<<" "<<offset_start<<" "<<offset_end<<" "<<bitget(this->b->data,end)<<" "<<r<<" "<<answer[offset_end]<<std::endl;
        if(start<end){
            //std:cerr<<"sel2: "<<b<<" "<<r-1<<std::endl;
            if(start>last_phrase){
                source = this->dc->select(this->sources_id[r] + 1) - (length(r,last_phrase)-2);
                //std::cerr<<"C"<<source<<" "<<this->sources[r]<<" :"<<r<<" "<<length(r)<<" "<<this->dc->select(this->sources_id[r] + 1)<<" "<<(this->sources_id[r] + 1)<<std::endl;
                //std::cout<<"A"<<std::endl;
                //**start = this->sources[r] + (start - last_phrase) -1;
                start = source + (start - last_phrase) -1;
                //**end = this->sources[r] + (end - last_phrase) - 2;
                end = source + (end - last_phrase) - 2;
                offset_end = offset_end - 1;
                //return charextract2(answer, this->sources[r] + (start - last_phrase) -1, this->sources[r] + (end - last_phrase) - 2, offset_start, offset_end - 1);
            }else{
                //std::cout<<"B"<<std::endl;
                if(last_phrase+1!=end){
                    source = this->dc->select(this->sources_id[r] + 1) - (length(r,last_phrase)-2);
                    //std::cerr<<"D"<<source<<" "<<this->sources[r]<<" :"<<r<<" "<<length(r)<<" "<<this->dc->select(this->sources_id[r] + 1)<<" "<<(this->sources_id[r] + 1)<<std::endl;
                    //**charextract2(answer, this->sources[r], this->sources[r] + (end - last_phrase) - 2, offset_end - (end-last_phrase-1), offset_end - 1);
                    charextract2(answer, source, source + (end - last_phrase) - 2, offset_end - (end-last_phrase-1), offset_end - 1);
                }
                offset_end = offset_end - (end-last_phrase);
                end = last_phrase;
                //return charextract2(answer, start, last_phrase, offset_start, offset_end - (end-last_phrase));
            }
        }else{
            return;
        }
    }
}

/*void ExtractorEnd::charextract(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end){
    //std::cout<<"start: "<<start<<" end: "<<end<<std::endl;
    if(start>end)return;
    if(bitget(this->b->data,end)){
        unsigned int pos = end;
        unsigned int charpos = pos;
        for(unsigned int i=0;i<=end-start;i++){
            pos = end - i;
            if(bitget(this->b->data,pos)){
                charpos = pos;
            }else if(!bitget(this->b->data,charpos)){
                unsigned int r = rank_1(b,charpos+1);
                charpos = this->sources[r] + (charpos - select_1(b,r)) - 1;
            }
            std::cout<<"pos: "<<pos<<" charpos: "<<charpos<<std::endl;
            answer[offset_end - i] = this->chars[rank_1(b,charpos+1)-1];
            pos--;
            charpos--;
        }
    }else{
        unsigned int r = rank_1(b,end+1);
        unsigned int last_phrase = select_1(b,r);
        if(start>last_phrase){
            charextract(answer,this->sources[r] + (start - last_phrase) -1, this->sources[r] + (end - last_phrase) -1, offset_start, offset_end);
        }else{
            charextract(answer, this->sources[r], this->sources[r] + (end - last_phrase) -1, offset_end - (end-last_phrase-1), offset_end);
            answer[offset_end - (end-last_phrase)] = this->chars[r-1];
            if(start<last_phrase)
                charextract(answer, start, last_phrase - 1, offset_start, offset_end - (end-last_phrase) - 1);
        }
    }
}*/

namespace lz77index{
namespace extractor{

void LZ77(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end, static_selfindex_lz77* idx){
    while(1){//start<=end
        //if(start>end)return;
        unsigned int last_phrase = 0;
        unsigned int r = this->dc->rank_select(end,&last_phrase);
        this->selects++;
        if(last_phrase==end){
            //std::cout<<offset_end<<" "<<r-1<<" "<<this->chars[r-1]<<" s: "<<this->selects<<std::endl;
            answer[offset_end] = idx->this->chars[r-1];
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
                start = idx->this->sources[r] + (start - last_phrase) -1;
                end = idx->this->sources[r] + (end - last_phrase) -1;
                //return charextract(answer, this->sources[r] + (start - last_phrase) -1, this->sources[r] + (end - last_phrase) -1, offset_start, offset_end);
            }else{
                charextract(answer, idx->this->sources[r], idx->this->sources[r] + (end - last_phrase) -1, offset_end - (end-last_phrase-1), offset_end);
                //std::cout<<offset_end - (end-last_phrase)<<" "<<this->chars[r-1]<<" s: "<<this->selects<<std::endl;
                answer[offset_end - (end-last_phrase)] = idx->this->chars[r-1];
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

}
}

#include <cstdio>
#include <iostream>

#include "extractor.h"
#include "utils.h"
//#include "basics.h"

Extractor::Extractor(char* filename, unsigned int sampling){
    //this->text_len = (unsigned int)lenText(filename);
    this->filename = filename;
    char fn_start[2048];
    char fn_len[2048];
    char fn_chars[2048];
    snprintf(fn_start,2048,"%s.start",filename);
    snprintf(fn_len,2048,"%s.len",filename);
    snprintf(fn_chars,2048,"%s.char",filename);
    unsigned int* temp = readArray(fn_len,(int*)&(this->len),4);
    //unsigned int* rbitmap = (unsigned int*)calloc(this->text_len/32+1,sizeof(unsigned int));
    //unsigned int pos=0;
    //bitset(rbitmap,pos);
    unsigned int new_text_len = temp[0];
    for(unsigned int i=1;i<this->len;i++){
        //pos += temp[i];
        //std::cout<<"Setting pos: "<<pos<<std::endl;
        //bitset(rbitmap,pos);
        new_text_len += temp[i];
    }
    this->text_len = new_text_len;
    //std::cout<<this->text_len<<std::endl;
    //free(temp);
    this->plen = temp;
    //this->b = createBitmap(rbitmap,this->text_len,false);
    this->dc = new DeltaCodes(this->plen,this->len,sampling);//last value is sampling
    //delete[] rbitmap;

    
    {//this->sources = readArray(fn_start,(int*)&(this->len),4);
		//loads sources and represents each souces[i] with log_k bits.
		unsigned int *sources = readArray(fn_start,(int*)&(this->len),4);
		
		size_t i; uint maxph=sources[0];
		for (i=0;i<this->len; i++) {
			if (sources[i] > maxph) maxph = sources[i];	
			//cout << "sources[" << i  << "] =" << sources[i] << endl;
		}
		
		std::cout << "\n ##### LEN = " << this->len << " , MAXPH =" << maxph << " , text_len = " << this->len << std::endl;

		this->sourcesbits = bits(maxph);
		std::cout << "\n ##### bits per elem in sources[] = " << this->sourcesbits << " ####"<< std::endl;
		this->sourcesnints = uint_len_LL(this->sourcesbits, this->len);
		this->sources = (unsigned int *) malloc (sizeof(unsigned int) * this->sourcesnints);
		this->sources[this->sourcesnints-1] = 0000;
		for (i=0; i< this->len ; i++) {
			 set_field_LL(this->sources, this->sourcesbits, i, sources[i]); 
		 }		
	}
	

	
    
    this->chars = (unsigned char*)readArray(fn_chars,(int*)&(this->len),1);
    //std::cerr<<this->len<<" "<<new_text_len<<" "<<this->text_len<<std::endl;
    this->selects=0;

    /*computing size in bits*/
    this->bits_size = 3;
    //size of chars
    unsigned int sigma = this->getSigma();
    unsigned int log_sigma = this->log(sigma-1);
    if(log_sigma!=8)this->bits_size += 8*sigma;
    this->bits_size += log_sigma * this->len;
    //size of sources
    //this->bits_size += this->sourcesSize();
    //We have to add this in the constructor of each derived class
    //size of len bitmap, including the structure for selects
    this->bits_size += this->dc->size()*8;
}

Extractor::~Extractor(){
    if(this->sources!=NULL)free(sources);
    //if(this->b!=NULL)destroyBitmap(b);
    if(this->chars!=NULL)free(chars);
    if(this->plen!=NULL)free(plen);
    //if(this->bs!=NULL)delete bs;
    if(this->dc!=NULL)delete dc;
}

unsigned char* Extractor::extract(unsigned int start, unsigned int end){
    if(end>=this->text_len){
        end=this->text_len-1;
    }
    unsigned char* answer = (unsigned char*)malloc(sizeof(unsigned int)*(end-start+1));//new unsigned char[end-start+1];
    for(unsigned int i=start;i<=end;i++){
        answer[i-start]=getChar(i);
    }
    return answer;
}

unsigned char* Extractor::extractAll(){
    unsigned char* answer = new unsigned char[this->text_len];
    answer[0] = this->chars[0];
    unsigned int phrase = 2;//phrase to be extracted
    unsigned int last_phrase_pos = 0;
    unsigned int phrase_pos;
    unsigned int phrase_len;
    //unsigned int phrase_pos2;
    //unsigned int phrase_len2;
    while(phrase<this->len){      
        //phrase_pos = select_1(this->b,phrase);
        //phrase_len = phrase_pos - last_phrase_pos;
        phrase_pos = last_phrase_pos + this->plen[phrase-1];
        phrase_len = this->plen[phrase-1];    
        //if(phrase_pos!=phrase_pos2 || phrase_len!=phrase_len2)std::cout<<phrase<<" "<<phrase_pos<<" "<<phrase_pos2<<" "<<phrase_len<<" "<<phrase_len2<<" "<<last_phrase_pos<<std::endl;
        //std::cerr<<"last_phrase_pos: "<<last_phrase_pos<<" phrase_pos: "<<phrase_pos<<std::endl;
        for(unsigned int i=0;i<phrase_len-1;i++){			
			uint sourcesph = get_field_LL(this->sources, this->sourcesbits, phrase -1); //uint sourcesph = this->sources[phrase - 1];
            answer[last_phrase_pos + 1 + i] = answer[ sourcesph + i ];
            //std::cerr<<"copying: "<<last_phrase_pos+i+1<<"from "<<this->sources[phrase - 1] + i<<std::endl;
        }
        answer[phrase_pos] = this->chars[phrase - 1];
        //std::cerr<<"adding: "<<phrase_pos<<"from "<<phrase-1<<std::endl;
        phrase++;
        last_phrase_pos = phrase_pos;
    }
    return answer;
}

unsigned char Extractor::getChar(unsigned int pos){
    unsigned int select;
    unsigned int r;
    r = dc->rank_select(pos,&select);
    while(select!=pos){
        //std::cout<<select<<" "<<pos<<" "<<r<<std::endl;
        uint sourcesph = get_field_LL(this->sources, this->sourcesbits, r); //uint sourcesph = this->sources[r];
        pos = sourcesph + (pos - select) - 1;
        r = dc->rank_select(pos,&select);
    }
    return this->chars[r-1];
    /*if(bitget(this->b->data,pos)){
        return this->chars[rank_1(b,pos+1)-1];
    }
    unsigned int r = rank_1(b,pos+1);
    //unsigned int caca = r+1;std::cout<<"caca: "<<caca<<std::endl;
    //std::cerr<<r<<" "<<this->sources<<std::endl;
    unsigned int source =  this->sources[r];
    //std::cout<<pos<<" "<<source<<" "<<r<<" "<<select_1(b,r)<<std::endl;
    unsigned int newpos = source + (pos - select_1(b,r)) - 1;
    //std::cout<<std::endl<<"newpos: "<<newpos<<std::endl;
    return getChar(newpos);*/
}

unsigned int Extractor::getSigma(){
    unsigned int sigma=0;    
    unsigned int count[256];
    unsigned int a;
    unsigned char* text = readText(this->filename,&a);
    for(unsigned int i=0;i<256;i++)count[i]=0;    
    for(unsigned int i=0;i<this->text_len;i++){
        count[text[i]]+=1;
    }
    free(text);
    for(unsigned int i=0;i<256;i++)if(count[i]!=0)sigma++;
    return sigma;
}

unsigned int Extractor::size(){
    return sizeof(Extractor)+ 
	    this->dc->size() + 
		this->len*(sizeof(char))  + 		
		//this->len*(sizeof(unsigned int))           // size of sources[]
		this->sourcesnints*(sizeof(unsigned int))    // size of sources[] encoded with log_k bits
	;
}

/*returns the number of bits needed to store a number between 0 and x-1*/
unsigned int Extractor::log(unsigned int x){
    int l=0;
    while(x>0){
        l++;
        x=x>>1;
    }
    return l;    
}


/*unsigned int Extractor::sourcesSize(){
    unsigned int t=1;
    for(unsigned int i=0;i<this->len;i++){
        t=(t+i)%256;
    }
    std::cerr<<"caca base "<<t<<" "<<__LINE__<<std::endl;
    return this->len*this->log(this->text_len-1);
}*/

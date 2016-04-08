#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include "deltacodes.h"

//unsigned int P[]={6,5,4,4,3,3,3,3,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

DeltaCodes::DeltaCodes(unsigned int* array, unsigned int n, unsigned int sampling){   
  
    std::cout << "DELTACODES::DELTACODES->SAMPLING set to " << sampling << "*************\n";
    this->n = n;
    this->sampling = sampling;
    sampled_pointer =  new unsigned int[this->n/this->sampling+2]();
    sampled_total = new unsigned int[this->n/this->sampling+2]();
    sampled_pointer[0]=0;
    sampled_total[0]=0;
    this->total_size=0;    
    for(unsigned int i=0;i<this->n;i++){
        //printf(" [%d]",array[i]);
        this->total_size += 1 + 2*(bits(bits(array[i]))-1) + (bits(array[i])-1);   //len of delta-coded sequence
    }
    deltacodes = new unsigned int[(this->total_size-1)/32+2]();
    unsigned int pos = 0;
    unsigned int total = 0;
    for(unsigned int i=0;i<this->n;i++){
        if(i%this->sampling==0){
            this->sampled_pointer[i/this->sampling] = pos;
            this->sampled_total[i/this->sampling] = total;
        }
        total += array[i];
        encodeDelta(array[i],this->deltacodes,&pos);
    }
    if(this->n%this->sampling==0){
        this->sampled_pointer[this->n/this->sampling] = pos;
        this->sampled_total[this->n/this->sampling] = total;
    }
    std::cerr<<"Pos: "<<pos<<" total_size: "<<this->total_size<<std::endl;
    
    //now encode sampled_pointer with log_k bits.
    {
	size_t i; uint maxptr=pos;
	size_t nsamples = this->n/this->sampling+1;
	
	std::cout << "#####DC->maxptr =" << maxptr << std::endl;

	this->sampled_pointer_bits = bits(pos);
	std::cout << "##### bits per elem in dc->sampled_pointer[] = " << this->sampled_pointer_bits<< std::endl;
	this->sampled_pointer_nints = uint_len_LL(this->sampled_pointer_bits, nsamples);
	uint * ptrs = new unsigned int [this->sampled_pointer_nints];	
	ptrs[this->sampled_pointer_nints-1] = 0000;
	for (i=0; i< nsamples ; i++) {
		  set_field_LL(ptrs, this->sampled_pointer_bits, i, this->sampled_pointer[i]); 
	}    
	delete[] this->sampled_pointer;
	this->sampled_pointer = ptrs;	  
    }
    
}
DeltaCodes::~DeltaCodes(){
    if(this->deltacodes!=NULL)delete[] deltacodes;
    if(this->sampled_pointer!=NULL)delete[] sampled_pointer;
    if(this->sampled_total!=NULL)delete[] sampled_total;
}

unsigned int DeltaCodes::select(unsigned int total){
    unsigned int pos = total/this->sampling;
    unsigned int check = total%this->sampling;
    unsigned int s = this->sampled_total[pos];
    
    unsigned int pointer = get_field_LL(this->sampled_pointer, this->sampled_pointer_bits, pos);  //unsigned int pointer = this->sampled_pointer[pos];
    for(unsigned int i=0;i<check;i++){
        s += decodeDelta(this->deltacodes,&pointer);
    }
    return s-1;
}

//returns the number of 1's found until position pos (included)
unsigned int DeltaCodes::rank(unsigned int pos){
    unsigned int select;
    return this->rank_select(pos,&select);
}
unsigned int DeltaCodes::access(unsigned int pos){
    unsigned int select;
    this->rank_select(pos,&select);
    return select == pos;
}   
unsigned int DeltaCodes::rank_select(unsigned int pos, unsigned int* select){
    unsigned int upper_bound = std::min((pos+1)/this->sampling+1,this->n/this->sampling+1);
    unsigned int index = std::lower_bound(this->sampled_total,this->sampled_total+upper_bound,pos+1)-this->sampled_total;
    //std::cout<<"Index:"<<index<<std::endl;
    if(this->sampled_total[index]==pos+1){
        *select = pos; 
        return index*this->sampling;
    }else{
        unsigned int pointer = get_field_LL(this->sampled_pointer, this->sampled_pointer_bits, index -1); //this->sampled_pointer[index-1];
        unsigned int total = this->sampled_total[index-1];
        index=(index-1)*this->sampling;
        for(unsigned int i=0;i<this->sampling;i++){
            unsigned int val = decodeDelta(this->deltacodes,&pointer);
            if(val+total>pos+1){
                *select = total-1;
                return index+i;
            }else if(val+total==pos+1){
              *select=pos;
              return index+i+1;
            }
            total+=val;
        }
        return (unsigned int)-1;//should not return this.
    }
}

unsigned int DeltaCodes::size(){
    return     sizeof(unsigned int)*(this->n/this->sampling+1)    // sampled total_values
	     //sizeof(unsigned int)*(this->n/this->sampling+1)    // sampled ptrs
	     + sizeof(unsigned int)*(this->sampled_pointer_nints) // sampled ptrs
             + sizeof(unsigned int)*((this->total_size-1)/32+2);  //encoded delta sequence.
}

unsigned int DeltaCodes::bits(unsigned int n){
    unsigned int b=0;
    while(n>0){
        b++;
        n>>=1;
    }
    return b;
}
void DeltaCodes::encodeDelta(unsigned int val, unsigned int* deltaCodes, unsigned int* pos){
    unsigned int deltaCodesPos = *pos;    
    unsigned int k = bits(val);
    unsigned int p = bits(k);
    //computes the first k-1 bits of val
    unsigned int code = val & ((1<<(k-1))-1);
    //unsigned int codeLength = 2*p+k-2;		

    //Add the initial p-1 0's
    deltaCodesPos += p-1;

    //Add p bits of k
    if( ((deltaCodesPos%32) + p) > 32 ) {	
	    deltaCodes[deltaCodesPos/32] |= (k>>((deltaCodesPos%32)+p-32));
	    deltaCodes[deltaCodesPos/32+1] = (k<<(64-(deltaCodesPos%32)-p));	
    }else{
	    deltaCodes[deltaCodesPos/32] |= (k<<(32-p-(deltaCodesPos%32)));				
    }
    deltaCodesPos += p;

    //Add k-1 bits of val (without leading 1)
    if( ((deltaCodesPos%32) + (k-1)) > 32 ) {	
	    deltaCodes[deltaCodesPos/32] |= (code>>((deltaCodesPos%32)+(k-1)-32));
	    deltaCodes[deltaCodesPos/32+1] = (code<<(64-(deltaCodesPos%32)-(k-1)));	
    } else {
	    deltaCodes[deltaCodesPos/32] |= (code<<(32-(k-1)-(deltaCodesPos%32)));				
    }
    deltaCodesPos += k-1;
    *pos = deltaCodesPos;
}

unsigned int DeltaCodes::decodeDelta(unsigned int* deltaCodes, unsigned int* pos){
    //std::cout<<"DecodeDelta Pos:"<<*pos<<std::endl;
    unsigned int code;
    unsigned int pointer = *pos;
    code = (deltaCodes[pointer/32] << (pointer%32)) |
	       ((pointer%32 != 0) * (deltaCodes[pointer/32+1] >> (32-(pointer%32))));

    //Compute number of 0's (p) in the left of the code
    //unsigned int p = P[((code & 0xF8000000)>>27)];//should check if p=6
    unsigned int p = 1;
    while(!(code & 0x80000000)) {
	    code <<= 1;
	    p++;
    }
    //code<<=p-1;

    //Compute length of binary representation of code (k)
    unsigned int k = code >> (32-p);		

    //Update deltaCodes pointer
    pointer += 2*p-1;

    // Add binary representation of code
    code = (deltaCodes[pointer/32] << (pointer%32)) |
	       ((pointer%32 != 0) * (deltaCodes[pointer/32+1] >> (32-(pointer%32))));
    code = ((code >> 1) | 0x80000000) >> (32-k);
    pointer += k-1;
    *pos = pointer;
    return code;
}



unsigned int DeltaCodes::get_n(){/**fari 2013, only for statistics in ilist.cpp **/
  return this->n;
}

unsigned int DeltaCodes::get_total_size_bits(){ /**fari 2013, only for statistics in ilist.cpp **/
  return this->total_size;
}

unsigned int DeltaCodes::get_sampled_pointer_bits(){ /**fari 2013, only for statistics in ilist.cpp **/
  return this->sampled_pointer_bits;
}

unsigned int DeltaCodes::get_sampled_pointer_nints(){ /**fari 2013, only for statistics in ilist.cpp **/
  return this->sampled_pointer_nints;
}


/*int main(int argc,char** argv){
    if(argc==1){
        std::cerr<<"Usage: delta n1 n2 n3"<<std::endl;
        return EXIT_FAILURE;
    }*/
    /*unsigned int* deltas = new unsigned int[argc-1];
    unsigned int pos=0;
    for(int i=1;i<argc;i++){
        encodeDelta((unsigned int)atoi(argv[i]),deltas,&pos);
    }
    pos=0;
    for(int i=1;i<argc;i++){
        unsigned int d = decodeDelta(deltas,&pos);
        if(d!=(unsigned int)atoi(argv[i]))std::cerr<<d<<" "<<atoi(argv[i])<<std::endl;
    }
    delete[] deltas;*/
 /*   unsigned int* array = new unsigned int[argc-1];
    for(int i=1;i<argc;i++){
        array[i-1]=(unsigned int)atoi(argv[i]);
    }
    DeltaCodes* dc=new DeltaCodes(array,argc-1,10);
    delete[] array;
    std::cout<<"Size(bytes):"<<dc->size()<<std::endl;
    for(unsigned int i=0;i<=argc-1;i++){
        std::cout<<"Select "<<i<<": "<<dc->select(i)<<std::endl;
    }
    for(unsigned int i=0;i<31;i++){
        std::cout<<"Rank "<<i<<": "<<dc->rank(i)<<std::endl;
    }
    for(unsigned int i=0;i<31;i++){
        std::cout<<"Access "<<i<<": "<<dc->access(i)<<std::endl;
    }
    delete dc;
    return EXIT_SUCCESS;
}*/

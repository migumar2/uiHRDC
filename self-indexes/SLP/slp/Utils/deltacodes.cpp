/* deltacodes.cpp
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <iostream>
#include "deltacodes.h"

DeltaCodes::DeltaCodes(){}

DeltaCodes::DeltaCodes(unsigned int* array, unsigned int n, unsigned int sampling){   
    this->n = n;
    this->sampling = sampling;
    sampled_pointer =  new unsigned int[this->n/this->sampling+2]();
    sampled_total = new unsigned int[this->n/this->sampling+2]();
    sampled_pointer[0]=0;
    sampled_total[0]=0;
    this->total_size=0;    
    for(unsigned int i=0;i<this->n;i++){
        this->total_size += 1 + 2*(bits(bits(array[i]))-1) + (bits(array[i])-1);
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
    unsigned int pointer = this->sampled_pointer[pos];
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

    if(this->sampled_total[index]==pos+1){
        *select = pos; 
        return index*this->sampling;
    }else{
        unsigned int pointer = this->sampled_pointer[index-1];
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
    return sizeof(unsigned int)*2*(this->n/this->sampling+2)+sizeof(unsigned int)*((this->total_size-1)/32+2);
}

unsigned int DeltaCodes::elements(){
	return n;
}

void
DeltaCodes::save(ofstream & output)
{
	saveValue(output, n);
	saveValue(output, sampling);
	saveValue(output, total_size);
	saveValue(output, sampled_pointer, n/sampling+2);
	saveValue(output, sampled_total, n/sampling+2);
	saveValue(output, deltacodes, (total_size-1)/32+2);
}

DeltaCodes* 
DeltaCodes::load(ifstream & input)
{
	DeltaCodes *dc = new DeltaCodes();

	dc->n = loadValue<uint>(input);
	dc->sampling = loadValue<uint>(input);
	dc->total_size = loadValue<uint>(input);
	dc->sampled_pointer = loadValue<uint>(input, dc->n/dc->sampling+2);
	dc->sampled_total = loadValue<uint>(input, dc->n/dc->sampling+2);
	dc->deltacodes = loadValue<uint>(input, (dc->total_size-1)/32+2);

	return dc;
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
    unsigned int code;
    unsigned int pointer = *pos;
    code = (deltaCodes[pointer/32] << (pointer%32)) |
	       ((pointer%32 != 0) * (deltaCodes[pointer/32+1] >> (32-(pointer%32))));

    //Compute number of 0's (p) in the left of the code
    unsigned int p = 1;
    while(!(code & 0x80000000)) {
	    code <<= 1;
	    p++;
    }

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


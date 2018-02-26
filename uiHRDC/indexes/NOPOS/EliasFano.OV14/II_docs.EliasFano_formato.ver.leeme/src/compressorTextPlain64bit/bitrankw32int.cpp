#include "bitrankw32int.h"
#include "assert.h"
#include "math.h"
#include <sys/types.h>


/////////////
//Rank(B,i)// 
/////////////
//_factor = 0  => s=W*lgn
//_factor = P  => s=W*P
//Is interesting to notice
//factor=2 => overhead 50%
//factor=3 => overhead 33%
//factor=4 => overhead 25%
//factor=20=> overhead 5%

BitRankW32Int::BitRankW32Int( uint *bitarray, uint _n, bool owner, uint _factor){
  data=bitarray;
  this->owner = owner;
  this->n=_n;
  uint lgn=bits(n-1);
  this->factor=_factor;
  if (_factor==0) this->factor=lgn;
  else this->factor=_factor;
  b=32;
  s=b*this->factor;
  integers = n/W+1;
  BuildRank();
}

BitRankW32Int::~BitRankW32Int() {
  delete [] Rs;
  if (owner) delete [] data;
}

//Metodo que realiza la busqueda d
void BitRankW32Int::BuildRank(){
  uint num_sblock = n/s;
  Rs = new uint[num_sblock+5];// +1 pues sumo la pos cero
  for(uint i=0;i<num_sblock+5;i++)
    Rs[i]=0;
  uint j;
  Rs[0]=0;
  for (j=1;j<=num_sblock;j++) {
    Rs[j]=Rs[j-1];
    Rs[j]+=BuildRankSub((j-1)*factor,factor);
  }
}

uint BitRankW32Int::BuildRankSub(uint ini,uint bloques){
  uint rank=0,aux;
  for(uint i=ini;i<ini+bloques;i++) {
    if (i < integers) {
      aux=data[i];
      rank+=popcount(aux);
    }
  }
  return rank; //retorna el numero de 1's del intervalo

}

uint BitRankW32Int::rank(uint i) {
  ++i;
  uint resp=Rs[i/s];
  uint aux=(i/s)*factor;
  for (uint a=aux;a<i/W;a++)
    resp+=popcount(data[a]);
  resp+=popcount(data[i/W]  & ((1<<(i & mask31))-1));
  return resp;
}

bool BitRankW32Int::IsBitSet(uint i) {
  return (1u << (i % W)) & data[i/W];
}

int BitRankW32Int::save(FILE *f) {
  if (f == NULL) return 20;
  if (fwrite (&n,sizeof(uint),1,f) != 1) return 21;
  if (fwrite (&factor,sizeof(uint),1,f) != 1) return 21;
  if (fwrite (data,sizeof(uint),n/W+1,f) != n/W+1) return 21;
  if (fwrite (Rs,sizeof(uint),n/s+1,f) != n/s+1) return 21;
  return 0;
}

int BitRankW32Int::load(FILE *f) {
  if (f == NULL) return 23;
  if (fread (&n,sizeof(uint),1,f) != 1) return 25;
  b=32; // b is a word
  if (fread (&factor,sizeof(uint),1,f) != 1) return 26;
  s=b*factor;
  uint aux=(n+1)%W;
  if (aux != 0)
    integers = (n+1)/W+1;
  else
    integers = (n+1)/W;
  data= new uint[n/W+1];
  if (!data) return 1;
  if (fread (data,sizeof(uint),n/W+1,f) != n/W+1) return 27;
  this->owner = true;
  Rs= new uint[n/s+1];
  if (!Rs) return 1;
  if (fread (Rs,sizeof(uint),n/s+1,f) != n/s+1) return 28;
  return 0;
}

BitRankW32Int::BitRankW32Int(FILE *f, int *error) {
  *error = BitRankW32Int::load(f);
}

uint BitRankW32Int::SpaceRequirementInBits() {
  return (owner?n:0)+(n/s)*sizeof(uint)*8 +sizeof(BitRankW32Int)*8; 
}

uint BitRankW32Int::prev(uint start) {
      // returns the position of the previous 1 bit before and including start.
      // tuned to 32 bit machine

      uint i = start >> 5;
      int offset = (start % W);
      long answer = start;
      uint val = data[i] << (Wminusone-offset);

      if (!val) { val = data[--i]; answer -= 1+offset; }

      while (!val) { val = data[--i]; answer -= W; }

      if (!(val & 0xFFFF0000)) { val <<= 16; answer -= 16; }
      if (!(val & 0xFF000000)) { val <<= 8; answer -= 8; }

      while (!(val & 0x80000000)) { val <<= 1; answer--; }
      return (uint)max(answer,0);
}
uint BitRankW32Int::next(uint k) {
        uint count = k;
        uint des,aux2;
        des=count%W; 
        aux2= data[count/W] >> des;
        if (aux2 > 0) {
                if ((aux2&0xff) > 0) return count+select_tab[aux2&0xff]-1;
                else if ((aux2&0xff00) > 0) return count+8+select_tab[(aux2>>8)&0xff]-1;
                else if ((aux2&0xff0000) > 0) return count+16+select_tab[(aux2>>16)&0xff]-1;
                else {return count+24+select_tab[(aux2>>24)&0xff]-1;}
        }

        for (uint i=count/W+1;i<integers;i++) {
                aux2=data[i];
                if (aux2 > 0) {
                        if ((aux2&0xff) > 0) return i*W+select_tab[aux2&0xff]-1;
                        else if ((aux2&0xff00) > 0) return i*W+8+select_tab[(aux2>>8)&0xff]-1;
                        else if ((aux2&0xff0000) > 0) return i*W+16+select_tab[(aux2>>16)&0xff]-1;
                        else {return i*W+24+select_tab[(aux2>>24)&0xff]-1;}
                }
        }
        return n;
} 

uint BitRankW32Int::select(uint x) {
  // returns i such that x=rank(i) && rank(i-1)<x or n if that i not exist
  // first binary search over first level rank structure
  // then sequential search using popcount over a int
  // then sequential search using popcount over a char
  // then sequential search bit a bit

  //binary search over first level rank structure
  uint l=0, r=n/s;
  uint mid=(l+r)/2;
  uint rankmid = Rs[mid];
  while (l<=r) {
    if (rankmid<x)
      l = mid+1;
    else
      r = mid-1;
    mid = (l+r)/2;
    rankmid = Rs[mid];
  }
  //sequential search using popcount over a int
  uint left;
  left=mid*factor;
  x-=rankmid;
        uint j=data[left];
        uint ones = popcount(j);
        while (ones < x) {
    x-=ones;left++;
    if (left > integers) return n;
          j = data[left];
      ones = popcount(j);
        }
  //sequential search using popcount over a char
  left=left*b;
  rankmid = popcount8(j);
  if (rankmid < x) {
    j=j>>8;
    x-=rankmid;
    left+=8;
    rankmid = popcount8(j);
    if (rankmid < x) {
      j=j>>8;
      x-=rankmid;
      left+=8;
      rankmid = popcount8(j);
      if (rankmid < x) {
        j=j>>8;
        x-=rankmid;
        left+=8;
      }
    }
  }

  // then sequential search bit a bit
        while (x>0) {
    if  (j&1) x--;
    j=j>>1;
    left++;
  }
  return left-1;
}

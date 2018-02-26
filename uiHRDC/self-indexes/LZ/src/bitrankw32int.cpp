#include <cstdlib>

#include "bitrankw32int.h"


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

namespace lz77index{
namespace basics{

bitRankW32Int * createBitRankW32Int( unsigned int *bitarray, unsigned int _n, char owner, unsigned int _factor) {
  bitRankW32Int * br =(bitRankW32Int *) malloc(sizeof(struct sbitRankW32Int));
  br->data=bitarray;
  br->owner = owner;
  br->n=_n;
  unsigned int lgn=bits(br->n-1);
  br->factor=_factor;
  if (_factor==0) br->factor=lgn;
  else br->factor=_factor;
  br->b=32;
  br->s=br->b*br->factor;
  br->integers = br->n/W;
  buildRank(br);
  return br;
}


void destroyBitRankW32Int(bitRankW32Int *br) {
  free(br->Rs);
  if (br->owner) free(br->data);
  free(br);
}


//Metodo que realiza la busqueda d
void buildRank(bitRankW32Int * br) {
	unsigned int i;
  unsigned int num_sblock = br->n/br->s;
  br->Rs = (unsigned int *) malloc(sizeof(unsigned int)*(num_sblock+1));   // +1 pues sumo la pos cero
  for(i=0;i<num_sblock+1;i++)
    br->Rs[i]=0;
  unsigned int j;
  br->Rs[0]=0;
  for (j=1;j<=num_sblock;j++) {
    br->Rs[j]=br->Rs[j-1];
    br->Rs[j]+=buildRankSub(br, (unsigned int)(j-1)*(br->factor),br->factor);
  }
}


unsigned int buildRankSub(bitRankW32Int * br, unsigned int ini,unsigned int bloques) {
  unsigned int i;
  unsigned int rank=0,aux;
  for(i=ini;i<ini+bloques;i++) {
    if (i <= br->integers) {
      aux=br->data[i];
      rank+=popcount(aux);
    }
  }
  return rank;                   //retorna el numero de 1's del intervalo

}


//Precondiciones: ni menor que 0 ni mayor que b->n
unsigned int rank(bitRankW32Int * br, unsigned int i) {
  unsigned int a;
  //if(i+1==0) return 0;
  ++i;
  unsigned int resp=br->Rs[i/br->s];
  unsigned int aux=(i/br->s)*(br->factor);
  for (a=aux;a<i/W;a++)
    resp+=popcount(br->data[a]);
  resp+=popcount(br->data[i/W]  & ((1<<(i & mask31))-1));
  return resp;
}

unsigned int rankAnt(bitRankW32Int * br, unsigned int i) {
  return rank(br,i)-bitget(br->data,i);
}  

unsigned int isBitSet(bitRankW32Int * br, unsigned int i) 
{
  return (1u << (i % W)) & br->data[i/W];
}


int save(bitRankW32Int * br, FILE *f) {
	unsigned int s,n;
	s=br->s;
	n=br->n;
  if (f == NULL) return 20;
  if (fwrite (&(n),sizeof(unsigned int),1,f) != 1) return 21;
  if (fwrite (&(br->factor),sizeof(unsigned int),1,f) != 1) return 21;
  if (fwrite (br->data,sizeof(unsigned int),n/W+1,f) != n/W+1) return 21;
  if (fwrite (br->Rs,sizeof(unsigned int),n/s+1,f) != n/s+1) return 21;
  return 0;
}


int load(bitRankW32Int * br, FILE *f) {
  if (f == NULL) return 23;
  if (fread (&(br->n),sizeof(unsigned int),1,f) != 1) return 25;
  br->b=32;    
  unsigned int b=br->b;                      // b is a word
  if (fread (&(br->factor),sizeof(unsigned int),1,f) != 1) return 25;
  br->s=b*br->factor;
  unsigned int s=br->s;
  unsigned int n= br->n;
  //unsigned int aux=(n+1)%W;
  //if (aux != 0)
  //  integers = (n+1)/W+1;
  //else
  //  integers = (n+1)/W;
  br->integers = n/W;
  br->data= (unsigned int *) malloc(sizeof( unsigned int) *(n/W+1));
  if (!br->data) return 1;
  if (fread (br->data,sizeof(unsigned int),br->n/W+1,f) != n/W+1) return 25;
  br->owner = 1;
  br->Rs=(unsigned int*)malloc(sizeof(unsigned int)*(n/s+1));
  if (!br->Rs) return 1;
  if (fread (br->Rs,sizeof(unsigned int),n/s+1,f) != n/s+1) return 25;
  return 0;
}


bitRankW32Int * createBitRankW32IntFile(FILE *f, int *error) {
	 bitRankW32Int * br = (bitRankW32Int *) malloc(sizeof(struct sbitRankW32Int));
  *error = load(br,f);
  return br;
}


unsigned int spaceRequirementInBits(bitRankW32Int * br) {
  return (br->owner?br->n:0)+(br->n/br->s)*sizeof(unsigned int)*8 +sizeof(struct sbitRankW32Int)*8;
}

unsigned int lenght_in_bits(bitRankW32Int * br) { return br->n; }

unsigned int prev(bitRankW32Int * br,unsigned int start) {
  // returns the position of the previous 1 bit before and including start.
  // tuned to 32 bit machine

  unsigned int i = start >> 5;
  int offset = (start % W);
  unsigned int answer = start;
  unsigned int val = br->data[i] << (Wminusone-offset);

  if (!val) { val = br->data[--i]; answer -= 1+offset; }

  while (!val) { val = br->data[--i]; answer -= W; }

  if (!(val & 0xFFFF0000)) { val <<= 16; answer -= 16; }
  if (!(val & 0xFF000000)) { val <<= 8; answer -= 8; }

  while (!(val & 0x80000000)) { val <<= 1; answer--; }
  return answer;
}


unsigned int select1(bitRankW32Int * br,unsigned int x) {
  return bselect(br,x);
}


unsigned int bselect(bitRankW32Int * br,unsigned int x) {
  if(x==0) return 0;
  // returns i such that x=rank(i) && rank(i-1)<x or n if that i not exist
  // first binary search over first level rank structure
  // then sequential search using popcount over a int
  // then sequential search using popcount over a char
  // then sequential search bit a bit

  //binary search over first level rank structure
  unsigned int n= br->n;
  unsigned int s= br->s;
  unsigned int b=br->b;
  unsigned int l=0, r=n/s;
  unsigned int integers = br->integers;
  unsigned int factor = br->factor;
  unsigned int mid=(l+r)/2;
  unsigned int rankmid = br->Rs[mid];
  while (l<=r) {
    if (rankmid<x)
      l = mid+1;
    else
      r = mid-1;
    mid = (l+r)/2;
    rankmid = br->Rs[mid];
  }
  //sequential search using popcount over a int
  unsigned int left;
  left=mid*factor;
  x-=rankmid;
  unsigned int j=br->data[left];
  unsigned int ones = popcount(j);
  while (ones < x) {
    x-=ones;left++;
    if (left > integers) return n;
    j = br->data[left];
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


unsigned int select0(bitRankW32Int * br,unsigned int x) {
  // returns i such that x=rank_0(i) && rank_0(i-1)<x or n if that i not exist
  // first binary search over first level rank structure
  // then sequential search using popcount over a int
  // then sequential search using popcount over a char
  // then sequential search bit a bit

  //binary search over first level rank structure
    unsigned int n= br->n;
  unsigned int s= br->s;
  unsigned int factor = br->factor;
  unsigned int integers = br->integers;
  unsigned int b= br->b;
  unsigned int l=0, r=n/s;
  unsigned int mid=(l+r)/2;
  unsigned int rankmid = mid*factor*W-(br->Rs)[mid];
  while (l<=r) {
    if (rankmid<x)
      l = mid+1;
    else
      r = mid-1;
    mid = (l+r)/2;
    rankmid = mid*factor*W-(br->Rs)[mid];
  }
  //sequential search using popcount over a int
  unsigned int left;
  left=mid*factor;
  x-=rankmid;
  unsigned int j=br->data[left];
  unsigned int zeros = W-popcount(j);
  while (zeros < x) {
    x-=zeros;left++;
    if (left > integers) return n;
    j = br->data[left];
    zeros = W-popcount(j);
  }
  //sequential search using popcount over a char
  left=left*b;
  rankmid = 8-popcount8(j);
  if (rankmid < x) {
    j=j>>8;
    x-=rankmid;
    left+=8;
    rankmid = 8-popcount8(j);
    if (rankmid < x) {
      j=j>>8;
      x-=rankmid;
      left+=8;
      rankmid = 8-popcount8(j);
      if (rankmid < x) {
        j=j>>8;
        x-=rankmid;
        left+=8;
      }
    }
  }

  // then sequential search bit a bit
  while (x>0) {
    if  (j%2 == 0 ) x--;
    j=j>>1;
    left++;
  }
  left--;
  if (left > n)  return n;
  else return left;
}

}
}


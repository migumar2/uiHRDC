#include "bitmapw32int.h"
#include "assert.h"
#include "math.h"
#include <sys/types.h>




BitMapW32Int::BitMapW32Int( uint *bitarray, uint _n, bool owner){
  data=bitarray;
  this->owner = owner;
  this->n=_n;
  integers = n/WW32+1;
}

BitMapW32Int::~BitMapW32Int() {
  if (owner) delete [] data;
}


bool BitMapW32Int::IsBitSet(uint i) {
  return (1u << (i % WW32)) & data[i/WW32];
}

int BitMapW32Int::save(FILE *f) {
  if (f == NULL) return 20;
  if (fwrite (&n,sizeof(uint),1,f) != 1) return 21;
  if (fwrite (data,sizeof(uint),n/WW32+1,f) != n/WW32+1) return 21;
  return 0;
}

int BitMapW32Int::load(FILE *f) {
  if (f == NULL) return 23;
  if (fread (&n,sizeof(uint),1,f) != 1) return 25;

  data= new uint[n/WW32+1];
  if (!data) return 1;
  if (fread (data,sizeof(uint),n/WW32+1,f) != n/WW32+1) return 27;

  this->owner = true;
  return 0;
}

BitMapW32Int::BitMapW32Int(FILE *f, int *error) {
  *error = BitMapW32Int::load(f);
}

uint BitMapW32Int::SpaceRequirementInBits() {
  return (owner?n:0); 
}


// Implements operations over a bitmap

#include "bitmap.h"


  // In theory, we should have superblocks of size s=log^2 n divided into
  // blocks of size b=(log n)/2. This takes 
  // O(n log n / log^2 n + n log log n / log n + log n sqrt n log log n) bits
  // In practice, we can have any s and b, and the needed amount of bits is
  // (n/s) log n + (n/b) log s + b 2^b log b bits
  // Optimizing it turns out that s should be exactly s = b log n
  // Optimizing b is more difficult but could be done numerically.
  // However, the exponential table does no more than popcounting, so why not
  // setting up a popcount algorithm tailored to the computer register size,
  // defining that size as b, and proceeding.

//unsigned char OnesInByte[] = 
const unsigned char popc[] =   //number of ones in one byte value [0..255].
{
0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8,
};

uint popcount (register uint x)

   { return popc[x&0xFF] + popc[(x>>8)&0xFF] + popc[(x>>16)&0xFF] + popc[x>>24];
   }

  
/******************************************************************/
// FUNCIONS DE EDU ...
/******************************************************************/
/* 
	Creates a bitmap and structures to rank and select 
*/

//bitmap createBitmapEdu (uint *string, uint n){  return createBitmap(string,n);}

bitmap createBitmap (uint *string, uint n){
    bitmap B;
    
	unsigned int nb;
	unsigned int ns;
	unsigned int countB, countS, blockIndex, superblockIndex;
    register unsigned int block;

	B =(struct sbitmap *) malloc (sizeof(struct sbitmap));
	B->data = string;
	B->n = n; 
	ns = (n/256)+1;
	nb = (n/32)+1;

    B->bSize = nb;
    B->sSize = ns;
	B->bdata =(byte *)malloc(nb*sizeof(byte));  //	Db = (unsigned char *)malloc(nb*sizeof(unsigned char));
	B->sdata = (uint*)malloc(ns*sizeof(uint));  // 	Ds = (unsigned int *)malloc(ns*sizeof(unsigned int));

	B->mem_usage = (ns*sizeof(uint)) + (nb*sizeof(byte)) + (sizeof(struct sbitmap));
	/* Ahora construimos los bloques */
    blockIndex = 0;
    superblockIndex = 0;
    countB = 0;
    countS = 0;

    while(blockIndex < nb) {
    	
       if(!(blockIndex%8)) {
          countS += countB;
          B->sdata[superblockIndex++] = countS;
          countB = 0;
       }
       
       B->bdata[blockIndex] = countB;
	   block = string[blockIndex++];
	   
	   countB += popcount(block);
    }

	B->pop = countS+countB;
	
//	{int i;     //fprintf(stderr,"\n");
//     for (i=0;i<ns;i++) {//fprintf(stderr,"%d ",B->sdata[i]);
//     	}
//     //fprintf(stderr,"\n");
//     for (i=0;i<8;i++) {//fprintf(stderr,"%d ",B->bdata[i]);
//     	}
//	}
    return B;
}	


/*
  Number of 1s in range [0,posicion]
*/
//uint rank1Edu(bitmap B, unsigned int position) {
//uint rank1Edu(bitmap B, unsigned int position) { return rank(B,position);}
uint rank(bitmap B, unsigned int position) {
    register unsigned int block;    
    if (position > B->n) return B->pop;    
	//position -=1;
	
	block = B->data[position/32] << (31-position%32);

    return B->sdata[position/256] + B->bdata[position/32] + 
           popc[block & 0xff] + popc[(block>>8) & 0xff] +
           popc[(block>>16) & 0xff] + popc[block>>24];
}   


/********************************************************************************************/
/**************************************************************************************/

static uint binsearch (uint *data, uint size, uint val)

   { uint i,j,m;
     i = 0; j = size;
     while (i+1 < j)
	{ m = (i+j)/2;
	  if (data[m] >= val) j = m;
	  else i = m;
	}
     return i;
   }

uint bselect (bitmap B, uint j)

   { uint spos,bpos,pos,word,x;
     byte *blk;
     if (j > B->pop) return B->n;
     spos = binsearch(B->sdata,(B->n+256-1)/256,j);
     
     //fprintf(stderr,"\n SPOS IS %d, and B->sdata[pos] = %d",spos,B->sdata[spos]);
     j -= B->sdata[spos];
     pos = spos<<8;
     blk = B->bdata + (pos>>5);
     bpos = 0;
    
    //while ((bpos < (1<<3)-1) && (blk[bpos+1] < j)) bpos++;
    while ( ((spos*8+bpos) < ((B->n-1)/W)) && (bpos < (1<<3)-1) && (blk[bpos+1] < j)) bpos++;
     
    
      //fprintf(stderr,"\n BPOS  = %d",bpos);
     pos += bpos<<5;
     word = B->data[pos>>5];
     j -= blk[bpos];
     //fprintf(stderr,"\n pos>>5 = %d ... pasou XXX con word = %d, and j= %d",pos>>5,word,j);
     while (1) 
    { x = popc[word & ((1<<8)-1)]; 
    	//fprintf(stderr,"\n word = %u popc vale %u",word & ((1<<8)-1),x);
	  if (j <= x) break;
	  j -= x; pos += 8;
	  word >>= 8;
	  
	}
	
     while (j) { if (word & 1) j--; word >>= 1; pos++; }

     //	fprintf(stderr,"\n\nBSELECT::: POSICIÓN FINAL = %u",pos-1);
     return pos-1;
     
   }


// destroys the bitmap, freeing the original bitstream
void destroyBitmap (bitmap B)

   { //free (B->data);
     free (B->bdata);
     free (B->sdata);
     free (B);
   }
   
   
// Prints the bit vector
void showBitVector(uint * V, int vectorSize) {
     uint bitIndex=0;
     while(bitIndex<vectorSize) {
        fprintf(stderr,"%d",bitget(V,bitIndex));
        bitIndex++;
     }	 
}   
  
void saveBitmap (char *filename, bitmap b) {
	int file;
	unlink(filename);
	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	write(file, &(b->sSize), sizeof(uint));
	write(file, b->sdata, sizeof(int) * (b->sSize));
	write(file, &(b->bSize), sizeof(uint));
	write(file, b->bdata, sizeof(byte) * (b->bSize));

	write(file, &(b->pop), sizeof(uint));
	write(file, &(b->n), sizeof(uint));
	close(file);		
}

/* loads the Rank structures from disk, and sets Bitmap->data ptr to "string"
*/
bitmap loadBitmap (char *filename, uint *string, uint n) {  
	bitmap B;
	int file;
	
	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot read file %s\n", filename);
		exit(0);
	}		
	
	B = (struct sbitmap *) malloc (sizeof(struct sbitmap));
	B->data = string;
	
	read(file, &(B->sSize), sizeof(uint));
	B->sdata = (uint *) malloc(sizeof(uint) * B->sSize);
	read(file, B->sdata, sizeof(uint) * B->sSize);

	read(file, &(B->bSize), sizeof(uint));
	B->bdata = (byte *) malloc(sizeof(byte) * B->bSize);
	read(file, B->bdata, sizeof(byte) * B->bSize);	
	
	read(file, &(B->pop), sizeof(uint));
	read(file, &(B->n), sizeof(uint));	
	close(file);
	B->mem_usage = (sizeof(uint) * B->sSize) + (sizeof(byte) * B->bSize) + (sizeof(struct sbitmap));
	
	if (n != B->n) {printf("\n LoadBitmap failed: %u distinto de %u",n,B->n); exit(0);}
	return B;
		
} 
   
   
/********************************************************************************************/
/********************************************************************************************/




	// creates a bitmap structure from a bitstring, which is shared

bitmap createBitmapGONZA (uint *string, uint n)
//bitmap createBitmap (uint *string, uint n)

   { bitmap B;
     uint i,j,pop,bpop,pos;
     uint s,nb,ns,words;
     B = (struct sbitmap *) malloc (sizeof(struct sbitmap));
     B->data = string;
     
   
 	 B->n = n; words = (n+W-1)/W;
     ns = (n+256-1)/256; nb = 256/W; // adjustments          
     
     B->bSize = ns*nb;
     B->bdata = (byte *) malloc (ns*nb*sizeof(byte));
     B->sSize = ns;
     B->sdata = (uint *)malloc (ns*sizeof(int));

	 B->mem_usage = (ns*sizeof(int)) + (ns*nb*sizeof(byte)) + (sizeof(struct sbitmap));
#ifdef INDEXREPORT
     printf ("     Bitmap over %i bits took %i bits\n", n,n+ns*nb*8+ns*32);
#endif 
	  //fprintf (stderr,"     Bitmap over %i bits took %i bits\n", n,n+ns*nb*8+ns*32);
     pop = 0; pos = 0;
     for (i=0;i<ns;i++)
	{ bpop = 0;
	  B->sdata[i] = pop;
	  for (j=0;j<nb;j++)
	     { if (pos == words) break;
	       B->bdata[pos++] = bpop;
	       bpop += popcount(*string++);
	     }
	  pop += bpop;
	}
     B->pop = pop;
     
		//     //fprintf(stderr,"\n");
		//     for (i=0;i<ns;i++) {//fprintf(stderr,"%d ",B->sdata[i]);
		//     	}
		//     //fprintf(stderr,"\n");
		//     for (i=0;i<ns*nb;i++) {//fprintf(stderr,"%d ",B->bdata[i]);
		//     	}
		     
     return B;
   }

	// rank(i): how many 1's are there before position i, not included

//uint rank (bitmap B, uint i)
uint rankGONZA (bitmap B, uint i)

   { 
   	i++;
   	if (i > B->n) return B->pop;
   	return B->sdata[i>>8] + B->bdata[i>>5] +
	    popcount (B->data[i>>5] & ((1<<(i&0x1F))-1));
   }


   
   
   
   
   



		


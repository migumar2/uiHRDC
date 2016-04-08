/* Range search data structure
 *
 * We use the idea of Chazelle mentioned in the paper, but make it faster
 * by making the set of points a true bijection (it may be not because we 
 * have permitted empty nodes in RevTrie). After it is a bijection we are
 * able to know beforehand the number of zeros at any segment. This also
 * gives a very convenient construction algorithm.
 */

#include "basics.h"
#include "static_bitsequence_brw32.h"
#include "range.h"

#define W (8*sizeof(unsigned int))
#define bitsW 5 /* Be aware of this */

/* reads bit p from e */
#define bitget1(e,p) ((e)[(p)/W] & (1<<((p)%W)))

/* sets bit p in e */
#define bitset(e,p) ((e)[(p)/W] |= (1<<((p)%W)))

/* cleans bit p in e */
#define bitclean(e,p) ((e)[(p)/W] &= ~(1<<((p)%W)))

namespace lz77index{
namespace basics{

struct srange{
    static_bitsequence_brw32 **levels;
    unsigned int nlev;
    unsigned int n;
};

/* builds the range data structure for values (x,y[x]), x=1..n-1
 * y gets freed
 */
range createRange(unsigned int *y, unsigned int n){
    range R;
    unsigned int *a,*ny;
    unsigned int pop,bit,i,pos,ymax;
    unsigned int *bitstr;
    int lev;
    /*bool zero;*/
	/* extend y so as to make it a bijection */
    ymax = 0;
    for(i=1;i<n;i++){
        if (y[i] > ymax) ymax = y[i];
    }
    a = (unsigned int*)malloc((1+ymax)*sizeof(unsigned int)); /* for now x = y^-1, after for swap */
    for(i=0;i<=ymax;i++) a[i] = ~0;
    for(i=1;i<n;i++) a[y[i]] = i;
    a[0] = 0;
    /*OLD:zero = false; */ /* cell 0 is free in y, use it */
    for(i=1;i<=ymax;i++){/*OLD: from i=0 */ 
	    if(a[i] == (unsigned int)~0){  
	        /*if(zero){*/
                a[i] = n++; 
            /*}else{ 
                a[i] = 0; 
                zero = true; 
            }*/
        }
    }
    y = (unsigned int*)realloc(y,n*sizeof(unsigned int));
    for(i=0;i<n;i++) y[a[i]] = i;
    ny = a;
	/* now build the levels */
    R = (range)malloc(sizeof(struct srange));
    R->n = n;
    R->nlev = bits(n-1);
    R->levels = (static_bitsequence_brw32**)malloc(R->nlev*sizeof(static_bitsequence_brw32*));
    lev = R->nlev; bit = 1<<lev;
    while(lev) {
        pop = 0;
        bitstr = (unsigned int*)calloc(((n+W-1)/W),sizeof(unsigned int));
        /*bitstr = (unsigned int*)malloc(((n+W-1)/W)*sizeof(unsigned int));*/
        for(i=0;i<n;i++){
            pos = y[i] & ~(bit-1);
	        if(y[i] & bit){
                pos += pop & (bit-1);
                pop++;
            }else{
                pos += (i-pop) & (bit-1);
            }
	        ny[pos] = y[i];
            if (y[i] & (bit>>1)) bitset(bitstr,pos);
            else bitclean (bitstr,pos);
        }
        R->levels[R->nlev-lev] = new static_bitsequence_brw32(bitstr, n, 20);
        free(bitstr);
        bit >>= 1;
        lev--;
	    a = y;
        y = ny;
        ny = a;
   }
   free(y);
   free(ny);
   return R;
}

/* important to understand what follows: 
 *      zpos = # of bits to the left of the current block, ypos = 2*zpos
 */

static void (*process)(void*, unsigned int);
static void *processData;

/* search when y1 and y2 are unimportant
 */
static void searchy12(static_bitsequence_brw32 **levels, unsigned int bit, unsigned int zpos, unsigned int x1, unsigned int x2){
    unsigned int z1,z2;
    int dz,dx;
    while(1){ /* tail recursion */
	    z1 = x1-(*levels)->rank1(x1-1);
        z2 = x2-(*levels)->rank1(x2-1);
	    dz = z2-z1;
	    dx = x2-x1;
	    levels++;
	    if(bit == 1){
            zpos <<= 1;
	        if(dz) process(processData,zpos);
	        if(dx-dz) process(processData,zpos+1);
	        return;
	    }
	    if(dz){ /* left branch exists */
            if (dx-dz){ /* right branch exists */
	            searchy12(levels,bit>>1,zpos,zpos+z1,zpos+z2);
	            dz = zpos+bit; 
                x1 += dz-z1; 
                x2 += dz-z2;
	 	        bit >>= 1; 
                zpos += bit;
	        }
	        else{ /* only left */
	            x1 = zpos+z1; 
                x2 = zpos+z2; 
                bit >>= 1; 
            }
    	}else{  /* only right */
	        dz = zpos+bit-z1;
            x1 += dz;
            x2 += dz;
		    bit >>= 1; 
            zpos += bit;
	    }
    }
}

/* search when y1 is as small as necessary
 */
static void searchy1(static_bitsequence_brw32 **levels, unsigned int bit, unsigned int zpos, unsigned int x1, unsigned int x2, unsigned int y2){
    unsigned int z1,z2;
    z1 = x1-(levels[0])->rank1(x1-1);
    z2 = x2-(levels[0])->rank1(x2-1);
    /* left branch */
    if(z2 > z1){ /* there are zeros, proceed */
	    if(bit == 1) process(processData,zpos<<1);
	    else if(y2 & bit) searchy12 (levels+1,bit>>1,zpos,zpos+z1,zpos+z2);
	    else searchy1(levels+1,bit>>1,zpos,zpos+z1,zpos+z2,y2 & ~bit);
    }
    if(y2 & bit){ /* right branch */
	    if(x2-x1 > z2-z1){ /* there are ones, proceed */
	        if(bit == 1) process(processData,(zpos<<1)+1);
	        else searchy1 (levels+1,bit>>1,zpos+(bit>>1),x1-z1+zpos+bit,x2-z2+zpos+bit, y2 & ~bit);
        }
    }
}

/* searches when y2 is as large as necessary
 */
static void searchy2(static_bitsequence_brw32 **levels, unsigned int bit, unsigned int zpos, unsigned int x1, unsigned int x2, unsigned int y1){ 
    unsigned int z1,z2;
    z1 = x1-(levels[0])->rank1(x1-1);
    z2 = x2-(levels[0])->rank1(x2-1);
    if(!(y1 & bit)){ /* left branch */
        if(z2 > z1){ /* there are zeros, proceed */
	        if(bit == 1) process(processData,zpos<<1);
            else searchy2(levels+1,bit>>1,zpos,zpos+z1,zpos+z2,y1);
	    }
	}
    /* right branch */
    if (x2-x1 > z2-z1){ /* there are ones, proceed */
	    if(bit == 1) process(processData,(zpos<<1)+1);
	    else if(!(y1 & bit)) searchy12(levels+1,bit>>1,zpos+(bit>>1),x1-z1+zpos+bit,x2-z2+zpos+bit);
	    else searchy2 (levels+1,bit>>1,zpos+(bit>>1),x1-z1+zpos+bit,x2-z2+zpos+bit, y1 & ~bit);
    }
}

static void search(static_bitsequence_brw32 **levels, unsigned int bit, unsigned int zpos, unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2){
    unsigned int z1,z2;
    z1 = x1-(levels[0])->rank1(x1-1);
    z2 = x2-(levels[0])->rank1(x2-1);
    if(!(y1 & bit)){ /* left branch */
        if(z2 > z1){ /* there are zeros, proceed */
            if(bit == 1) process(processData,zpos<<1);
            else if(y2 & bit) searchy2(levels+1,bit>>1,zpos,zpos+z1,zpos+z2,y1);
            else search(levels+1,bit>>1,zpos,zpos+z1,zpos+z2,y1,y2);
        }
    }
    if(y2 & bit){ /* right branch */
        if(x2-x1 > z2-z1){ /* there are ones, proceed */
            if(bit == 1) process(processData,(zpos<<1)+1);
            else if(!(y1 & bit)) searchy1(levels+1,bit>>1,zpos+(bit>>1),x1-z1+zpos+bit,x2-z2+zpos+bit, y2 & ~bit);
            else search (levels+1,bit>>1,zpos+(bit>>1),x1-z1+zpos+bit,x2-z2+zpos+bit,y1 & ~bit, y2 & ~bit);
        }
    }   
}
/*
//  original search: simple and clean, and not much slower
//
//static void search (bitmap *levels, unsigned int bit, unsigned int zpos,
//                    unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2,
//                    void (*process)(), void *processData)
//
//   { unsigned int z1,z2;
//     z1 = x1-rank(levels[0],x1);
//     z2 = x2-rank(levels[0],x2);
//     if (!(y1 & bit)) // left branch
//        { if (z2 > z1) // there are zeros, proceed
//             { if (bit == 1) process(processData,zpos<<1);
//               else search (levels+1,bit>>1,zpos,
//                            zpos+z1,zpos+z2,y1,min(y2,bit-1) & ~bit,
//                            process,processData);
//             }
//        }
//     if (y2 & bit) // right branch
//        { if (x2-x1 > z2-z1) // there are ones, proceed
//             { if (bit == 1) process(processData,(zpos<<1)+1);
//               else search (levels+1,bit>>1,zpos+(bit>>1),
//                            x1-z1+zpos+bit,x2-z2+zpos+bit,
//                            max(y1,bit) & ~bit,y2 & ~bit,
//                            process,processData);
//             }
//        }
//   }
*/

/* executes process(y) for each pair (x,y) stored in the rectangle [x1..x2] x [y1..y2]
 */
void searchRange(range R, unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2, void (*Pprocess)(void*, unsigned int), void *PprocessData){
     process = Pprocess;
     processData = PprocessData;
     search(R->levels,1<<(R->nlev-1),0,x1,x2+1,y1,y2);
}

/* frees the range data structure
 */
void destroyRange(range R){
    if(R==NULL)return; 
    if(R->levels != NULL){
        while(R->nlev--) 
            delete (R->levels[R->nlev]);
        free(R->levels);
    }
    free(R);
}

unsigned int saveRange(range R, FILE* fp){
    unsigned int i;
    if(fwrite(&(R->n),sizeof(unsigned int),1,fp)!=1)return 1;
    if(fwrite(&(R->nlev),sizeof(unsigned int),1,fp)!=1)return 1;
    for(i=0;i<R->nlev;i++){
        if(((R->levels)[i])->save(fp) != 0)return 1;
    }
    return 0;
}

range loadRange(FILE* fp){
    unsigned int i;
    range ret = (range)malloc(sizeof(struct srange));
    if(ret!=NULL){
        ret->levels = NULL;
        if(fread(&(ret->n),sizeof(unsigned int),1,fp)!=1){destroyRange(ret);return NULL;}
        if(fread(&(ret->nlev),sizeof(unsigned int),1,fp)!=1){destroyRange(ret);return NULL;}
        /*ret->levels = (bitmap*)malloc(ret->nlev*sizeof(bitmap));*/
        ret->levels = (static_bitsequence_brw32**)calloc(ret->nlev,sizeof(static_bitsequence_brw32));
        if(ret->levels==NULL){destroyRange(ret);return NULL;}
        for(i=0;i<ret->nlev;i++){
            (ret->levels)[i] = static_bitsequence_brw32::load(fp);
            if((ret->levels)[i]==NULL){printf("destroying %d\n",i);destroyRange(ret);return NULL;}
        }
    }
    return ret;
}

unsigned int sizeOfRange(range R){
        unsigned int s=0;
            for(unsigned int i=0;i<R->nlev;i++){
                        s += (R->levels[i])->size();
                            }
                return sizeof(struct srange)+s;
}

}
}


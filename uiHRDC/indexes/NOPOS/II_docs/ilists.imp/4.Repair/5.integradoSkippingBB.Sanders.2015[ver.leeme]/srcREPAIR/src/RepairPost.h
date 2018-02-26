
#ifndef REPAIRDECOMPRESSOR
#define REPAIRDECOMPRESSOR

#include <iostream>
#include <cassert>
#include "bitrankw32int.h"
#include "bitmapw32int.h"
#include "basic.h"
#include "sort_alg_Rep.h"

#define GAP 1

//#include "RePair.h" //** del build_index.cpp

/*
#define CEILLOG_2(x,v)                      \
{                                           \
    register unsigned _B_x = (x) - 1;       \
    (v) = 0;                                \
    for (; _B_x ; _B_x>>=1, (v)++);         \
}   

#define SAMPLEPERIOD(val, k, B, len, maxvalue) \
{	int value; \
	value = (maxvalue * B / len); \
	value = (value <= maxvalue)? value:maxvalue +1; \
	CEILLOG_2(value,k); \
	val = 1<<k; \
}
*/



#define CEILLOG_2(x,v)                      \
{                                           \
    register unsigned long _B_x = (x) - 1;       \
    (v) = 0;                                \
    for (; _B_x ; _B_x>>=1, (v)++);         \
}   

#define SAMPLEPERIOD(val, k, B, len, maxvalue) \
{	long value; \
	value = ((ulong)maxvalue * B / len); \
	value = (value <= maxvalue)? value:maxvalue +1;  /*modificado_fari_para_REPAIR...*/ \
	CEILLOG_2(value,k); \
	val = 1<<k; \
}



#define PTRBITS(val, ptrs, plen, id) \
 {uint tmp, bitsneeded; \
 	/*tmp = ptrs[i] - ptrs[i-1]; */ \
	tmp = GetField(ptrs, plen, id) - GetField(ptrs, plen, id-1); \
 	/*fprintf(stderr,"\n tmp %d",tmp);fflush(stderr);*/ \
 	/*if (tmp) { */ \
		CEILLOG_2((tmp+1),bitsneeded); \
		/* value 64 could be encoded in a list and 7 bits would be needed -> tmp = 64 +1 */ \
	/*} \
	else bitsneeded=0; */ \
	/*fprintf(stderr,"\n bitsneeded %d",bitsneeded);fflush(stderr);*/ \
	val = bitsneeded; \
 }
 
//number of buckets in current list.
#define NUMBUCKETS(val, maxval, k) \
{ val = (maxval + (1<<k) -1) /(1<<k);\
}


#define ZONEOCCSPTR(id, buck, numBucks, bits_Abs, bits_Ptr) \
    ( (buck==0)?(0):(bitread_32(zoneOccs, (occs[id]+ (numBucks-1)*bits_Abs + (buck-1)*bits_Ptr),bits_Ptr)))



#define ZONEOCCSABS(id, buck, bits_Abs) \
    ( (buck==0)?(0):(bitread_32(zoneOccs, (occs[id]+ (buck-1)*bits_Abs),bits_Abs)))


				
using namespace std;

	typedef struct {
		//uint *lenList;	
		uint nlists;		    //number of posting lists (equals to maxID+1)
		uint maxPostingValue;   //maximum value in any posting list (given as a source parameter)

		uint *zoneBitmaps;	//bitMap representing a large posting list (with maxPostingValue elements).
		uint bits_bitmaps;  //bits used by each bitmap
		uint ints_bitmaps_aligned ;  //uints needed by each bitmap, for them to be W-aligned into zoneOccs.
		uint numBitmaps;	//in number of ids treated as bitmaps.
		uint lenBitmapThreshold;   // such that list i receives bitmap if lenlist[i]> threshold.				
	}t_il;



class RepairPost {
  public:
  
	t_il ail;  
	t_il *il;   //represents the bitmap-type postings.  
    
    uint m; // Tamano final
    uint n; // Tamano original
    BitRankW32Int * BR; // 1->original, 0->reemplazado
    BitRankW32Int * BRR; // Se construye pal arbolito
    uint *symbols; // simbolos con reemplazos
    uint max_value;  // maximo valor del orginal,
    uint max_assigned;  // maximo valor asignado  (  = max_value + num reglas creado:: v�ase addRule)
    uint shift,min_value; 
    uint * symbols_new, symbols_new_len; // diccionario y su largo
    uint nbits;
    uint nodes, edges;
    uint e;


	uint * ptrs;
	uint ptrs_len;
	uint plen;
    
		uint * info;   //**VECTOR TEMPORAL USADO PARA EXTRAER S�MBOLOS (extract())
		uint pos;
		int prevval;
		
		
	
	BitMapW32Int * BMR; // Bitmap 
	uint * csymb_n, bits_sn;

		uint * RSS; //vector temporal usado para crear csymb_n que contiene info de skipping.
		
	uint *lenPost;		//len of each posting list


	////////////////////////////////////////////////////
	//sampling info.
	uint B;	// sample over the occurrences of each word (parameter for sampling)

	uint *occs;			//ptr to the begining of the occs of word i in zoneOccs.		
	uint *zoneOccs;		//bitVector with buckets [Abs|bcOff][Abs|bcOff]...
	uint zoneOccsSize;	//number of elements in zoneOccs
	uint bits_zoneOccsAbs;	//len of each value (in bits) of the absolute values.
							//pointers use a variable number of bits.	
	////////////////////////////////////////////////////
	size_t sizeUncompressed;       //Len of uncompressed set of lists (including lenlist)  
	int maxLenPosting;  //max len of any posting list.          



	//** Intersection with skipping_values **/
	uint *result;    //array to keep the result of the intersection. Initally the first list
	uint matches;   //number of matches found (elementos in result array)
	uint fi,fl;      //position in C for the 2nd list in the intersection.
	uint currVal;    //last expanded value in the 2nd list. 



	//RepairPost();					//**empty contructor
    RepairPost(char * file);   

		void readConstants(char *file);     
					
		void createAux(char * file);  //**creates the sampling structure during call to load()
		void load (char * file);   //**loads the created structure.

		void readLenPostings(char *file);
		void readSampleK(char *file);	
			
		void expand_build(uint v);
		uint lenList(uint v);

    ~RepairPost();
    uint size();

		void expand(uint v);
	    uint * adj(int v);
		
		uint * adj_il(int v, uint *noccs);   //the fist pos returned is the first element.
		void expand_il(uint v);	    
	    
	    uint outdegree(int v);

	uint sizetobits();
	uint sizetobitsLenPost();
	uint sizetobitsOccs();	

				    
	/** searches using skipping-data **/		    
	void expandAndSearchSkipping (uint v);	 

	int int2SkippingLookup(uint id1, uint id2, uint *noccs, uint **occs);    	
	int intNSkippingLookup(uint *ids, uint nids, uint *noccs, uint **occs); 

	int merge2 (uint id1, uint id2, uint *noccs, uint **occs);


	/** managing the bitmaps */
	//loads the structure containing the bitmap-type posting lists.
	int  load_bitmap( char *basename);
	int  free_bitmap ();
	uint bitmap_size();
	int  shouldUseBitmap( uint id);
	uint lenListBits(uint v);
	
	/** aux functions ******/
	int mergeNBitmap (uint *ids, uint nids, uint *noccs, uint **occurs);
	int fsearchBitmap (uint id, uint value);


	char * showFirst(uint id, uint n);
	
	/**** VOID METHODS !! **/              
    //** Some checking methods **/ 
	void showLastValueSkipping(uint node);
	void showFromLastSample(uint node);	

	/** stupid svs-unc. Just to check **/
	int fsearchUnc (uint id, uint value);
	int svs2Unc(uint id1, uint id2, uint *noccs, uint **occs);    	

};

#endif

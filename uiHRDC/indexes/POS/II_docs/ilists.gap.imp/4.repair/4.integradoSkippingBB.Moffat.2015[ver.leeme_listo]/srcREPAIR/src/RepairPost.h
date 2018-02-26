
#ifndef REPAIRDECOMPRESSOR
#define REPAIRDECOMPRESSOR

#include <iostream>
#include <cassert>
#include "bitrankw32int.h"
#include "bitmapw32int.h"
#include "basic.h"
#include "sort_alg_Rep.h"

#define GAP 1


typedef struct {
	uint id;
	uint gap;	
}
tsearchbitmap;

//#include "RePair.h" //** del build_index.cpp


#define CEILLOG_2(x,v)                      \
{                                        \
    register unsigned _B_x = (x) - 1;       \
    (v) = 0;                                \
    for (; _B_x ; _B_x>>=1, (v)++);         \
}  

#define SAMPLEPERIOD(val, ksample, len) \
{	uint tmp; \
	/*fprintf(stderr,"\n entro ceillog %d",len);fflush(stderr);*/ \
	CEILLOG_2(len,tmp); \
	/*fprintf(stderr,"\n salgo ceillog %d",tmp);fflush(stderr);*/ \
	if(!tmp) tmp=1; \
	val = ksample * tmp; \
	/*fprintf(stderr,"\n **SAMPLEP %d, sampleK %d",val,ksample);fflush(stderr);*/ \
}


		
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
  
  //	BitRankW32Int * BR_ptr_bitmap;
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
	uint sampleK;	// sample over the occurrences of each word (parameter for sampling)
	uint *occs;			//ptr to the begining of the occs of word i in zoneOccs.		
	uint *zoneOccs;		//bitVector with buckets [Abs|bcOff][Abs|bcOff]...
	uint zoneOccsSize;	//number of elements in zoneOccs
	uint bits_zoneOccs;	//len of each value (in bits) of the k-bitArray	
	////////////////////////////////////////////////////
	size_t sizeUncompressed;       //Len of uncompressed set of lists (including lenlist)  
	int maxLenPosting;  //max len of any posting list.          



	//** Intersection with skipping_values **/
	uint *result;    //array to keep the result of the intersection. Initally the first list
	uint matches;   //number of matches found (elementos in result array)
	uint fi,fl;      //position in C for the 2nd list in the intersection.
	int currVal;    //last expanded value in the 2nd list. 



	int GAP1,GAP2;

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


 uint expandAndSearch (uint v, uint *iniabsValue, uint searchedValue);
				    
	/** searches using skipping-data **/		    
	void expandAndSearchSkipping (uint v);	 
	int int2Skipping(uint id1, uint id2, uint *noccs, uint **occs);   
	int intNSkipping(uint *ids, uint nids, uint *noccs, uint **occs); 


	int int2SkippingSeq(uint id1, uint id2, uint *noccs, uint **occs);    
	//int int2SkippingBin(uint id1, uint id2, uint *noccs, uint **occs);    
	int int2SkippingExp(uint id1, uint id2, uint *noccs, uint **occs);  
	
	int intNSkippingSeq(uint *ids, uint nids, uint *noccs, uint **occs); 
	int intNSkippingExp(uint *ids, uint nids, uint *noccs, uint **occs); 
	//int intNSkippingExp2(uint *ids, uint nids, uint *noccs, uint **occcs);

	/** managing the bitmaps */
	//loads the structure containing the bitmap-type posting lists.
	int  load_bitmap( char *basename);
	int  free_bitmap ();
	uint bitmap_size();
	int  shouldUseBitmap( uint id);
	uint lenListBits(uint v);
	
	/** aux functions ******/
	int mergeNBitmap (uint *ids, uint nids, uint *GAPS, uint *noccs, uint **occurs);
	
	int fsearchBitmap (uint id, uint value);



	char * showFirst(uint id, uint n);
	
	/**** VOID METHODS !! **/              
    //** Some checking methods **/ 
	void showLastValueSkipping(uint node);
	void showFromLastSample(uint node);	

	/** stupid svs-unc. Just to check **/
	inline int fsearchUnc (uint id, uint value);
	int svs2Unc(uint id1, uint id2, uint *noccs, uint **occs);    	

	/** stupid svs-exp-search **/
	inline int fsearchGal (uint id, uint value);
	int svs2Exp(uint id1, uint id2, uint *noccs, uint **occs);    	
};

#endif

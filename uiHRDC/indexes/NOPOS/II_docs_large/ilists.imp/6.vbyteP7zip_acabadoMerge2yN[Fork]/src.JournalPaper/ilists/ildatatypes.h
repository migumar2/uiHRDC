

#ifndef DATA_TYPES_IL_INCLUDED
#define DATA_TYPES_IL_INCLUDED


	typedef struct{
		int (*defaultFsearch)    (void *ail, uint id, uint value);
		int (*defaultIntersect2) (void *ail, uint id1, uint id2, uint *noccs, uint **occs );
		int (*defaultIntersectN) (void *ail, uint *ids, uint nids, uint *noccs, uint **occs );
	}tdefFunctions;
	
		
	typedef struct {	
		uint nlists;		    //number of posting lists (equals to maxID+1)
		uint maxPostingValue;   //maximum value in any posting list (given as a source parameter)
		ulong sizeUncompressed;	//size of the index-assuming an uncompressed representation (uint **)

		uint *lenList;		//len of each list

		ulong *occs;			// ptr to the begining of the occs of word i in zoneOccs.				
				
		byte *bcs;			// byteCodes with differential encoding of gaps.
		ulong bcsSize;		// in bytes

		tdefFunctions defs; // functions used: fsearch /intersect2 / intersectN.

		char path2lzmaEncoder [256];
		uint *useslzma;     //a bitmap indicating if the list is encoded with vbyte or vbyte+lzma
		void *lzmadata;
		uint minbcssize;    //threshold used to check if using bytecodes+lzma is better than using bytecodes.
							// that is, only if |bytecodes for id| > "minbcssize", the check is performed.

		//keeps info of the previous search --> speeding up searches ??
		uint i;	//  position in the top level structure.
		uint bcpos; //  position inside bcs (botton level containing the compressed d-gaps).
		uint val; // value up to that position
		uint j;
		
	}t_il;


#endif



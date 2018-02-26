

#ifndef DATA_TYPES_IL_INCLUDED
#define DATA_TYPES_IL_INCLUDED

	typedef struct{
		int (*defaultFsearch)    ( uint id, uint value);
		//int (*defaultIntersect2) ( uint id1, uint id2, uint *noccs, uint **occs );
		//int (*defaultIntersectN) ( uint *ids, uint nids, uint *noccs, uint **occs );
		int (RepairPost::*defaultIntersect2)(unsigned int, unsigned int, unsigned int*, unsigned int**);
		int (RepairPost::*defaultIntersectN)(unsigned int *, unsigned int, unsigned int*, unsigned int**);
	}tdefFunctions;
	
		
	typedef struct {	
		//uint nlists;		    //number of posting lists (equals to maxID+1)
		//uint maxPostingValue;   //maximum value in any posting list (given as a source parameter)
		//uint sizeUncompressed;	//size of the index-assuming an uncompressed representation (uint **)

		tdefFunctions defs; // functions used: fsearch /intersect2 / intersectN.
		
		RepairPost *rp;

		//		uint lenBitmapDiv;
		//		uint maxMemRepair;
		//		uint replacesPerIter;
		//		double repairBreak;
		//		char filenameout[256];		
	}t_ilr;
		
#endif



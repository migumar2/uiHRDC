/*************************************************************************************************/
int getPositions (void *index,  uchar *pattern, ulong length, uint *ids, uint nids, uint **list, uint *len) {
	int retval;
	uint i;

	twcsa* wcsa = (twcsa *) index;	
	
	//intersecting lists
	uint *candDocs;
	uint numcandDocs;

	if (nids ==1) {
		retval= extractList_il(wcsa->ils, ids[0], &candDocs,&numcandDocs);
	}
	else if (nids ==2) {
		retval = intersect_2_il(wcsa->ils, ids[0],ids[1] ,&numcandDocs, &candDocs);
	}
	else {
		retval = intersect_N_il (wcsa->ils, ids,nids ,&numcandDocs, &candDocs);
	}
	IFERRORIL (retval);
	
	//fprintf(stderr,"\n there are %d candidate docs to be intersected",numcandDocs);
	//for (i=0;i<numcandDocs;i++) fprintf(stderr,"[%d]",candDocs[i]);
	//exit(0);

	if (!numcandDocs) {		
		*len =0;
		*list= NULL;
		return 0;
	}
	
	//printf("\nThe cnadidate docs are:");
	//for (i=0;i<numcandDocs;i++) {
	//	printf("[%d]",candDocs[i]);
	//}
	//printf("\n");		
	
	{	
		// ** 2 ** Perform the search inside the candidate docs with horspool;

		//allocating memory for the occurrences.
		#ifndef FREQ_VECTOR_AVAILABLE
		//	uint *positions = (uint *) malloc (sizeof(uint) * wcsa->maxNumOccs); //upper bound for number of occurrences
			uint *positions = (uint *) malloc (sizeof(uint) * wcsa->maxNumOccs*2); //upper bound for number of occurrences
		#endif

		#ifdef FREQ_VECTOR_AVAILABLE		
		//	uint *positions = (uint *) malloc (sizeof(uint) * wcsa->freqs[ids[0]]); //upper bound for number of occurrences
			uint *positions = (uint *) malloc (sizeof(uint) * wcsa->freqs[ids[0]]*2); //upper bound for number of occurrences
		#endif
	
	//uint *positions = (uint *) malloc (sizeof(uint) * 10000000);
	
	//NOTE:: HORSPOOL ON TEXT PLAIN COULD MATCH PARTIAL OCCURRENCES!! --> DOUBLE MEMORY ALLOCATE FOR POSITIONS!
	
		uint matches=0;

		uint doc_id;		
		byte *cbeg,*cend;
		
		byte d[256];
		inicialize_d(pattern, length, d);		
		
		unsigned char *doc; uint doclen;				
		
		//printf("\nApplying Horspool on the candidate docs:");
		for (i=0;i<numcandDocs;i++) {		
			doc_id = candDocs[i];	
			
			//printf("\nTo apply horspool in block %d, between pos %d and %d !!!",block, cbeg-text, cend-text);
			//call horspool
			{
				uint count;
				
				//wcsa->rp->extract_block(doc_id, doc, &extractedBytes);				
				//cbeg=doc; cend =cbeg + extractedBytes;
				//uint init_offset = doc_id * wcsa->blockSize;
				
//if (i<10)				printf("\n To extract  document doc_id= %u, maxNumoccs= %u", doc_id, wcsa->maxNumOccs);fflush(stderr);fflush(stdout);
				uint init_offset = extract_doc_representation (wcsa->ct, doc_id, &doc, &doclen);
//if (i<10)				printf("\n ini offset =  %u, endoffset= %u", init_offset, init_offset+doclen);fflush(stderr);fflush(stdout);
				cbeg=doc; cend =cbeg + doclen;
				//uint init_offset = 0 ;//wcsa->docbeginning[doc_id];
				
//				printf("\n To search inside document of size %lu bytes",(long)doclen); fflush(stderr);fflush(stdout);
				searchPositionsFullFile (init_offset, cbeg, cend, pattern, length, &(positions[matches]), &count, d);
				matches +=count;
				
			}		
			if (doc) free(doc);	
		}
		

		*list =positions;
		*len = matches;
	}
		
	free(candDocs);	
	return 0;		
}























/*************************************************************************************************/
/*
int getPositions_ONLY1_QGRAM (void *index,  uchar *pattern, ulong length, uint *ids, uint nids, uint **list, uint *len) {
	int retval;
	uint i;

	twcsa* wcsa = (twcsa *) index;	
	
	//intersecting lists
	uint *candDocs;
	uint numcandDocs;

	retval= extractList_il(wcsa->ils, ids[0], &candDocs,&numcandDocs);

//	if (nids ==1) {
//		retval= extractList_il(wcsa->ils, ids[0], &candDocs,&numcandDocs);
//	}
//	else if (nids ==2) {
//		retval = intersect_2_il(wcsa->ils, ids[0],ids[1] ,&numcandDocs, &candDocs);
//	}
//	else {
//		retval = intersect_N_il (wcsa->ils, ids,nids ,&numcandDocs, &candDocs);
//	}
	
	IFERRORIL (retval);
	
////fprintf(stderr,"\n there are %d candidate blocks to be intersected",numcandDocs);
////for (i=0;i<numcandDocs;i++) fprintf(stderr,"[%d]",candDocs[i]);
////exit(0);

//	if (!numcandDocs) {		
//		*len =0;
//		*list= NULL;
//		return 0;
//	}
	
////printf("\nThe cnadidate blocks are:");
////for (i=0;i<numcandDocs;i++) {
////	printf("[%d]",candDocs[i]);
////}
////printf("\n");		
	
	{	
		// ** 2 ** Perform the search inside the candidate blocks with horspool;

		//allocating memory for the occurrences.
		#ifndef FREQ_VECTOR_AVAILABLE
			uint *positions = (uint *) malloc (sizeof(uint) * wcsa->maxNumOccs); //upper bound for number of occurrences
		#endif

		#ifdef FREQ_VECTOR_AVAILABLE		
			uint *positions = (uint *) malloc (sizeof(uint) * wcsa->freqs[ids[0]]); //upper bound for number of occurrences
		#endif
	
		uint matches=0;

		uint block_id;		
		byte *cbeg,*cend;
		
		byte d[256];
		inicialize_d(pattern, length, d);		
				
		//printf("\nApplying Horspool on the candidate blocks:");
		byte *doc = (byte *) malloc (sizeof(byte) * (wcsa->blockSize + wcsa->q -1));
		
		ulong extractedBytes;
		for (i=0;i<numcandDocs;i++) {		
			block_id = candDocs[i];	
			
			//printf("\nTo apply horspool in block %d, between pos %d and %d !!!",block, cbeg-text, cend-text);
			//call horspool
			{
				uint count;
				wcsa->rp->extract_block(block_id, block, &extractedBytes);
				
				cbeg=block; cend =cbeg + extractedBytes;
				uint init_offset = block_id * wcsa->blockSize;
				
				searchPositionsFullFile (init_offset, cbeg, cend, pattern, length, &(positions[matches]), &count, d);
				matches +=count;
				
			}			
		}
		if (block) free(block);

		*list =positions;
		*len = matches;
	}
		
	free(candDocs);	
	return 0;		
}
*/


/** sorting a uint-vector descending, (O(n^2)) but valid as n<< **/
int simplesort(uint *V, uint n) {
	register int i,j;
	register uint max,tmp;
	for (i=0; i< n-1;i++) {
		max =i;
		for (j=i+1;j<n;j++) {
			if ( V[max] < V[j] ) max = j;
		}
		tmp =V[i];V[i]=V[max]; V[max]=tmp;	//swap	
	}
}

int checkSimpleSort() {
	
	{
	char text [100];
	uint V[100], i;
	uint len=100;;
		while (len !=0) {
			printf("\nIntro number of integers integer: ");
			fgets(text, 100, stdin);
			len = atoi(text);
			if (!len) break;
			i=0;
			while(i<len) {	
					printf("Intro integer [%d]: ",i);
					fgets(text, 100, stdin);
					V[i]=atoi(text);
					i++;
			}
			printf("\nWill sort vector: ");
			for (i=0;i<len;i++) printf("[%d]",V[i]);
			simplesort(V,len);	
			printf("\nSorted vector: ");	
			for (i=0;i<len;i++) printf("[%d]",V[i]);	
			printf("\n");				
		}		
	}
		
}






int fsearchPREV(void * index, int target, uint idforList)
{
	twcsa* wcsa = (twcsa *) index;
	uint l,r,m,val,num;
	uint pos,k;
	uint lenList = wcsa->lenList[idforList];
	uint sampleK = wcsa->occs.sampleK;
	uint bucksize = wcsa->occs.occsBucketSize;
	uint absBits = wcsa->occs.absOffsetSize;
	uint bcBits = wcsa->occs.bcOffsetSize;
	uint firstpos = wcsa->occs.occs[idforList];
	uint *zoneoccs = wcsa->occs.zoneOccs;
	byte *bcs = wcsa->occs.bcs;
	
	l = firstpos/bucksize; r= l+ (lenList +sampleK -1)/sampleK -1;
		
	//		printf("\n=============================================: target = %d, lenList = %d,%s ==",target,lenList,wcsa->words[idforList]);
	//		printf("\n firstpos = %d, lenList = %d, sampleK = %d, numbucks=%d, bucksize = %d",firstpos, lenList,sampleK, wcsa->occs.numBuckets,bucksize);fflush(stdout);
	//		printf("\n l= %d, r=%d, target = %d",l,r,target);fflush(stdout);
	
	//After the last sampled absolute pos
	val = bitread (zoneoccs, r*bucksize, absBits);  //the last Abs sample
	printf("\n ###valor en r*bucksize %d",val);
	if (val  <= target) {
		printf("\n ###ULTIMO CACHO: target = %d, val = %d",target,val);
			if (val ==target) return 1;
			pos = bitread (zoneoccs, r*bucksize + absBits , bcBits);
			l=0;
			r =lenList%sampleK;
			printf("\n\t\t l= %d, r=%d, target = %d",l,r,target);fflush(stdout);

			while (l<r) {  //decodes sequentially
				BC_TO_DEC(bcs, pos, num);
				val +=num;
				printf("\n\t\t\t %d pos",val);
				if (val ==target) { 
					printf("\n **found!!");
					return 1; // found!!.
				}
				if (val > target) { 
					printf("\n **not found!! %d",val);
					return 0; //not found
				}
				l++;	
			}
			printf("\n ***not found!!");
			return 0;
	}
	
	//Before the last sampled absolute pos
	//printf("\nTO BIN SEARCH:: l= %d, r=%d, val = %d, target =%d",l,r,val,target);fflush(stdout);
	//m = (l + r) / 2;	
	while (r>(l+1)) {
		m = (l + r) / 2;	
		pos = m*bucksize;
		val = bitread (zoneoccs, pos, absBits);
		if (val < target) l=m;
		else if (val==target) return 1;
		else r=m;
		//printf("\nINTO BIN SEARCH:: l= %d, m= %d, r=%d, val = %d, target =%d",l,m,r,val,target);fflush(stdout);
	}
	
 	//if (val==target) return 1;	
	//printf("\nend BIN SEARCH:: l= %d, m= %d, r=%d, val = %d, target =%d",l,m,r,val,target);fflush(stdout);
	
	pos = l*bucksize;
	val = bitread (zoneoccs, pos, absBits);
	if (target ==val) return 1;

		//printf("\nPREV DEC BCS:: l= %d, val = %d, target =%d",l,val,target);fflush(stdout);
		
	//sequential decompression of bcs
	pos = bitread (zoneoccs, pos + absBits , bcBits);	
	r=sampleK-1;
	for (k=0;k< r; k++) {
		BC_TO_DEC(bcs, pos, num); //num is the gap
		val +=num;
		//printf("\n   decoding BCS:: gap= %d, val = %d, target =%d, k = %d,r = %d",num,val,target,k,r);fflush(stdout);
		if (val == target) return 1;
		if (val > target) return 0; //not found			
	}
	return 0;
}


/**
  * Binary searches a target value in the list of the idforList-th word.
  */
int fsearch(void * index, int target, uint idforList)
{
	twcsa* wcsa = (twcsa *) index;
	uint l,r,m,val,num;
	uint pos,k;
	uint lenList = wcsa->lenList[idforList];
	uint sampleK = wcsa->occs.sampleK;
	uint bucksize = wcsa->occs.occsBucketSize;
	uint absBits = wcsa->occs.absOffsetSize;
	uint bcBits = wcsa->occs.bcOffsetSize;
	uint firstpos = wcsa->occs.occs[idforList];
	uint *zoneoccs = wcsa->occs.zoneOccs;
	byte *bcs = wcsa->occs.bcs;
	
	l = firstpos/bucksize; r= l+ (lenList +sampleK -1)/sampleK -1;
			
	//After the last sampled absolute pos: ==> sequential decompression after there.
	val = bitread (zoneoccs, r*bucksize, absBits);  //the last Abs sample
	if (val  <= target) {
		if (val ==target) return 1;
		pos = bitread (zoneoccs, r*bucksize + absBits , bcBits);
		
		l = 0;
		r = lenList%sampleK;  //number of codewords to decode (at most)
		while (l<r) {  //decodes sequentially
			BC_TO_DEC(bcs, pos, num);
			val += num;
			if (val ==target) return 1; // found!!.
			if (val > target) return 0; //not found
			l++;	
		}
		return 0;
	}
	
	//Before the last sampled absolute pos:: 
	//printf("\nTO BIN SEARCH:: l= %d, r=%d, val = %d, target =%d",l,r,val,target);fflush(stdout);
	while (r>(l+1)) {
		m = (l + r) / 2;	
		pos = m * bucksize;
		val = bitread (zoneoccs, pos, absBits);
		if (val < target) l = m;
		else if (val == target) return 1;
		else r = m;
	}
	
	//decoding from 
	pos = l * bucksize;
	val = bitread (zoneoccs, pos, absBits);
	if (target ==val) return 1;

		//printf("\nPREV DEC BCS:: l= %d, val = %d, target =%d",l,val,target);fflush(stdout);
		
	//sequential decompression of bcs
	pos = bitread (zoneoccs, pos + absBits , bcBits);	
	r=sampleK-1;
	for (k=0;k< r; k++) {
		BC_TO_DEC(bcs, pos, num); //num is the gap
		val +=num;
		//printf("\n   decoding BCS:: gap= %d, val = %d, target =%d, k = %d,r = %d",num,val,target,k,r);fflush(stdout);
		if (val == target) return 1;
		if (val > target) return 0; //not found			
	}
	return 0;
}




/** searches for "value" inside list idList 
	 returns 0 if not found,
	        >0 if found
	 !!very slow if a very frequent id is needed.
 */
int fsearchUNCOMPRESS(void *index, uint value, uint idList ) {
	uint *list;
	uint len,i;
	uncompressList (index, idList, &list, &len);
	//fprintf(stderr,"\n len of list uncompressed for fsearch = %d",len);
	for (i=0;i<len;i++){
		if (list[i] == value) {
			free(list);
			return 1;			
		}
	} 
	free(list);				
	return 0;	
}




/** Intersection of lists of occurrences **/
int svs (void *index, uint *idsIn, uint nids, uint **list, uint *len) {

	//sorting ids ==> lists are sorted by frequency !!
	uint i;
	uint *ids = (uint *)malloc (sizeof(uint) * nids);
	for (i=0;i<nids;i++) ids[i]=idsIn[i];
		simplesort(ids,nids);  // ==> change to sort by lenList !!
	
	/**starting SVS ------------------------ */		
	//Creating the first list;
	
	{	uint i,j,n;
		uint *firstlist;
		uint matches;
		uncompressList (index, ids[0], list, len);	
		firstlist =*list;
		n=*len;
		uint result;
		
		for (i=1; i<nids; i++) {  //the nids-1 remaining lists
			matches =0;
			for (j=0;j<n;j++) {
				result=fsearch (index, firstlist[j] , ids[i]);
				//result=fsearchUNCOMPRESS (index, firstlist[j] , ids[i]);
				
				if (result) {
				//if (fsearch (index, firstlist[j] , ids[i])) {
				//if (fsearchUNCOMPRESS (index, firstlist[j] , ids[i])) {
					
					firstlist[matches++]= firstlist[j];
					//printf("\nFSEARCH de posicion %d = acierto contra palabra ",firstlist[j]);
				}
				//else printf("\nFSEARCH de posicion %d = fallo contra palabra ",firstlist[j]);
			}
			n=matches;
			if (n ==0) {
				free(*list); *list=NULL;*len =0;
				break;				
			}
		}				
		*len =n;
		free(ids);
	}			
	return 0;
}


int getPositions (void *index, uint *ids, uint nids, uint **list, uint *len) {
//	int retval;
//	byte encodedPatt[MAX_PATTERN_CODE_SIZE];  //concatenates the codes of all the patterns.
//	uint lenPatt,i;
//	
//	
//	//intersecting lists
//	uint *candBlocks;
//	uint numCandBlocks;
//	retval= svs(index,ids,nids,&candBlocks,&numCandBlocks);	 //blocks that have to be traversed.
//	
//	if (!numCandBlocks) {		
//		*len =0;
//		return 0;
//	}
//	
//	//printf("\nThe cnadidate blocks are:");
//	//for (i=0;i<numCandBlocks;i++) {
//	//	printf("[%d]",candBlocks[i]);
//	//}
//	//printf("\n");		
//	//horspool on the candidate blocks
//
//	{	
//		// ** 1 ** concatenates the codewords of all the words in the phrase
//		ulong aCode,codeSize;  
//		lenPatt=0;
//		//printf("\ncontatenating the codes associated to the following ids:");
//		for (i=0;i<nids;i++) {			
//			//printf("[%d]",ids[i]);
//			getCode(ids[i],&aCode, &codeSize);						
//			getBytes(&(encodedPatt[lenPatt]), aCode, codeSize);		
//			lenPatt +=codeSize;
//		}
//		//printf("\ncontatenated codeword has size %d bytes and is <<:",lenPatt);
//		//for (i=0;i<lenPatt;i++) {printf("[%d]",encodedPatt[i]); }
//		//printf("\n");		
//	}
//	
//	{	
//		// ** 2 ** Perform the search inside the candidate blocks with horspool;
//
//		twcsa* wcsa = (twcsa *) index;
//		//allocating memory for the occurrences.
//		uint *positions = (uint *) malloc (sizeof(uint) * wcsa->freqs[ids[0]]); //max number of occurrents
//		uint matches=0;
//		uint blockSize =wcsa->blockSize;
//		uint textSize = wcsa->textSize;
//		byte *text = wcsa->text;
//
//		uint block;		
//		byte *cbeg,*cend;
//		uint offset;
//		
//		//printf("\nApplying Horspool on the candidate blocks:");
//		for (i=0;i<numCandBlocks;i++) {		
//			block = candBlocks[i];	
//			offset = block * blockSize; //beginning of the buffer
//			cbeg =text + offset;
//			cend =cbeg + blockSize;
//			if ((offset+blockSize) > textSize)
//				 cend = text+textSize;
//			
//			//sinchronize at the beggining of the buffer
//			if (block !=0) {
//				while (*cbeg >=128) cbeg--;  //until reaching a stopper (if not aligned to a codeword-beginning)
//				cbeg++; //next positon
//			}
//
//			//printf("\nTo apply horspool in block %d, between pos %d and %d !!!",block, cbeg-text, cend-text);
//			//call horspool
//			{
//				uint count;
//				searchPositionsFullFile (cbeg-text,cbeg, cend, encodedPatt, lenPatt, &(positions[matches]), &count);
//				matches +=count;
//			}
//			
//		}
//
//		*list =positions;
//		*len = matches;
//	}
//		
//	free(candBlocks);	
	return 0;		
}


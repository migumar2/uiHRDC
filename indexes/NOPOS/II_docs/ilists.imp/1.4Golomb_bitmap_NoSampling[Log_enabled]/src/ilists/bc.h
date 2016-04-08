


/* ##uncomment one of the following lines##... choosing how g_parameter is computed for each list */
	#define REGULAR_GOLOMB_RICE_TYPE
	//#define WWW2009_GOLOMB_RICE_TYPE

/* ##uncomment the following line if needed##... using golomb_decoding_tables or not */
	#define USE_DECODING_TABLES

/* ##uncoment one of the following options. */
	/** Table indexed by "unsigned char" **/
	#define GOL_TYPE (unsigned char)
	#define GOL_TYPE_BYTES (1)

	/** Table indexed by "unsigned shorts" **/
	//#define GOL_TYPE (unsigned short)
	//#define GOL_TYPE_BYTES (2)



/**** Constants transparent to the GOL_TYPE used ******/
#define MAX_GOLOMB_RICE_PARAMETER 32
#define GOL_BITS  (GOL_TYPE_BYTES*8)
#define GOL_BYTES (GOL_TYPE_BYTES)
#define GOL_TABLES_SIZE (1<< GOL_BITS)
#define OFFSET_LOWEST_VALUE (1)
//#define OFFSET_LOWEST_VALUE (1)     //AS "ZERO" IS NEVER ENCODED... value OFFSET_LOWEST_VALUE will 
								 	  //will be encoded as "0"... OFFSET_LOWEST_VALUE +1 as "1", etc.


//tables used to processing golomb-rice coded sequence in a bytewise fashion.
//different tables are requerid depending on the b-golomb-rice-parameter.
typedef struct {
	uint *nCodes;//[GOL_TABLES_SIZE];    //number of codes decoded in the following GOL_BITS bits.
	uint *nBitsDec;//[GOL_TABLES_SIZE];  //number of bits decoded for those nCodes.
	uint *sumVal;//[GOL_TABLES_SIZE];	 //sum of values decoded.
} tFastGolomb;

tFastGolomb *golombRiceTables;


/** function prototypes *******************************************************/
uint computeGolombRiceParameter_forList(uint *buff, uint n);
uint getWWW2009GolombRiceParameter_forList(uint maxID, uint n);

int initializeGolombRiceTables(tFastGolomb **tables, byte *used);
void freeGolombRiceTables(tFastGolomb *tables);
inline size_t rice_encode(uint *buf, size_t pos, uint val, uint nbits);
inline uint rice_decode(uint *buf, size_t *pos, unsigned nbits);
inline uint rice_size(uint val, uint nbits);

void show_Optimal_GOLOMB_works(uint ini, uint limit);
void check_GOLOMB_works2(uint ini, uint limit, uint golombB);
void check_GOLOMB_works_show(uint ini, uint limit);



/** Implementation of functions  ************************************************/

/* searches for the next value <= searchedValue.
 * given the buffer of codes, ppos the begining offset of the next code, n its ordinal number,
 * pprevvalue (sumation before the n-th code),decoding codes from the n-th to the (N-1)-th.
 * Updates: ppos, n, and val (the sumation of decoded gaps until the n-th).
 */
inline void lookupNext(uint *buffer, size_t *ppos, uint *nn, uint N, uint *pprevvalue, 
            	uint gol_bits, tFastGolomb *golombRiceTables, uint searchedVal) {	


                  
    size_t pos;     	
	uint index,n,nCodes,i,nBitsDec,sumVal;
	uint prevvalue;
	nCodes=0;nBitsDec=0;
	pos = *ppos;  
	n=*nn;  prevvalue = *pprevvalue;
	
	while ( (prevvalue < searchedVal) && (n < N) ){
		index = bitread(buffer, pos, GOL_BITS); 
		nCodes = golombRiceTables[gol_bits].nCodes[index];
		nBitsDec = golombRiceTables[gol_bits].nBitsDec[index];
		sumVal = golombRiceTables[gol_bits].sumVal[index];
		//nCodes=0;nBitsDec=0;
/*			
		fprintf(stderr,"\n nCodes = %d",nCodes);
		fprintf(stderr," nBitsDec = %d",nBitsDec);
		fprintf(stderr," sumVal = %d",sumVal);
		fprintf(stderr," index= %d, gol_bits = %d",index,gol_bits);
		fprintf(stderr,"(n,N = %d,%d), pos = %d",n,N,pos);
		fprintf(stderr,"\n prevvalue = %d , searchedVal = %d",prevvalue,searchedVal);
		fflush(stderr); fflush(stdout);		
*/

		if (n+nCodes < N) {
			if (nCodes > 0) {			
				if ( ((int)(prevvalue + sumVal)) <= searchedVal) {
					pos += nBitsDec;
					prevvalue += sumVal;
					n += nCodes;
				}
				else {			
					for (i=0 ; i < nCodes; i++) {
						sumVal = rice_decode(buffer, &pos, gol_bits);
						prevvalue += sumVal;
						n++;	
						if (prevvalue >= searchedVal) break;					
					}
				}
			}
			else { //decoding the next code.  /**isto ok cando nCodes = 0 **/
				sumVal=rice_decode(buffer, &pos, gol_bits);
				prevvalue += sumVal;		
				n++;
			}
		}
		else { // less than GOL_BITS remain to be decoded.
			while (n < N) { //normal decoding of the last bits.
				sumVal = rice_decode(buffer, &pos, gol_bits);
				prevvalue += sumVal;
				n++;
				if (prevvalue >= searchedVal) break;
			}
		}			
	}	
	*ppos = pos;  *nn = n; 	*pprevvalue = prevvalue;		
}

/*
#define lookupNextMacro(buffer,pos,n,N,prevvalue,gol_bits,golombRiceTables,searchedVal) \
{                                                                                       \
	uint index,nCodes,i,nBitsDec,sumVal;                                                \
	nCodes=0;nBitsDec=0;                                                                \
		while ( (prevvalue < searchedVal) && (n < N) ){                                 \
		index = bitread(buffer, pos, GOL_BITS);                                         \
		nCodes = golombRiceTables[gol_bits].nCodes[index];                              \
		nBitsDec = golombRiceTables[gol_bits].nBitsDec[index];                          \
		sumVal = golombRiceTables[gol_bits].sumVal[index];                              \
		if (n+nCodes < N) {                                                             \
			if (nCodes > 0) {                                                           \
				if ((prevvalue + sumVal) <= searchedVal) {                              \
					pos += nBitsDec; prevvalue += sumVal; n += nCodes;                  \
				}                                                                       \
				else {                                                                  \
					for (i=0 ; i < nCodes; i++) {                                       \
						sumVal = rice_decode(buffer, &pos, gol_bits);                   \
						prevvalue += sumVal; n++;                                       \
						if (prevvalue >= searchedVal) break;                            \
					}                                                                   \
				}                                                                       \
			}                                                                           \
			else {                                                                      \
				sumVal=rice_decode(buffer, &pos, gol_bits);                             \
				prevvalue += sumVal; n++;                                               \
			}                                                                           \
		}                                                                               \
		else {                                                                          \
			while (n < N) {                                                             \
				sumVal = rice_decode(buffer, &pos, gol_bits);                           \
				prevvalue += sumVal; n++;                                               \
				if (prevvalue >= searchedVal) break;                                    \
			}                                                                           \
		}                                                                               \
	}                                                                                   \
}

*/

//Creates the tables taht permit to fast process the encoded list during decoding 
//for each possible value of b in the range [1..maxGolombRiceParameter).
int initializeGolombRiceTables(tFastGolomb **tables, byte *used){
	uint b, i;
	uint buffer[2];
	uint memUsed = 0;
	tFastGolomb *t = (tFastGolomb *) malloc (MAX_GOLOMB_RICE_PARAMETER * sizeof(tFastGolomb));
	if (!t) return -1;  //not enough memory.
	//printf("\n *allocated %d bytes for the GolombRice-Decoding-Tables",(MAX_GOLOMB_RICE_PARAMETER * sizeof(tFastGolomb)));
	printf("\n *gol_bits = %d, gol_bytes = %d, gol_tables_size = %d",GOL_BITS, GOL_BYTES, GOL_TABLES_SIZE);

	memUsed += MAX_GOLOMB_RICE_PARAMETER * sizeof(tFastGolomb);
		
	printf("\n ** gol_Rice parameters used at least once: ");
	for (b=0;b<MAX_GOLOMB_RICE_PARAMETER;b++) {
		
		if (!used[b]) {
			t[b].nCodes= NULL;
			t[b].nBitsDec= NULL;
			t[b].sumVal= NULL;
			continue;
		}
		
		printf("[%d]",b);
		t[b].nCodes= (uint *) malloc (sizeof(uint)   * GOL_TABLES_SIZE);
		t[b].nBitsDec= (uint *) malloc (sizeof(uint) * GOL_TABLES_SIZE);
		t[b].sumVal= (uint *) malloc (sizeof(uint)   * GOL_TABLES_SIZE);
		
		memUsed += 3*(sizeof(uint) * GOL_TABLES_SIZE);
		
		//sets values for each posible entry_value in the tables of parameter value "b"
		for (i=0; i< GOL_TABLES_SIZE ; i++) {
			buffer[0]=0; buffer[1]=0;
			bitwrite (buffer, 0, GOL_BITS, i);
			
			size_t pos; 
			uint k, bitsD, sum, dec_val;
			pos=k=bitsD=sum=0;
			while (pos < ((size_t)GOL_BITS)) {
				dec_val = rice_decode(buffer, &pos, b);
				if (pos <= ((size_t)GOL_BITS)) {
					bitsD = (uint) pos;
					k++;
					sum +=dec_val;
				}
			}
			t[b].nCodes[i] = k;        //number of codes contained in the value of "i".
			t[b].nBitsDec[i] = bitsD;  //number of bits decoded (len of those codes).
			t[b].sumVal[i] = sum;      //sum of the values decoded.
			//printf("\n i=%6d, nCodes = %6d, nBits = %6d, sumVal = %6d",i,k,bitsD,sum);
		}
	}
	*tables = t;	

	printf("***");
	printf("\n *allocated %d bytes for the GolombRice-Decoding-Tables", memUsed);	
	return memUsed;
}

//frees the memory allocated
void freeGolombRiceTables(tFastGolomb *tables) {
	uint b;
	for (b=0; b<MAX_GOLOMB_RICE_PARAMETER; b++) {
		if (tables[b].nCodes!= NULL) free(tables[b].nCodes);
		if (tables[b].nBitsDec!= NULL) free(tables[b].nBitsDec);
		if (tables[b].sumVal!= NULL) free(tables[b].sumVal);		
	}	
	free(tables);
}

/***** FloorLog_2 and CeilLog_2  macros ****/
#define CEILLOG_2(x,v)                      \
{                                           \
    register ulong _B_x = (x) - 1;       \
    (v) = 0;                                \
    for (; _B_x ; _B_x>>=1, (v)++);         \
}                 

#define FLOORLOG_2(x,v)                     \
{                                           \
    register ulong _B_x = (x);           \
    (v) = 0;                                \
    for (; _B_x>1 ; _B_x>>=1, (v)++);       \
}               



/*****************************************************************************/
/** 
 *  Given a list of (n) integers (buff) [typically gap values from an inverted
 *  list], computes the optimal b parameter, that is, the number of bits that 
 *  will be coded binary-wise from a given encoded value.
 *    $$ val = \lfloor (\log_2( \sum_{i=0}^{n-1} (diff[i])) / n )) \rfloor $$
 * */
uint computeGolombRiceParameter_forList(uint *buff, uint n) {
	ulong total =0;
	register uint i;
	for (i=0; i<n;i++) total +=buff[i];
	//printf("\n total1 = %d ==> total2= %d/%d = %d",total,total,n, total/n);
	total /=n;
	uint val;
	FLOORLOG_2((total),val);
	//printf("\n ** Floor_log2(%d) =  %d **",total,val);
	
	return val;
}


/*****************************************************************************/
/** 
 *  Given a inverted list with "n" (freq) doc_IDs, and the "maxID" value, 
 *  computes the optimal b parameter, that is, the number of bits that 
 *  will be coded binary-wise from a given encoded value.
 *    $$ val = \lfloor (\log_2( (maxID - n) / (n+1) )) \rfloor $$
 * */
uint getWWW2009GolombRiceParameter_forList(uint maxID, uint n) {
	ulong total = ((ulong)(maxID-n))/(n+1);
	
//	fprintf(stderr,"\n ** maxID = %d, lenlist= %d **",maxID,n);
	uint val;
	FLOORLOG_2((total),val);
//	fprintf(stderr,"\n ** Floor_log2(%d) =  %d \n**",total,val);
	
	return val;
}

/*****************************************************************************/
/* encodes "val" into buf[pos..] using golombRice parameter = nbits.         */
inline size_t rice_encode(uint *buf, size_t pos, uint val, uint nbits)
{
	val-=OFFSET_LOWEST_VALUE;  //0 is never encoded. So encoding 1 as 0, 2 as 1, ... v as v-1
	uint w;
	bitwrite (buf, pos, nbits, val); pos+=nbits;
	
	for (w = (val>>nbits); w > 0; w--){
		bitwrite (buf, pos, 1, 1); pos++;
	}
	bitwrite (buf, pos, 1, 0); pos++;
	return pos;
}

/*****************************************************************************/
inline uint rice_size(uint val, uint nbits)
{
	val-=OFFSET_LOWEST_VALUE;   //0 is never encoded. So encoding 1 as 0, 2 as 1, ... v as v-1
	uint w,size;
	size=nbits;
	for (w = (val>>nbits); w > 0; w--){
		size++;
	}
	size++;
	return size;
}

/*****************************************************************************/
inline uint rice_decode(uint *buf, size_t *pos, unsigned nbits)
{
	unsigned int v;
	v = bitread (buf, *pos, nbits);
	*pos+=nbits;
	//printf("\n\t [decoding] v del bitread vale = %d",v);
	while (bitget(buf,*pos))
	{
		v += (1<<nbits);
		(*pos)++;
	}

	(*pos)++;
	return(v + OFFSET_LOWEST_VALUE);   //1 was encoded as 0 ... v as v-1 ... !!
}

/*
#define rice_decodeMacro(buf,pos,nbits,val) \
{                                           \
    register unsigned v;                    \
    (v) = bitread(buf,pos,nbits);           \
    for (; bitget(buf,pos) ; v += (1<<nbits), pos++);       \
    pos++;                                  \
    val = (v + OFFSET_LOWEST_VALUE);        \
}  
*/
/*****************************************************************************/


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

void print_bits(uint *buff, size_t pos, uint n, const char *prev) {
	size_t i;
	size_t last = n+pos;
	printf("%s[%d bits ==>",prev,n);
	for (i=pos;i<last;i++) {
		if (bitget(buff,i))
			printf("1");
		else 
			printf("0");
	}
	printf("]");
}


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/




void check_GOLOMB_works2(uint ini, uint limit, uint golombB){
	uint i;
	uint GolombParameter = golombB;
	
/*	
	for (i=0;i<limit;i++) {
		uint v,c;
		FLOORLOG_2(i,v);
		CEILLOG_2(i,c);
		printf("\n FLOOR_LOG_2[%d] = %d  CEIL_LOG_2[%d] = %d  ",i,v,i,c);
	}
*/



	printf("\n checking golomb compression: b= %d [INTS= %d..%d]",golombB,ini,limit-1);
	uint len = limit-ini;
	
	
	uint *buffer = (uint *) malloc (len*2 * sizeof(uint));
	uint *inbuffer  = (uint *) malloc ((len) * sizeof(uint));  //source numbers.
	uint *outbuffer = (uint *) malloc ((len) * sizeof(uint));  //decoded numbers.
		
	for (i=0;i<len*2;i++) buffer[i]= 0000;
	
	uint p=0;  
	for (i=ini;i<limit;i++) inbuffer[p++]=i;
	
	uint num,dec;
	
	//num=1; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6d is encoded with %d bytes",num,size);
	//num=1; size=rice_size(num,GolombParameter);printf("\n num %6d is encoded with %d bits ",num,size);

	//encoding
	ulong pos =0;
	for (i=0;i<len ;i++) {		
		num =inbuffer[i];
		pos = rice_encode(buffer, pos, num, GolombParameter);
	}

	size_t maxpos =pos;
	print_bits(buffer, 0, pos, "\n codigo generado:: ");

	//decoding
	pos=0;i=0;
	while (pos<maxpos){
		//BC_TO_DEC(code,pos,num);
		dec=rice_decode(buffer, &pos, GolombParameter);
		outbuffer[i++]=dec;
	}	
	
	
	//comparing the results.
	for (i=0;i<len;i++) {
		if (inbuffer[i] != outbuffer[i]) {
			printf("\n DISTINTOS %d!=%d!!!!",inbuffer[i],outbuffer[i]);
			return ;
		}
	}
	
	printf("\n Golomb encode/decode worked fine!\n");






	/************************/
	byte *used = (byte *) malloc (sizeof(byte) * MAX_GOLOMB_RICE_PARAMETER);
	for (i=0;i<MAX_GOLOMB_RICE_PARAMETER;i++) used[i]=1;
	
	printf("\n Now testing FastDecoding GolombRice-tables");
	if (-1 == initializeGolombRiceTables(&golombRiceTables,used)) {
		printf("\n unable to initialize golomb-Rice decoding tables");
		exit(0);
	}
	printf("\n golomb-Rice decoding tables initialized sucessfully"); fflush(stdout);
	free(used);


	//decoding
	pos=0;i=0;
	uint nCodes, nBitsDec, sumVal;
	uint index; size_t endPos;
	
	//uint golbbb = GOL_BITS;
	
	while (pos<(maxpos-GOL_BITS)){
		index = bitread(buffer, pos, GOL_BITS);
		nCodes = golombRiceTables[GolombParameter].nCodes[index];
		nBitsDec = golombRiceTables[GolombParameter].nBitsDec[index];
		sumVal = golombRiceTables[GolombParameter].sumVal[index];
		
		if (nCodes > 0) {
			endPos = pos + nBitsDec;
			while (pos < endPos) {
				dec=rice_decode(buffer, &pos, GolombParameter);
				printf("\n decoded: %d",dec);fflush(stdout);
				outbuffer[i++]=dec;
			}
		}
		else {
			dec=rice_decode(buffer, &pos, GolombParameter);
			printf("\n decoded: %d",dec);fflush(stdout);
			outbuffer[i++]=dec;			
		}
	}	

	while (pos < maxpos) { //normal decoding of the last bits.
		dec=rice_decode(buffer, &pos, GolombParameter);
		printf("\n decoded: %d",dec);fflush(stdout);
		outbuffer[i++]=dec;
	}		

	freeGolombRiceTables(golombRiceTables);

	//comparing the results.
	for (i=0;i<len;i++) {
		if (inbuffer[i] != outbuffer[i]) {
			printf("\n DISTINTOS %d!=%d!!!!",inbuffer[i],outbuffer[i]);
			return ;
		}
	}
		printf("\n sucess!!");
	/************************/
	
	free(buffer); free(inbuffer);free(outbuffer);
}




/*****************************************************************************/
void check_GOLOMB_works_show(uint ini, uint limit){
	uint i;
	uint GolombParameter = 4;

	uint *buffer = (uint *) malloc (limit*2 * sizeof(uint));	
	for (i=0;i<limit*2;i++) buffer[i]= 0000;
	
	uint num,dec, size;
	
	//num=1; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6d is encoded with %d bytes",num,size);
	num=1; size=rice_size(num,GolombParameter);printf("\n num %6d is encoded with %d bits ",num,size);



	for (i=ini;i<limit;i++) {
		size_t pos =0;
		num =i;
		//DEC_TO_BC(code, pos, num);
		pos = rice_encode(buffer, pos, num, GolombParameter);
		printf("\n pos = %6zu, num=%6d, golomb= %6d ",pos, num, GolombParameter); fflush(stdout);
		print_bits(buffer, 0, pos, ".. codigo:: ");
		pos =0;

		//BC_TO_DEC(code,pos,num);
		dec=rice_decode(buffer, &pos, GolombParameter);
		printf(", pos after decoding = %zu",pos);

		if (dec != num) {printf("\n DISTINTOS %d!=%d!!!!",num,dec);
			return ;
		}
	}	
	fflush(stderr);
	
	free(buffer);
}


/*****************************************************************************/
void show_Optimal_GOLOMB_works(uint ini, uint limit){
	uint i; uint GolombParameter;
	uint *buff = (uint *) malloc ((limit-ini) * sizeof(uint));
	uint z=0;
	for (i=ini;i<limit;i++) buff[z++]=i;
	GolombParameter = computeGolombRiceParameter_forList(buff, (limit-ini));
	
	printf("\n ** optimal parameter is %d **",GolombParameter);
	check_GOLOMB_works2(ini, limit, GolombParameter);
	
	free(buff);
}








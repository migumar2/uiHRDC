


/* ##uncomment one of the following lines##... choosing how g_parameter is computed for each list */
	#define REGULAR_GOLOMB_RICE_TYPE
	//#define WWW2009_GOLOMB_RICE_TYPE



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


/** function prototypes *******************************************************/
uint computeGolombRiceParameter_forList(uint *buff, uint n);
uint getWWW2009GolombRiceParameter_forList(uint maxID, uint n);


inline uint rice_encode(uint *buf, uint pos, uint val, uint nbits);
inline uint rice_decode(uint *buf, uint *pos, unsigned nbits);
inline uint rice_size(uint val, uint nbits);

void show_Optimal_GOLOMB_works(uint ini, uint limit);
void check_GOLOMB_works2(uint ini, uint limit, uint golombB);
void check_GOLOMB_works_show(uint ini, uint limit);



/** Implementation of functions  ************************************************/


/***** FloorLog_2 and CeilLog_2  macros ****/
#define CEILLOG_2(x,v)                      \
{                                           \
    register unsigned _B_x = (x) - 1;       \
    (v) = 0;                                \
    for (; _B_x ; _B_x>>=1, (v)++);         \
}                 

#define FLOORLOG_2(x,v)                     \
{                                           \
    register unsigned _B_x = (x);           \
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
	FLOORLOG_2(((uint)total),val);
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
	ulong total = (maxID-n)/(n+1);
	
//	fprintf(stderr,"\n ** maxID = %d, lenlist= %d **",maxID,n);
	uint val;
	FLOORLOG_2(((uint)total),val);
//	fprintf(stderr,"\n ** Floor_log2(%d) =  %d \n**",total,val);
	
	return val;
}

/*****************************************************************************/
/* encodes "val" into buf[pos..] using golombRice parameter = nbits.         */
inline uint rice_encode(uint *buf, uint pos, uint val, uint nbits)
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
inline uint rice_decode(uint *buf, uint *pos, unsigned nbits)
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

#define rice_decodeMacro(buf,pos,nbits,val) \
{                                           \
    register unsigned v;                    \
    (v) = bitread(buf,pos,nbits);           \
    for (; bitget(buf,pos) ; v += (1<<nbits), pos++);       \
    pos++;                                  \
    val = (v + OFFSET_LOWEST_VALUE);        \
}  

/*****************************************************************************/


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

void print_bits(uint *buff, uint pos, uint n, const char *prev) {
	uint i;
	uint last = n+pos;
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
	uint pos =0;
	for (i=0;i<len ;i++) {		
		num =inbuffer[i];
		pos = rice_encode(buffer, pos, num, GolombParameter);
	}

	uint maxpos =pos;
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
		uint pos =0;
		num =i;
		//DEC_TO_BC(code, pos, num);
		pos = rice_encode(buffer, pos, num, GolombParameter);
		printf("\n pos = %6d, num=%6d, golomb= %6d ",pos, num, GolombParameter); fflush(stdout);
		print_bits(buffer, 0, pos, ".. codigo:: ");
		pos =0;

		//BC_TO_DEC(code,pos,num);
		dec=rice_decode(buffer, &pos, GolombParameter);
		printf(", pos after decoding = %d",pos);

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








/****************************************************************************/
/** delete after the changes .........                                     **/
/****************************************************************************/
#define DEC_TO_BC(bc, pos, num)             \
{ while (num >= 128) {                      \
 bc[pos++] = (byte)((num & 127) + 128);     \
 num = (num >> 7);                          \
}                                           \
bc[pos++] = (byte) num;                     \
}

#define SIZE_DEC_TO_BC(num,size)            \
{uint numtmp=num;                           \
  size=0;                                   \
  while (num >= 128) {                      \
   size++;                                  \
   num = (num >> 7);                        \
  }                                         \
size++; num=numtmp;                         \
}			
			
#define BC_TO_DEC(bc,pos,num) \
 { \
 	 register uint _pot, _x; \
 	 _pot=1; \
 	 _x = bc[pos++]; \
 	 num=0; \
 	 while (_x >= 128) { \
 	 	num = num + (_x-128)*_pot; \
 	 	_x=bc[pos++]; \
 	 	_pot<<=7; \
 	 } \
 	 num=num + _x*_pot; \
 }

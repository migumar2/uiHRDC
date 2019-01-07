#include "ilist.h"
#include "LZend.nuevo.h"
#include "extractorend2.h"
#include <stdlib.h>
#include <cstring>
#include "utils.h"
#include "copyfile.cpp"


#define DOCid_ADD (1)



/** fari ****** INI BC MODIFIED (WITH NO 0)*************/

//Bytecodes (BC) and ByteCodesModified(BC0) so that [0] does not occur in the compressed secuence

// Explanation:
/* Since the [0] byte value is only used to encode the number "0" in bytecodes,
 * we can implement code=encBC0(n) as code=encBC(n+1), and n=decBC0(code) as n=  decBC(code)-1
 */

#define OFFSET_LOWEST_VALUE (-1)     // 0 is encoded as BC(1), 1 as BC(2), ...
//#define OFFSET_LOWEST_VALUE (0)    // BC0 becomes BC


/*
 num      0 is encoded with 1 bytes:[1]
 num      1 is encoded with 1 bytes:[2]
 num      2 is encoded with 1 bytes:[3]
 num      3 is encoded with 1 bytes:[4]
 ..
 num    126 is encoded with 1 bytes:[127]
 num    127 is encoded with 2 bytes:[128][1]
 num    128 is encoded with 2 bytes:[129][1]
 ...
 num    254 is encoded with 2 bytes:[255][1]
 num    255 is encoded with 2 bytes:[128][2]
 num    256 is encoded with 2 bytes:[129][2]
 ...
 num    382 is encoded with 2 bytes:[255][2]
 num    383 is encoded with 2 bytes:[128][3]
 num    384 is encoded with 2 bytes:[129][3]
 ...
 num  16382 is encoded with 2 bytes:[255][127]
 num  16383 is encoded with 3 bytes:[128][128][1]
 num  16384 is encoded with 3 bytes:[129][128][1]
 num  16385 is encoded with 3 bytes:[130][128][1]
 num  16386 is encoded with 3 bytes:[131][128][1]
 ...
 num  16510 is encoded with 3 bytes:[255][128][1]
 num  16511 is encoded with 3 bytes:[128][129][1]
 num  16512 is encoded with 3 bytes:[129][129][1]
 ...
 ...
 num 2097149 is encoded with 3 bytes:[254][255][127]
 num 2097150 is encoded with 3 bytes:[255][255][127]
 num 2097151 is encoded with 4 bytes:[128][128][128][1]
 num 2097152 is encoded with 4 bytes:[129][128][128][1]
 num 2097153 is encoded with 4 bytes:[130][128][128][1]

*/

#define INT_TO_BC(bc, pos, num)             \
{num-=OFFSET_LOWEST_VALUE;                  \
 while (num >= 128) {                       \
 bc[pos++] = (unsigned char)((num & 127) + 128);     \
 num = (num >> 7);                          \
}                                           \
bc[pos++] = (unsigned char) num;            \
num+=OFFSET_LOWEST_VALUE;                  \
}

#define SIZE_DEC_TO_BC(num,size)            \
{uint numtmp=num;                           \
  num-=OFFSET_LOWEST_VALUE;                 \
  size=0;                                   \
  while (num >= 128) {                      \
   size++;                                  \
   num = (num >> 7);                        \
  }                                         \
size++; num=numtmp;                         \
}

#define BC_TO_INT(bc,pos,num) \
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
      num+=OFFSET_LOWEST_VALUE; \
 }
/** fari ****** END BC MODIFIED (WITH NO 0)*************/



/** Sebas ******* version 2011 fallaba *******/

  
#define INT_TO_BCx(bc, pos, num)             \
{while (num >= 127) {                       \
 bc[pos++] = (unsigned char)((num % 127) + 128);     \
 num = (num / 127)-1;                          \
}                                           \
bc[pos++] = (unsigned char) (num+1);                     \
}
			
#define BC_TO_INTx(bc,pos,num) \
 { \
 	 register uint _pot, _x; \
 	 _pot=1; \
 	 _x = bc[pos++]; \
 	 num=0; \
         bool loop=false;\
 	 while (_x >= 128) { \
		loop=true;\
 	 	num = num + (_x-128)*_pot; \
 	 	_x=bc[pos++]; \
 	 	_pot*=127; \
 	 } \
	 if(loop)\
	 	 num=num + (_x)*_pot; \
	 else\
	 	 num=num + (_x-1)*_pot; \
 }

/** Sebas end ******* version 2011 fallaba *******/

 

/** *******************************************************************************
    * function used to parse the source posting lists that are loaded in build_il.*
    * Refer to "build_il() comments below" for a more detailed description        *
    * Basically, it reads the len of each posting list and its values.
		// *[Nwords=N]
		// *[maxPostvalue]
		// *[lenPost1][post11][post12]...[post1k�]
		// *[lenPost2][post21][post22]...[post2k��]
		// * ... 
		// *[lenPostN][postN1][postN2]...[postNk���]		
		//
	* It recreates array lenList, and uint ** occList and variables maxPostValue and nlists. */
	
int	parseAllPostingList(uint *nlists, uint **lenList, uint ***occList, 
                        uint *maxPostValue, uint *source, uint length){

	uint **occs;
	uint *lens;
	uint n;
	
	register uint i,j,sourcepos,len;
	
	if (length<2) return 1;
	
	n=source[0];
	*maxPostValue = source[1] + DOCid_ADD;
	
	//allocating memory.
	lens = (uint *) malloc (sizeof(uint) * n);
	occs = (uint **) malloc (sizeof(uint *) * n);
	
	sourcepos=2;
	
	uint maxpostL =0;
	for (i=0; i<n; i++) {
		lens[i]= len = source[sourcepos++];	
		occs[i]= (uint *) malloc (sizeof(uint) * len);
		for (j=0; j<len;j++) {
			occs[i][j] = DOCid_ADD + source[sourcepos++];
			if (occs[i][j] > maxpostL) maxpostL = (occs[i][j]);
		}
	}
	
	if (maxpostL > *maxPostValue) *maxPostValue = maxpostL;
	printf("\n **** maxpostSource = %d, maxpostComputed = %d",*maxPostValue,maxpostL);
	fflush(stdout);
	
	if (length !=sourcepos) return 31;
	
	*nlists =n;
	*lenList = lens;
	*occList = occs;	
	return 0;
}
 
 
 
 
 
 
 
 
 
/***********************************************************************************/
/*** FUNCTIONS USED FOR PARSING PARAMETERS FROM COMMAND LINE ***********************/
/* Three function to variables to manage parameters */
static bool is_delimeter(char *delimiters, char c) {
   int i=0,len_delimiters=strlen(delimiters);
   bool is=false;
   for (i=0;i<len_delimiters;i++)
     if (c == delimiters[i]) is=true;
   return is;
}


static void parse_parameters(char *options, int *num_parameters, char ***parameters, char *delimiters) {
  int i=0,j=0,temp=0,num=0, len_options=strlen(options);
  char *options_temp;
  while  (i<len_options) {
    while ((i<len_options) && is_delimeter(delimiters,options[i])) i++;
    temp=i;
    while ((i<len_options) && !is_delimeter(delimiters,options[i])) i++;
    if (i!=temp) num++;
  }
  (*parameters) = (char **) malloc(num*sizeof(char * ));
  i=0;
  while  (i<len_options) {
    while ((i<len_options) && is_delimeter(delimiters,options[i])) i++;
    temp=i;
    while ((i<len_options) && !is_delimeter(delimiters,options[i])) i++;
    if (i!=temp) {
      (*parameters)[j]=(char *) malloc((i-temp+1)*sizeof(char));
      options_temp = options+temp;
      strncpy((*parameters)[j], options_temp, i-temp);
      ((*parameters)[j])[i-temp] = '\0';
      j++;
    }
  }
  *num_parameters = num;
}

static void free_parameters(int num_parameters,char ***parameters) {
  int i=0;
  for (i=0; i<num_parameters;i++)
    free((*parameters)[i]);
  free((*parameters));
}
 /**********************************************************************************************/
 
uint encode(uint val){
	uint r = 0;
	uint m = 1;
	for(uint i=0;i<4;i++){
		r += ((val%255)+1)*m;
		val = val/255;
		m*=256;
	}
	return r;
}
uint decode(uint val){
	uint r = 0;
	uint m = 1;
	for(uint i=0;i<4;i++){
		r += ((val%256)-1)*m;
		val = val/256;
		m*=255;
	}
	return r;
}
/***************************************************************************/
/** Creates a compressed representation of a collection of text documents **/
/***************************************************************************/

	/* Creates the [compressed] representation of a collection of text documents
	  whose text is passed as text[0..length-1]. The offset of the document 
	  beginnings are given by docBeginnings[0..numdocs-1], whereas 
	  docBeginnings[numdocs]== length.
	  
	  Note that the *representation* is an 
	  opaque data type. Any build option must be passed in string 
	  build_options, whose syntax depends on the text representation. 
	  The compressed representation must always work with some default 
	  parameters if build_options is NULL. 
	  The returned representation is ready to be decompressed and queried
	  for statistics. */

/*
static char tmp_filename[] = "tmp_filename_il";
static char tmp_filename_rev[] = "tmp_filename_il.rev";
static char tmp_filename_sa[] = "tmp_filename_il.rev.sa";
static char tmp_filename_char[] = "tmp_filename_il.char";
static char tmp_filename_len[] = "tmp_filename_il.len";
static char tmp_filename_sources[] = "tmp_filename_il.start";
static char tmp_filename_doc[] = "tmp_filename_il.doclen";
static char tmp_filename_encodedlen[] = "tmp_filename_il.encodedlen";*/

static char tmp_filename[256];
static char tmp_filename_rev[256];
static char tmp_filename_sa[256];
static char tmp_filename_char[256];
static char tmp_filename_len[256];
static char tmp_filename_sources[256];
static char tmp_filename_doc[256];
static char tmp_filename_encodedlen[256];

char *error_il (int e){
    return NULL;
}
/* Building the inverted list structure */



    /* Creates a structure for the management of inverted lists from a
      given set of posting lists. 
      Array source has the following format...
      [Nwords=N][maxPostvalue][lenPost1][post11][post12]...[post1k]
                              [lenPost2][post21][post22]...[post2k]
                               ... 
                              [lenPostN][postN1][postN2]...[postNk]
      il is an opaque data type. Any build option must be passed in string 
      build_options, whose syntax depends on the index. The index must 
      always work with some default parameters if build_options is NULL. 
      The returned structure is ready to be queried. */

unsigned int* sourceToText(uint* source, uint length,uint *bcs_encodedLen_n,  char* filename){
	printf("len all postings %d, num words %d\n",length,source[0]);
    uint tlength=0;
    uint* text = (uint*) malloc(sizeof(uint)*(length-2-source[0]));
    uchar* text2 = (uchar*) malloc(sizeof(uint)*(length-2-source[0]));
    uint* encoded_len =  new uint[source[0]];
    uint pos = 2; 
    uint bc_pos=0;
    //uint old_bc_pos=0;
    //uint total_bc_len = 0;
    uint j=0;

    uint tmp_val;
    while(pos<length){
        uint len_il = source[pos];
        //	text[tlength]=0;
        //	tlength+=1;
        /*for(uint i=0;i<len_il;i++){
            text[tlength] = source[pos+i+1];
            tlength += 1;
        }*/
        //compute deltas
        //text[tlength] = encode(source[pos+1]);
        text[tlength] = source[pos+1];
        INT_TO_BC(text2, bc_pos, text[tlength]);
        tlength+=1;
        for(uint i=1;i<len_il;i++){
            //text[tlength] = encode(source[pos+i+1]-source[pos+i]);
            text[tlength] = source[pos+i+1]-source[pos+i];
            INT_TO_BC(text2, bc_pos, text[tlength]);
            tlength += 1;
        }
        //encoded_len[j] = bc_pos - old_bc_pos;
        encoded_len[j] = bc_pos;
	//old_bc_pos = bc_pos;
        //total_bc_len += encoded_len[j];
        pos = len_il + pos + 1;
	j++;
    }
    
    //encode values to avoid null bytes
	/*for(uint i=0;i<tlength;i++){
		text[i] = encode(text[i]);
	}*/
    //encode them using BC
    /*uint pos=0;
    for(uint i=0;i<tlength;i++){
        INT_TO_BC(text2, pos, text[i]);   
    }*/
    //saveText((uchar*)text, tlength*sizeof(uint), tmp_filename);
    //saveText(text2, total_bc_len, tmp_filename);
    
    fprintf(stderr,"\n **SOURCE POSTINGS = %lu ints = %lu bytes --> d-gaped+vbyted seq = %lu bytes **",(ulong)length , (ulong)length*sizeof(uint), (ulong)bc_pos);
    
    saveText(text2, bc_pos, tmp_filename);   //######## !! secuencia a indexar !!
    free(text);
    free(text2);
    *bcs_encodedLen_n=bc_pos;
    return encoded_len;                      //######## !! offsets de inicio de cada posting list !!
}

unsigned int* sourceToLen(uint* source, uint length, uint* numlists, uint* maxLength){
    uint tlength=0;
    uint* text = new uint[source[0]];
    uint pos = 2; 
    *maxLength = 0;
    while(pos<length){
        uint len_il = source[pos];
        text[tlength]=len_il;
        tlength += 1;
	    if(len_il>*maxLength)
            *maxLength = len_il;
        pos = len_il + pos + 1;
    }
    *numlists = source[0];
    return text;
}



int build_il (uint *source, uint length, char *build_options, void **ail){
  
  uint delta_sampling = DELTA_SAMPLING; //default value = 16
  {	/** processing the parameters only delta_sampling is required*/
	char delimiters[] = " =;";
	int j,num_parameters;
	char ** parameters;
	
	if (build_options != NULL) {
	parse_parameters(build_options,&num_parameters, &parameters, delimiters);
	for (j=0; j<num_parameters;j++) {
	  if  ((strcmp(parameters[j], "delta_sampling") == 0 ) && (j < num_parameters-1) ) {
	    delta_sampling=atoi(parameters[j+1]);
	    j++;
	  }
	  else if ((strcmp(parameters[j], "other_parameters") == 0 ) && (j < num_parameters-1) ) {
	    //lenBitmapDiv=atoi(parameters[j+1]);	    
	    j++;

	  }
	}
	free_parameters(num_parameters, &parameters);
	}
	
	printf("\n \t **parameters of method: delta_sampling= %u (default = %u)\n", delta_sampling, DELTA_SAMPLING);    
	fflush(stdout);
  }
  
    sprintf(tmp_filename,"%s.%u", "tmp_filename_il" ,getpid());
    sprintf(tmp_filename_rev,"%s.%u.%s", "tmp_filename_il" ,getpid(),"rev");
    sprintf(tmp_filename_sa,"%s.%u.%s", "tmp_filename_il" ,getpid(),"rev.sa");
    sprintf(tmp_filename_char,"%s.%u.%s", "tmp_filename_il" ,getpid(),"char");
    sprintf(tmp_filename_len,"%s.%u.%s", "tmp_filename_il" ,getpid(), "len");
    sprintf(tmp_filename_sources,"%s.%u.%s", "tmp_filename_il" ,getpid(), "start");
    sprintf(tmp_filename_doc,"%s.%u.%s", "tmp_filename_il" ,getpid(),"doclen");
    sprintf(tmp_filename_encodedlen,"%s.%u.%s", "tmp_filename_il" ,getpid(),"encodedlen");
    unlink(tmp_filename);
    unlink(tmp_filename_rev);
    unlink(tmp_filename_sa);
    unlink(tmp_filename_char);
    unlink(tmp_filename_len);
    unlink(tmp_filename_sources);
    unlink(tmp_filename_doc);
    unlink(tmp_filename_encodedlen);

    
    
    
    
      std::cout<<"  ilist.cpp::call to sourceToText(- len "<< length << " --)  \n*********************"<<std::endl;
  
    unsigned int bcs_encodedLen_n;
    unsigned int* encoded_len = sourceToText(source, length, &bcs_encodedLen_n, tmp_filename);
    //saveText(text, length, tmp_filename);

    
      std::cout<<"  ilist.cpp::call to new LZEnd2(---)  \n*********************"<<std::endl;
    LZparser* p;
    p = new LZEnd2(tmp_filename);
    p->parse();
    
    lzi* repr = (lzi*)malloc(sizeof(lzi));
    
    repr->bcs_encodedLen_n = bcs_encodedLen_n;
    repr->delta_sampling = delta_sampling;

    std::cout<<"  ilist.cpp::call to ExtractorEnd2(---)  \n*********************"<<std::endl;    
    //repr->extractor = new ExtractorEnd2(tmp_filename, DELTA_SAMPLING);  //definido en ilist.h
    repr->extractor = new ExtractorEnd2(tmp_filename, repr->delta_sampling);  //definido en ilist.h, fari 2013!!
    
    unsigned int numlists;

          std::cout<<"  ilist.cpp::call to sourceToLen(- len:"<< length << " numlists: " << numlists <<  " --)  \n*********************"<<std::endl;
    unsigned int* doc_len = sourceToLen(source, length, &numlists, &(repr->maxLength));
    
    repr->docs = numlists;
    //repr->doc_array = new DeltaCiodes(doc_len,numlists, DELTA_SAMPLING);
	printf("Saving doc lens\n");
    saveText((unsigned char*)doc_len, repr->docs*sizeof(unsigned int), tmp_filename_doc);
	printf("Saving doc encoded lens\n");
    saveText((unsigned char*)encoded_len, repr->docs*sizeof(unsigned int), tmp_filename_encodedlen);
	printf("Saved\n");
    //delete[] doc_len;
    //delete[] encoded_len;
	repr->encodedLen = encoded_len;
	
	//repr->docLen = doc_len;
	{//encoding the len of each posting list into "lenlist" in bit-wise fashion
	repr->lenlistbits = bits(repr->maxLength);
	repr->lenlistnints  = uint_len_LL(repr->lenlistbits , repr->docs);
	repr->lenlist = (uint *) malloc (sizeof(unsigned int) * repr->lenlistnints);
	repr->lenlist[repr->lenlistnints-1]=0000;
	uint i;
	size_t pos = 0;
	for (i=0; i< repr->docs ; i++) {
		 set_field_LL(repr->lenlist, repr->lenlistbits, pos, doc_len[i]); pos ++;
	 }
			 
	}// end lenlist
	free(doc_len);
	
    *ail = repr;


	

  
    /******************* FARI 2013.05.20 *********************************************/
		/** checking that the lists are decoded successfully *************/

		fprintf(stderr,"\n ! CHECKING that the decoded lists are identical to the original ones. ");
		fflush(stderr);fflush(stdout);
		uint numberFails=0; 
		{
				uint **occList;
				uint *lenList;
				uint maxPostValue;
				uint nlists ;
				
				//	//allocating memory.
				//	uint **occs;
				//	occs = (uint **) malloc (sizeof(uint *) * n);
				//	for (i=0;i<n;i++) occs[i]= (uint *) malloc (sizeof(uint) * wcsa->freqs[i]);

				parseAllPostingList(&nlists, &lenList, &occList, &maxPostValue, source,length);			
			
			 //checking:: decoding a list
			uint i, id ,len;
			uint *list;
			
			for (id=0;id < nlists;id++) {
				list = NULL;
				extractList_il (*ail, id, &list, &len);		
				for (i=0;i<len;i++){
					uint lval = list[i]+1;
					if (lval != occList[id][i]) {
						fprintf(stderr,"\n decoding of lists failed for id = %d: DIFFERENT!!: (%d,%d), [pos %d of lenlist[%u]=%u]",id, lval, occList[id][i], i, id,len);
						fflush(stderr);fflush(stdout);
						numberFails++; 
						break;
						//exit(0);
					}
				} 
				
			//	if (list){ free(list);}    //*** PIERDE MEMORIA AQUÍ, PERO ANTES "FALLABA EN FICH GRANDES" --> Debug !!!
			}	
			
			for (i=0;i<nlists;i++) free(occList[i]);
			free(occList);		
			free(lenList);
		
			fprintf(stderr,"\n \t %u lists were decoded ", id); 
			fflush(stderr);fflush(stdout);		  
		}
		
		if (!numberFails)
			fprintf(stderr,"\n ! Decoding the list of occurrences worked fine (the same as previous values) ");
		else 
			fprintf(stderr,"\n\n\n\n ! Decoding the list of occurrences failed for %u posting lists !!!!!!!\n\n\n\n", numberFails);
		
		fflush(stderr);fflush(stdout);	
    /******************* FARI 2013.05.20 *********************************************/
    
    

	return 0;
}
	


	/* returns a string with the parameters available for building the method. 
	   So that each index tells the user the available parameters.           */

char *showParameters_il(){
    	static char text[200];
	sprintf(text, "delta_sampling=%u", DELTA_SAMPLING);
	return text;
}


    /*  Saves index on disk by using single or multiple files, having 
      proper extensions. */

int save_il (void *ail, char *filebasename){
    char fn_base[2048];    
    char fn_start[2048];
    char fn_len[2048];
    char fn_chars[2048];
////    char fn_doclen[2048];
    char fn_encodedlen[2048];
//    snprintf(fn_base,2048,"cp tmp_filename_il %s",filebasename);
//    snprintf(fn_start,2048,"cp tmp_filename_il.start %s.start",filebasename);
//    snprintf(fn_len,2048,"cp tmp_filename_il.len %s.len",filebasename);
//    snprintf(fn_chars,2048,"cp tmp_filename_il.char %s.char",filebasename);
//    snprintf(fn_doclen,2048,"cp tmp_filename_il.doclen %s.doclen",filebasename);
//    snprintf(fn_encodedlen,2048,"cp tmp_filename_il.doclen %s.encodedlen",filebasename);


	if(0)  //failed fari-2018
	{
			snprintf(fn_base,2048,"cp %s %s",tmp_filename,filebasename);
			snprintf(fn_start,2048,"cp %s %s.start",tmp_filename_sources, filebasename);
			snprintf(fn_len,2048,"cp %s %s.len",tmp_filename_len, filebasename);
			snprintf(fn_chars,2048,"cp %s %s.char",tmp_filename_char,filebasename);
			////    snprintf(fn_doclen,2048,"cp %s %s.doclen",tmp_filename_doc,filebasename);
			snprintf(fn_encodedlen,2048,"cp %s %s.encodedlen",tmp_filename_encodedlen,filebasename);

			int ret;
			ret = system(fn_base);
			ret = system(fn_start);
			ret = system(fn_len);
			ret = system(fn_chars);
			////    ret = system(fn_doclen);
			ret = system(fn_encodedlen);
	}
    
	if(1){ //fari dec 2018

		snprintf(fn_base,2048,"%s",filebasename);
		snprintf(fn_start,2048,"%s.start", filebasename);
		snprintf(fn_len,2048,"%s.len", filebasename);
		snprintf(fn_chars,2048,"%s.char",filebasename);
		snprintf(fn_encodedlen,2048,"%s.encodedlen",filebasename);

			
		ssize_t totalbytes; char *dst,*src;
			
		src=tmp_filename;dst=fn_base; 
			totalbytes=cp_file(src,dst);
			if (-1==totalbytes) {printf("\n cp-file failed: %s to %s",src,dst);	exit(0);} 
			else {printf("\n copy succeedded (%zd bytes): %s to %s",totalbytes,src,dst);}  

		src=tmp_filename_sources;dst= fn_start;
			totalbytes=cp_file(src,dst);
			if (-1==totalbytes) {printf("\n cp-file failed: %s to %s",src,dst);	exit(0);}    
			else {printf("\n copy succeedded (%zd bytes): %s to %s",totalbytes,src,dst);} 
						
		src=tmp_filename_len;dst= fn_len;  
			totalbytes=cp_file(src,dst);
			if (-1==totalbytes) {printf("\n cp-file failed: %s to %s",src,dst);	exit(0);}    
			else {printf("\n copy succeedded (%zd bytes): %s to %s",totalbytes,src,dst);} 
								
		src=tmp_filename_char;dst=fn_chars;  
			totalbytes=cp_file(src,dst);
			if (-1==totalbytes) {printf("\n cp-file failed: %s to %s",src,dst);	exit(0);}    
			else {printf("\n copy succeedded (%zd bytes): %s to %s",totalbytes,src,dst);} 
			
		src=tmp_filename_encodedlen;dst=fn_encodedlen; 
			totalbytes=cp_file(src,dst);
			if (-1==totalbytes) {printf("\n cp-file failed: %s to %s",src,dst);	exit(0);}    
			else {printf("\n copy succeedded (%zd bytes): %s to %s",totalbytes,src,dst);} 	
			fflush(stdout);fflush(stderr);			
	}    
		

    
    //writes out "delta_sampling" parameter
    {
      lzi *il = (lzi *) ail;
      //il->delta_sampling=16;
      
      char fn_delta_sampling[2048];
      sprintf(fn_delta_sampling, "%s.delta_sampling",filebasename);
      FILE *f = fopen(fn_delta_sampling, "w");
      fprintf(f,"%u\n",il->delta_sampling);
      fclose(f);      
    }

    //writes out "bcs_encodedLen_n" and max_length value
    {
      lzi *il = (lzi *) ail;
      char fn_bcs_encodedLen_n[2048];
      sprintf(fn_bcs_encodedLen_n, "%s.idx.const",filebasename);
      FILE *f = fopen(fn_bcs_encodedLen_n, "w");
      fprintf(f,"%u\n",il->bcs_encodedLen_n);
      fprintf(f,"%u\n",il->maxLength);
      fclose(f);      
    }
    
    //writes out "idx.lenlist" array
    {
      lzi *il = (lzi *) ail;
      char fn_bcs_lenlist[2048];
      sprintf(fn_bcs_lenlist, "%s.idx.lenlist",filebasename);
      FILE *f = fopen(fn_bcs_lenlist, "w");
      fwrite(&il->lenlistbits , sizeof(unsigned int), 1, f);
      fwrite(&il->lenlistnints, sizeof(unsigned int), 1, f);
      fwrite(il->lenlist, sizeof(unsigned int), il->lenlistnints, f);            
      fclose(f);      
    }    
        

    return 0;
}
  
    /*  Loads index from one or more file(s) named filename, possibly 
      adding the proper extensions. */

int load_il (char *filebasename, void **ail){
////    char fn_doclen[2048];
    char fn_encodedlen[2048];
//    snprintf(fn_doclen,2048,"%s.doclen",filebasename);
    snprintf(fn_encodedlen,2048,"%s.encodedlen",filebasename);
    lzi* repr = (lzi*)malloc(sizeof(lzi));
////    repr->docLen = (unsigned int*)readArray(fn_doclen,(int*)(&(repr->docs)),sizeof(unsigned int));
    
    repr->encodedLen = (unsigned int*)readArray(fn_encodedlen,(int*)(&(repr->docs)),sizeof(unsigned int));

////    repr->maxLength = repr->docLen[0];
////    for(unsigned int i=1;i<repr->docs;i++){
////        if(repr->docLen[i]>repr->maxLength)repr->maxLength=repr->docLen[i];
////    }

    
    
    //loads "delta_sampling" parameter
    {
      lzi *il = repr;
      
      char fn_delta_sampling[2048];
      sprintf(fn_delta_sampling, "%s.delta_sampling",filebasename);
      FILE *f = fopen(fn_delta_sampling, "r");
      fscanf(f,"%u\n",&il->delta_sampling);
      fclose(f);    
      
     // cout << "\nDELTA SAMPLING PARAMETER VALUE is " << il->delta_sampling << "!!!!!!!!!!!!!!!!!!!!!!!!!";
    }

    //loads "bcs_encodedLen_n" and maxlength values
    {
      lzi *il = repr;
      char fn_bcs_encodedLen_n[2048];
      sprintf(fn_bcs_encodedLen_n, "%s.idx.const",filebasename);
      FILE *f = fopen(fn_bcs_encodedLen_n, "r");
      fscanf(f,"%u\n",&il->bcs_encodedLen_n);
      fscanf(f,"%u\n",&il->maxLength);
      //cout << "\n\n\nBCS_ENCODED_LEN VALUE is " << il->bcs_encodedLen_n << "and maxlength =" << il->maxLength << "!!!!!!!!!!!!!!!!!!!!!!!!!";
      fclose(f);      
    }
    
    
    //loads out "idx.lenlist" array
    {
      lzi *il =  repr;
      char fn_bcs_lenlist[2048];
      sprintf(fn_bcs_lenlist, "%s.idx.lenlist",filebasename);
      FILE *f = fopen(fn_bcs_lenlist, "r");
      fread(&il->lenlistbits , sizeof(unsigned int), 1, f);
      fread(&il->lenlistnints, sizeof(unsigned int), 1, f);
		il->lenlist = (uint *) malloc (sizeof(unsigned int) * repr->lenlistnints);
		il->lenlist[repr->lenlistnints-1]=0000;
            
      fread(il->lenlist, sizeof(unsigned int), il->lenlistnints, f);            
      fclose(f);      
    }    

    
    //repr->doc_array = new DeltaCodes(doc_len,repr->docs, DELTA_SAMPLING);
    //free(doc_len);
    
    //repr->extractor = new ExtractorEnd2(filebasename, DELTA_SAMPLING);
    repr->extractor = new ExtractorEnd2(filebasename, repr->delta_sampling); /*fari 2013, delta_sampling es build_parameter, default 16 */
    
    
    *ail = repr;
    return 0;
}


int show_statistics_usage (void *ail) {
  ulong nlists = (ulong) ((lzi*)ail)->docs;
  
  DeltaCodes* dc = ((ExtractorEnd2*)(((lzi*)ail)->extractor))->get_DC();
  uint sampling = ((lzi*)ail)->delta_sampling;
  


  
 /*tobits*/ ulong dc_sampled_ptrs_size   = 	(ulong) (sizeof(unsigned int) * (dc->get_sampled_pointer_nints())) ;
 /*tobits*/ //ulong dc_sampled_ptrs_size   = 	(ulong) (sizeof(unsigned int) * (dc->get_n()/sampling +1)) ;
/*tobits*/  ulong dc_sampled_values_size = 	(ulong) (sizeof(unsigned int) * (dc->get_n()/sampling +1)) ;
  ulong dc_delta_encoded_seq_bits   =  (ulong) (dc->get_total_size_bits());
  ulong dc_delta_encoded_seq_size   =  (ulong) (sizeof(unsigned int) * ((dc->get_total_size_bits()-1)/32 +2)) ;
  ulong dc_total =  dc_sampled_ptrs_size + dc_sampled_values_size + dc_delta_encoded_seq_size;
  
  ulong lzend_numphrases = (ulong) ( ((ExtractorEnd2*)(((lzi*)ail)->extractor))->get_len() );
  ulong lzend_extractor_structure = (ulong) ( sizeof(ExtractorEnd2)  ) ;
/*tobits*/  ulong lzend_phrases_sources_size = ((ExtractorEnd2*)(((lzi*)ail)->extractor))->get_sourcesbits_numbytes(); //lzend_numphrases * sizeof(uint);

  ulong lzend_phrases_chars_size   = lzend_numphrases * sizeof(char);
  ulong extractor_lzend_total = lzend_extractor_structure + lzend_phrases_sources_size +lzend_phrases_chars_size  + dc_total;
  ulong extractor_lzend_total_orig = (ulong) ( ((ExtractorEnd2*)(((lzi*)ail)->extractor))->size() );
  
  ulong ilist_offsets_size= nlists*sizeof(uint);
  ulong ilist_lens_size = ((lzi*)ail)->lenlistnints*sizeof(uint);
  ulong ilist_size = sizeof(lzi) + ilist_offsets_size + ilist_lens_size + extractor_lzend_total;
    printf("\n...................................................");
    printf("\n STATISTICS IN MEMORY USAGE for ilist-lzend........");
    printf("\n\t ilist structure = %lu bytes", (ulong)sizeof(lzi) );
    printf("\n\t ptrs to ilist_i ini offsets (encoded_len[]) = %lu bytes [nlists=%lu]", ilist_offsets_size, nlists );
    //printf("\n\t lens of ilist_i (doc_len[])  = %lu bytes [nlists=%lu]", ilist_lens_size, nlists );
    printf("\n\t @lens of ilist_i (doc_len[])  = %lu bytes [nlists=%lu][bits/val = %u]", ilist_lens_size, nlists, ((lzi*)ail)->lenlistbits );

    printf("\n\t extractor2lzend_size          = %lu bytes   (%lu frases)", extractor_lzend_total,lzend_numphrases);
    printf("\n\t \t Extractor structure        = %lu bytes", lzend_extractor_structure );
    printf("\n\t \t trailing chars[]           = %lu bytes", lzend_phrases_chars_size  );
    printf("\n\t \t lzend phrases in sources[] = %lu bytes", lzend_phrases_sources_size);
    printf("\n\t \t @lzend phrases in sources[] = %lu bytes  [***bits/val = %u***]", lzend_phrases_sources_size, ( ((ExtractorEnd2*)(((lzi*)ail)->extractor))->get_sourcesbitsperValue() ));
    
    
    printf("\n\t \t DeltaCodes = %lu bytes ", (ulong) (dc->size())  );
    printf("\n\t \t \t @sampled pointers  = %lu bytes [bits/val = %u]", dc_sampled_ptrs_size  ,   dc->get_sampled_pointer_bits() );
    printf("\n\t \t \t sampled values    = %lu bytes ", dc_sampled_values_size );
    printf("\n\t \t \t delta encoded seq = %lu bytes ", dc_delta_encoded_seq_size );
    printf("\n\t \t \t \t delta encoded seq len (in bits) = %lu  ", dc_delta_encoded_seq_bits  );
    
    uint total_il_size; size_il(ail, &total_il_size); 
    printf("\n Total bytes ilist = %u bytes",total_il_size );
    printf("\n Total bytes ilist = %lu bytes",ilist_size );
    
    printf("\n...................................................");
}

/** space if encoding sources[] with log_k bits, and if we encode
 *  DC->sampled pointers and DC->sampled_values with log_x and log_y bits respectively
 */

ulong show_statistics_usage_bits (void *ail) {
  ulong nlists = (ulong) ((lzi*)ail)->docs;
  
  DeltaCodes* dc = ((ExtractorEnd2*)(((lzi*)ail)->extractor))->get_DC();
  uint sampling = ((lzi*)ail)->delta_sampling;
  
  ulong dc_delta_encoded_seq_bits   =  (ulong) (dc->get_total_size_bits());
  ulong dc_delta_encoded_seq_size   =  (ulong) (sizeof(unsigned int) * ((dc->get_total_size_bits()-1)/32 +2)) ;

  /*tobits*/ //ulong dc_sampled_ptrs_size   = 	(ulong) (sizeof(unsigned int) * (dc->get_n()/sampling +2)) ;
  /*tobits*/ ulong dc_sampled_ptrs_size   = 	(ulong) ( bits(dc_delta_encoded_seq_bits+1) * (dc->get_n()/sampling +1)) /8;

  /*tobits*/  ulong dc_sampled_values_size = 	(ulong) (sizeof(unsigned int) * (dc->get_n()/sampling +2)) ;
  ulong dc_total =  dc_sampled_ptrs_size + dc_sampled_values_size + dc_delta_encoded_seq_size;
  
  ulong lzend_numphrases = (ulong) ( ((ExtractorEnd2*)(((lzi*)ail)->extractor))->get_len() );
  ulong lzend_extractor_structure = (ulong) ( sizeof(ExtractorEnd2)  ) ;

/*tobits*/  //ulong lzend_phrases_sources_size = lzend_numphrases * sizeof(uint);
/*tobits*/  ulong lzend_phrases_sources_size = lzend_numphrases * bits( lzend_numphrases+1 ) /8;

  ulong lzend_phrases_chars_size   = lzend_numphrases * sizeof(char);
  ulong extractor_lzend_total = lzend_extractor_structure + lzend_phrases_sources_size +lzend_phrases_chars_size  + dc_total;
  ulong extractor_lzend_total_orig = (ulong) ( ((ExtractorEnd2*)(((lzi*)ail)->extractor))->size() );
  

/*tobits*/   //ulong ilist_offsets_size= nlists*sizeof(uint);
/*tobits*/   ulong ilist_offsets_size= nlists *   bits(((lzi*)ail)->bcs_encodedLen_n +1)   /8;

	     
/*tobits*/   //ulong ilist_lens_size = nlists*sizeof(uint);
/*tobits*/   //ulong ilist_lens_size = nlists *   bits(((lzi*)ail)->maxLength +1)   /8;
			  ulong ilist_lens_size = ((lzi*)ail)->lenlistnints*sizeof(uint);

  ulong ilist_size = sizeof(lzi) + ilist_offsets_size + ilist_lens_size + extractor_lzend_total;
    printf("\n...................................................");
    printf("\n WRONG STATISTICS IN MEMORY USAGE for ilist-lzend (PRIOR ESTIMATION IS NOT OK!!)........");
    printf("\n\t ilist structure = %lu bytes", (ulong)sizeof(lzi) );
    
    printf("\n\t @ptrs to ilist_i ini offsets (encoded_len[]) = %lu bytes [nlists=%lu][bits/val = %u]", ilist_offsets_size, nlists ,bits(((lzi*)ail)->bcs_encodedLen_n +1));
    printf("\n\t @lens of ilist_i (doc_len[])  = %lu bytes [nlists=%lu][bits/val = %u]", ilist_lens_size, nlists, bits(((lzi*)ail)->maxLength +1) );

    printf("\n\t extractor2lzend_size          = %lu bytes   (%lu frases)", extractor_lzend_total,lzend_numphrases);
    printf("\n\t \t Extractor structure        = %lu bytes", lzend_extractor_structure );
    printf("\n\t \t trailing chars[]           = %lu bytes", lzend_phrases_chars_size  );
    printf("\n\t \t @lzend phrases in sources[] = %lu bytes  [***bits/val = %u*** wrong estimation**]", lzend_phrases_sources_size, bits(lzend_numphrases+1));
   // printf("\n\t \t @lzend phrases in sources[] = %lu bytes  [bits/val = %u]", lzend_phrases_sources_size, ( ((ExtractorEnd2*)(((lzi*)ail)->extractor))->get_sourcesbitsperValue() ));
    
        
    printf("\n\t \t DeltaCodes = %lu bytes (sumando = %lu)", (ulong) (dc->size()) , dc_total );
    printf("\n\t \t \t @sampled pointers  = %lu bytes [bits/val = %u]", dc_sampled_ptrs_size  , bits(dc_delta_encoded_seq_bits+1));
    printf("\n\t \t \t *sampled values    = %lu bytes ", dc_sampled_values_size );
    printf("\n\t \t \t delta encoded seq = %lu bytes ", dc_delta_encoded_seq_size );
    printf("\n\t \t \t \t delta encoded seq len (in bits) = %lu  ", dc_delta_encoded_seq_bits  );
    
    //uint total_il_size; size_il(ail, &total_il_size); 
    //printf("\n Total bytes ilist = %u bytes",total_il_size );
    printf("\n Total bytes ilist = %lu bytes",ilist_size );
    
    printf("\n...................................................");
    return ilist_size;
}

    /* Frees the memory occupied by index. */

int free_il (void *ail){
  
    show_statistics_usage(ail);
    show_statistics_usage_bits(ail);
  
    delete (ExtractorEnd2*)(((lzi*)ail)->extractor);

////    free((((lzi*)ail)->docLen));
    
    free((((lzi*)ail)->lenlist));
    
    free((((lzi*)ail)->encodedLen));
    
    //delete (DeltaCodes*)(((lzi*)ail)->doc_array);
    free(ail);

//	printf("\n tyring to unlink : %s",tmp_filename);
    unlink(tmp_filename);
//	printf("\n tyring to unlink : %s",tmp_filename_rev);
    unlink(tmp_filename_rev);
//	printf("\n tyring to unlink : %s",tmp_filename_sa);
    unlink(tmp_filename_sa);
//	printf("\n tyring to unlink : %s",tmp_filename_char);
    unlink(tmp_filename_char);
//	printf("\n tyring to unlink : %s",tmp_filename_len);	
    unlink(tmp_filename_len);
//	printf("\n tyring to unlink : %s",tmp_filename_sources);
    unlink(tmp_filename_sources);
//	printf("\n tyring to unlink : %s",tmp_filename_doc);	
    unlink(tmp_filename_doc);
//	printf("\n tyring to unlink : %s",tmp_filename_encodedlen);	
    unlink(tmp_filename_encodedlen);


    return 0;
}


    /* Gives the memory occupied by index in bytes. */

int size_il(void *ail, uint *size){
    //*size = sizeof(lzi) + ((ExtractorEnd2*)(((lzi*)ail)->extractor))->size() + ((DeltaCodes*)(((lzi*)ail)->doc_array))->size();
    //*size = sizeof(lzi) + ((ExtractorEnd2*)(((lzi*)ail)->extractor))->size() + ((lzi*)ail)->docs*2*sizeof(uint);
      *size = sizeof(lzi) 
	  + ((lzi*)ail)->docs*1*sizeof(uint) +  ((lzi*)ail)->lenlistnints*sizeof(uint)     //offset + lenlist_kbitwise 
	  + ((ExtractorEnd2*)(((lzi*)ail)->extractor))->size() ;  //extractor + DC + |char[]| + |sources[]|
    return 0;
}

    /* Gives the memory occupied by the index assuming an uncompressed
       representation (uint **)*/
int size_il_uncompressed(void *ail, uint *size){
    return 0;
}

    /* Gives the length of the longest posting-list*/
int maxSizePosting_il(void *ail, uint *size){
    lzi* repr = (lzi*)ail;
    return repr->maxLength;
}


	/* Generates estatistics about the len of the compressed posting lists */
int buildBell_il(void *ail, const char matlabfilename[]){
    return 0;
}


/********************************************************/
/***** Operations (searching) with the posting lists ****/
/********************************************************/


    /* Extracts the id-th list, having id \in [0..maxId-1] */

int extractList_il (void *ail, uint id, uint **list, uint *len){
    //DeltaCodes* doc_array = (DeltaCodes*)(((lzi*)ail)->doc_array);
    unsigned int start = id==0?0:(((lzi*)ail)->encodedLen)[id-1];
    unsigned int end = (((lzi*)ail)->encodedLen)[id]-1;
    uchar* tmp = (uchar*)((ExtractorEnd2*)(((lzi*)ail)->extractor))->extract(start,end);
    
   //// *len = (((lzi*)ail)->docLen)[id];
    lenlist(ail, id, len);
    
    *list = new uint[*len];
    uint pos=0;
    uint num = 0;
    BC_TO_INT(tmp, pos, num);
    //(*list)[0] = decode(num);
    (*list)[0] = num;

    for(uint i=1;i<*len;i++){
    	//if((*list)[i]<=(*list)[i-1])(*list)[i]=(*list)[i-1]+1;
        num=0;
        BC_TO_INT(tmp, pos, num);
	    //(*list)[i] = decode(num)+(*list)[i-1];
	    (*list)[i] = num+(*list)[i-1];
    }
    free(tmp);
    return 0;
}

    /* Extracts the id-th list, having id \in [0..maxId-1] 
       Assumes list has enough space previously allocated. */

int extractListNoMalloc_il (void *ail, uint id, uint *list, uint *len){
    uint* aux;
    extractList_il(ail, id, &aux,len);
    for(uint i=0;i<*len;i++){
        list[i] = aux[i];       // !!!!!!
    }
    free(aux);
    return 0;
}



/*fari 2013 */
    /* Extracts the id-th list, having id \in [0..maxId-1] 
     * it recovers the vbyte-d sequence of that lists
     * it returns in *len, the number of integers withing that list*/
int extractList_lzend_to_bytecode (void *ail, uint id, uchar **bcs, uint *len){
    //DeltaCodes* doc_array = (DeltaCodes*)(((lzi*)ail)->doc_array);
    unsigned int start = id==0?0:(((lzi*)ail)->encodedLen)[id-1];
    unsigned int end = (((lzi*)ail)->encodedLen)[id]-1;
    *bcs = (uchar*)((ExtractorEnd2*)(((lzi*)ail)->extractor))->extract(start,end);
    ////*len = (((lzi*)ail)->docLen)[id];
       lenlist(ail, id, len);
    return 0;
}



    /* Sets options for searches: such as parameters, funtions to use, etc 
       Permits to set different algorithms for intersect2, intersectN and 
       if needed "fsearch-operations needed by svs-type algorithms"     */

int setDefaultSearchOptions_il (void *ail, char *search_options){
    return 0;
}

	/* Intersects two lists given by id1 and id2  \in [0..maxId-1].
	   The defaultIntersect2 function must be initialized previously (svs, merge, ...)
	   Returns the resulting list of "intersecting positions"  */

uint* intersect_aux(uint* arr1, uint len1, uint* arr2, uint len2, uint* len){
    uint alen = len1<len2?len1:len2;
    uint* intersect = (uint*)malloc(sizeof(uint)*alen);
    uint a=0;
    uint b=0;
    uint count=0;
    while( (a<len1) &&(b<len2)){
        if(arr1[a]==arr2[b]){
            intersect[count] = arr1[a];
            count += 1;
            a++;b++;
        }else if(arr1[a]<arr2[b]){
            a++;
        }else{
            b++;
        }
    }
	if (!count) {free(intersect); intersect=NULL;}
    *len = count;
    return intersect;
}



/**sebas 2011**/
int intersect_2_il (void *ail, uint id1, uint id2,  uint *noccs, uint **occs){
    uint len1;
    uint* arr1;
    extractList_il(ail, id1, &arr1, &len1);
    uint len2;
    uint* arr2;
    extractList_il(ail, id2, &arr2, &len2);
    *occs = intersect_aux(arr1,len1,arr2,len2,noccs);
    free(arr1);
    free(arr2);
    return 0;
}



/**********************************************************************************/
/** should be faster, but it is not :(  *******************************************/
/** merging bcs instead of decoding bcs + merge of uints                         **/
/**fari 2013**/

int intersect_2_il_bcs (void *ail, uint id1, uint id2, uint *noccs, uint **occs){

	uint  n1, n2;
	uchar *src1, *src2;

	extractList_lzend_to_bytecode(ail,id1,&src1,&n1);
	extractList_lzend_to_bytecode(ail,id2,&src2,&n2);
	
	merge2uncBCS (id1,n1,src1,id2,n2,src2, noccs, occs);		
		
	return 0;	
}

/**fari 2013**/
/* intersects two streams of byte-codes belonging to term i1, and i2 
 * src1 and src2 point to those streams   
 */ 
int merge2uncBCS (uint id1, uint n1, uchar *src1, uint id2, uint n2, uchar* src2, uint *noccs, uint **occs){
	uint i1,i2;
	uint bcpos1,bcpos2, val1, val2, num;

	uint *list = (uint *) malloc ( n1 * sizeof(uint));
	uint matches=0;

	i1=1;i2=1;
	val1=val2=0;
	bcpos1=bcpos2=0;
	BC_TO_INT(src1, bcpos1, num);	
	val1 +=num;
	BC_TO_INT(src2, bcpos2, num);	
	val2 +=num;
	
	//fprintf(stderr,"\n call to merge2lzma, id1=%u, id2=%u, n1=%u, n2=%u ", id1,id2,n1,n2);
	
	//do {
	while ((i1 < n1) && (i2<n2)) {	
        /***/
		if (val1 == val2) {
			list[matches++] = val1;

			//next for id1
			{
				BC_TO_INT(src1, bcpos1, num);		
				val1 +=num;
				i1++;
			}
			
			//next for id2
			{	BC_TO_INT(src2, bcpos2, num);		
				val2 +=num;
				i2++;
			}	
		}

        /***/
		else if (val1 < val2) {
			//moves i1 
			while ((i1<n1) && (val1 < val2)) {
				BC_TO_INT(src1, bcpos1, num);		
				val1 +=num;
				i1++;
			}	
		}
        /***/
		else {//if (val1 > val2) {
			//moves i2 
			while ((i2<n2) && (val2 < val1)) {
				BC_TO_INT(src2, bcpos2, num);		
				val2 +=num;
				i2++;				
			}	
		}				
	}
	//while ((i1 < n1) && (i2<n2));

	//After merge-loop, but an occurrence could still occurr
	//if val1==val2 or 
	//if i_x==n_x, but i_y<n_y  and val_y < val_x ==> we should move on list_y while (i_y<n_y)&& (val_y<val_x)
	if (i1 < n1) { // posting1-not ended.
		while ((i1<n1) && (val1 < val2)) {
			BC_TO_INT(src1, bcpos1, num);
			val1 +=num;
			i1++;
		}			
	}
	else if (i2 < n2) { // posting1-not ended.
		while ((i2<n2) && (val2 < val1)) {
			BC_TO_INT(src2, bcpos2, num);		
			val2 +=num;
			i2++;				
		}				
	}

	//finally checking if the last 2-values are identical or not
	if (val1 == val2) {
		list[matches++] = val1 ;
	}	
		
	*occs = list;
	*noccs= matches;
	return 0;
}

/** END should be faster, but it is not :(  *******************************************/
/**********************************************************************************/














typedef struct {
    uint pos;
    uint len;
} lpair;

int lpair_compare(const void *a, const void *b) {
        return ( ((int)(((lpair*)a)->len)) - ((int)(((lpair*)b)->len)));
}
	/* Intersects $nids$ lists given by ids in ids[], having ids[i] \in [0..maxId-1].
	   Returns the resulting list of "intersecting positions"  */

int intersect_N_il (void *ail, uint *ids, uint nids, uint *noccs, uint **occs ){
    lpair* lengths = (lpair*)malloc(sizeof(lpair)*nids);
    for(unsigned int i=0;i<nids;i++){
        lengths[i].pos = ids[i];
        lenlist(ail, ids[i], &(lengths[i].len));
    }
    qsort(lengths, nids, sizeof(lpair), &lpair_compare);
        
    uint* auxArr;
    uint auxLen;
    intersect_2_il(ail, lengths[0].pos, lengths[1].pos, &auxLen, &auxArr);
    for(unsigned int i=2;(i<nids)&&(auxLen);i++){
		//if(!auxLen) {break;}
        uint len;
        uint* arr;
        extractList_il(ail, lengths[i].pos, &arr, &len);
        auxArr = intersect_aux(auxArr,auxLen,arr,len,&auxLen);
        free(arr);
    }
    free(lengths);
    *occs = auxArr;
    *noccs = auxLen;
    return 0;
}

int intersect_N_il_prev (void *ail, uint *ids, uint nids, uint *noccs, uint **occs ){

    lpair* lengths = (lpair*)malloc(sizeof(lpair)*nids);
    for(unsigned int i=1;i<nids;i++){
        lengths[i].pos = ids[i];
        lenlist(ail, ids[i], &(lengths[i].len));
    }
    qsort(lengths, nids, sizeof(uint), &lpair_compare);
    uint* auxArr;
    uint auxLen;
    intersect_2_il(ail, lengths[0].pos, lengths[1].pos, &auxLen, &auxArr);
    for(unsigned int i=2;i<nids;i++){
        uint len;
        uint* arr;
        extractList_il(ail, lengths[i].pos, &arr, &len);
        auxArr = intersect_aux(auxArr,auxLen,arr,len,&auxLen);
    }
    free(lengths);
    *occs = auxArr;
    *noccs = auxLen;
    return 0;
}


	/* Returns the len of the id-th posting list */
int lenlist(void *ail, uint id, uint *len){
	lzi *il = (lzi*)ail;
    ////*len = (((lzi*)ail)->docLen)[id];
    *len = get_field_LL(il->lenlist, il->lenlistbits, id);
    //uint lll = (((lzi*)ail)->docLen)[id];    
    //if (lll != (*len)) {printf("\nlenlist falla (id = %u) %u %u", id,*len, lll); exit(0);}
    
    return 0;
}

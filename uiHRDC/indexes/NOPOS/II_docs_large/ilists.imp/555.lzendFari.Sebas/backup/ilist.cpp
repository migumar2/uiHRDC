#include "ilist.h"
#include "LZend.nuevo.h"
#include "extractorend2.h"
#include <stdlib.h>
#include <cstring>
#include "utils.h"


#define DOCid_ADD (1)

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
	
	if (length !=sourcepos) return 31;
	
	*nlists =n;
	*lenList = lens;
	*occList = occs;	
	return 0;
}

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

static char tmp_filename[] = "tmp_filename_il";
static char tmp_filename_rev[] = "tmp_filename_il.rev";
static char tmp_filename_sa[] = "tmp_filename_il.rev.sa";
static char tmp_filename_char[] = "tmp_filename_il.char";
static char tmp_filename_len[] = "tmp_filename_il.len";
static char tmp_filename_sources[] = "tmp_filename_il.start";
static char tmp_filename_doc[] = "tmp_filename_il.doclen";

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

void sourceToText(uint* source, uint length, char* filename){
	printf("%d %d\n",length,source[0]);
    uint tlength=0;
    uint* text = (uint*) malloc(sizeof(uint)*(length-2-source[0]));
    uint pos = 2; 
    while(pos<length){
        uint len_il = source[pos];
//	text[tlength]=0;
//	tlength+=1;
        for(uint i=0;i<len_il;i++){
            text[tlength] = source[pos+i+1];
            tlength += 1;
        }
        /*for(uint i=1;i<len_il;i++){
            text[tlength] = source[pos+i+1]-source[pos+i];
            tlength += 1;
        }*/
        pos = len_il + pos + 1;
    }
	for(uint i=0;i<tlength;i++){
		text[i] = encode(text[i]);
	}
    saveText((uchar*)text, tlength*sizeof(uint), tmp_filename);
    free(text);
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
	if(len_il>*maxLength)*maxLength = len_il;
        pos = len_il + pos + 1;
    }
    *numlists = source[0];
    return text;
}
int build_il (uint *source, uint length, char *build_options, void **ail){
    unlink(tmp_filename);
    unlink(tmp_filename_rev);
    unlink(tmp_filename_sa);
    unlink(tmp_filename_char);
    unlink(tmp_filename_len);
    unlink(tmp_filename_sources);
    unlink(tmp_filename_doc);
    sourceToText(source, length, tmp_filename);
    //saveText(text, length, tmp_filename);
	printf("**********************");
    printf("\n: build_il: call to LZEND2->Parse()\n");
        
    LZparser* p;
    p = new LZEnd2(tmp_filename);
    p->parse();
    lzi* repr = (lzi*)malloc(sizeof(lzi));
    
	printf("**********************");
    printf("\n: build_il: call to LZEND2->ExtractorEnd2()");    
    repr->extractor = new ExtractorEnd2(tmp_filename, DELTA_SAMPLING);
    unsigned int numlists;
    unsigned int* doc_len = sourceToLen(source, length, &numlists, &(repr->maxLength));
    repr->docs = numlists;
	printf("**********************");
    printf("\n: build_il: call to LZEND2->deltaCodes()");    
    repr->doc_array = new DeltaCodes(doc_len,numlists, DELTA_SAMPLING);
    saveText((unsigned char*)doc_len, repr->docs*sizeof(unsigned int), tmp_filename_doc);
    delete[] doc_len;
    *ail = repr;


	return 0;
}
	


	/* returns a string with the parameters available for building the method. 
	   So that each index tells the user the available parameters.           */

char *showParameters_il(){
    return NULL;
}


    /*  Saves index on disk by using single or multiple files, having 
      proper extensions. */

int save_il (void *ail, char *filebasename){
    char fn_base[2048];    
    char fn_start[2048];
    char fn_len[2048];
    char fn_chars[2048];
    char fn_doclen[2048];
    snprintf(fn_base,2048,"cp tmp_filename_il %s",filebasename);
    snprintf(fn_start,2048,"cp tmp_filename_il.start %s.start",filebasename);
    snprintf(fn_len,2048,"cp tmp_filename_il.len %s.len",filebasename);
    snprintf(fn_chars,2048,"cp tmp_filename_il.char %s.char",filebasename);
    snprintf(fn_doclen,2048,"cp tmp_filename_il.doclen %s.doclen",filebasename);
    int ret;
    ret = system(fn_base);
    ret = system(fn_start);
    ret = system(fn_len);
    ret = system(fn_chars);
    ret = system(fn_doclen);
    return 0;
}


    /*  Loads index from one or more file(s) named filename, possibly 
      adding the proper extensions. */

int load_il (char *filebasename, void **ail){
    char fn_doclen[2048];
    snprintf(fn_doclen,2048,"%s.doclen",filebasename);
    lzi* repr = (lzi*)malloc(sizeof(lzi));
    unsigned int* doc_len = (unsigned int*)readArray(fn_doclen,(int*)(&(repr->docs)),sizeof(unsigned int));
    repr->maxLength = doc_len[0];
    for(unsigned int i=1;i<repr->docs;i++){
        if(doc_len[i]>repr->maxLength)repr->maxLength=doc_len[i];
    }
    repr->doc_array = new DeltaCodes(doc_len,repr->docs, DELTA_SAMPLING);
    free(doc_len);
    repr->extractor = new ExtractorEnd2(filebasename, DELTA_SAMPLING);
    *ail = repr;
    return 0;
}


    /* Frees the memory occupied by index. */

int free_il (void *ail){
    delete (ExtractorEnd2*)(((lzi*)ail)->extractor);
    delete (DeltaCodes*)(((lzi*)ail)->doc_array);
    free(ail);
    return 0;
}


    /* Gives the memory occupied by index in bytes. */

int size_il(void *ail, uint *size){
    *size = sizeof(lzi) + ((ExtractorEnd2*)(((lzi*)ail)->extractor))->size() + ((DeltaCodes*)(((lzi*)ail)->doc_array))->size();
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
    DeltaCodes* doc_array = (DeltaCodes*)(((lzi*)ail)->doc_array);
    unsigned int start = (doc_array->select(id)+1)*4;
    unsigned int end = (doc_array->select(id+1))*4+3;
    *list = (uint*)((ExtractorEnd2*)(((lzi*)ail)->extractor))->extract(start,end);
    *len = (end-start+1)/4;
    for(uint i=0;i<*len;i++){
	//if((*list)[i]<=(*list)[i-1])(*list)[i]=(*list)[i-1]+1;
	(*list)[i] = decode((*list)[i]);
    }
    return 0;
}

    /* Extracts the id-th list, having id \in [0..maxId-1] 
       Assumes list has enough space previously allocated. */

int extractListNoMalloc_il (void *ail, uint id, uint *list, uint *len){
    uint* aux;
    extractList_il(ail, id, &aux,len);
    for(uint i=0;i<*len;i++){
        list[i] = aux[i];
    }
    free(aux);
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
	//printf("\n len1= %u, len2=%u, alen is set to %u",len1,len2,alen);

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
	//if (!(*noccs)) {free(*occs); *occs=NULL;}
    return 0;
}

typedef struct {
    uint pos;
    uint len;
} lpair;

int lpair_compare(const void *a, const void *b) {
        return ((int)(((lpair*)a)->len) - (int)(((lpair*)b)->len));
}
	/* Intersects $nids$ lists given by ids in ids[], having ids[i] \in [0..maxId-1].
	   Returns the resulting list of "intersecting positions"  */

int intersect_N_il (void *ail, uint *ids, uint nids, uint *noccs, uint **occs ){
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
	//if (!auxLen) {free(auxArr); auxArr=NULL;}
    *occs = auxArr;
    *noccs = auxLen;
    return 0;
}


	/* Returns the len of the id-th posting list */
int lenlist(void *ail, uint id, uint *len){
    DeltaCodes* doc_array = (DeltaCodes*)(((lzi*)ail)->doc_array);
    unsigned int start = doc_array->select(id)+1;
    unsigned int end = doc_array->select(id+1);
    *len = (end-start+1);
    return 0;
}

#ifdef WITHMAIN

	int main(int argc, char** argv){
		lzi* ail;
		load_il(argv[1],(void**)&ail);
		int tlength = 92959588;
	fprintf(stderr,"Encode %d %d\n",encode(tlength),decode(encode(tlength)));
	fprintf(stderr,"Encode %d %d\n",encode(0),decode(encode(0)));
	//    uint* list = (uint*)((ExtractorEnd2*)(((lzi*)ail)->extractor))->extract(0,tlength-1);
	//	char ot[] = "encoded.txt";
	//    saveText((uchar*)list, tlength, ot);
	int length;
	uint* text =(uint*)readArray(argv[1],&length,sizeof(uint));
	uint* source =(uint*)readArray(argv[2],&length,sizeof(uint));
			uint i;
			uint **occList;
			uint *lenList;
			uint maxPostValue;
			uint nlists;
			parseAllPostingList(&nlists, &lenList, &occList, &maxPostValue, source,length);
	// checking that the lists are decoded successfully *************
	unsigned int numlists,a;
		unsigned int* doc_len = sourceToLen(source, length, &numlists, &(a));
	if(numlists!=nlists)fprintf(stderr,"%d %d\n",numlists,nlists);

	uint pos=0;
				uint id ,len;
	/*for(id=0;i<nlists;i++){
		for(i=0;i<doc_len[id];i++){
			if(text[pos]!=occList[id][i]-DOCid_ADD){
				fprintf(stderr,"(%x %x) ", text[pos], occList[id][i]-DOCid_ADD);

			}
		}
	}*/
			fprintf(stderr,"\n ! CHECKING that the decoded lists are identical to the original ones. ");
			{ //checking:: decoding a list
				uint *list;
				
				for (id=0;id < nlists;id++) {
					//if ( shouldUseBitmap(il,id) ) continue;
					extractList_il (ail, id, &list, &len);		
					//extractList (void *ail, uint id, uint **list, uint *len)
					printf("\n");
					for (i=0;i<len;i++){
						if (list[i] != (occList[id][i]-DOCid_ADD) ) {
							fprintf(stderr,"\n decoding of lists failed for %d id = %d: DIFFERENT!!: (%d,%d)\n",i,id, list[i],occList[id][i]-DOCid_ADD);
					//for(i=0;i<len;i++){
					//	printf("(%x %x) ", list[i], occList[id][i]-DOCid_ADD);
					//}
							exit(0);
						}
					} 
					
					free(list);				
				}			
			}
			fprintf(stderr,"\n ! Decoding the list of occurrences worked fine (the same as previous values) ");

			fprintf(stderr,"\n ! CHECKING that the decoded lists are identical to the original ones (extract_no_malloc). ");
			{ //checking:: decoding a list
				uint i, id ,len;
				uint *list = (uint *) malloc(sizeof(uint) * (maxPostValue+1));
				
				for (id=0;id < nlists;id++) {
					//if ( shouldUseBitmap(il,id) ) continue;
					//extractList_il (*ail, id, &list, &len);	
					extractListNoMalloc_il(ail,id,list,&len);	
					//extractList (void *ail, uint id, uint **list, uint *len)	
					for (i=0;i<len;i++){
						if (list[i] != (occList[id][i]-DOCid_ADD)) {
							//fprintf(stderr,"\n decoding of lists failed for id = %d: DIFFERENT!!",id);
							fprintf(stderr,"\n decoding of lists failed for id = %d: DIFFERENT!!: (%d,%d)",id, list[i],occList[id][i]-DOCid_ADD);
							exit(0);
						}
					} 
				}			
				free(list);				

			}
			fprintf(stderr,"\n ! Decoding the list of occurrences worked fine (the same as previous values) ");


			// ****************************************************** //



	}
#endif

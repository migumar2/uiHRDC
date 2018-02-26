#include "compressor.h"
#include "LZend.nuevo.h"
#include "extractorend2.h"
#include <stdlib.h>
#include <cstring>
       #include <sys/types.h>
       #include <unistd.h>

#include "utils.h"

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
static char tmp_filename[] = "tmp_filename";
static char tmp_filename_rev[] = "tmp_filename.rev";
static char tmp_filename_sa[] = "tmp_filename.rev.sa";
static char tmp_filename_char[] = "tmp_filename.char";
static char tmp_filename_len[] = "tmp_filename.len";
static char tmp_filename_sources[] = "tmp_filename.start";
static char tmp_filename_doc[] = "tmp_filename.doclen";
*/
static char tmp_filename[256];
static char tmp_filename_rev[256];
static char tmp_filename_sa[256];
static char tmp_filename_char[256];
static char tmp_filename_len[256];
static char tmp_filename_sources[256];
static char tmp_filename_doc[256];

int build_representation (unsigned char *text, unsigned int length,
	unsigned int *docBeginnings, unsigned int numdocs, 
	char *build_options, void **representation) {
    sprintf(tmp_filename,"%s.%u", "tmp_filename" ,getpid());
    sprintf(tmp_filename_rev,"%s.%u.%s", "tmp_filename" ,getpid(),"rev");
    sprintf(tmp_filename_sa,"%s.%u.%s", "tmp_filename" ,getpid(),"rev.sa");
    sprintf(tmp_filename_char,"%s.%u.%s", "tmp_filename" ,getpid(),"char");
    sprintf(tmp_filename_len,"%s.%u.%s", "tmp_filename" ,getpid(), "len");
    sprintf(tmp_filename_sources,"%s.%u.%s", "tmp_filename" ,getpid(), "start");
    sprintf(tmp_filename_doc,"%s.%u.%s", "tmp_filename" ,getpid(),"doclen");
    
    saveText(text, length, tmp_filename);
    LZparser* p;
    p = new LZEnd2(tmp_filename);
    p->parse();
    lzdoc* repr = (lzdoc*)malloc(sizeof(lzdoc));
    repr->extractor = new ExtractorEnd2(tmp_filename, DELTA_SAMPLING);
    repr->docs = numdocs;
    unsigned int* doc_len = new unsigned int[numdocs];
    for(unsigned int i=0;i<numdocs;i++){
        doc_len[i] = docBeginnings[i+1]-docBeginnings[i];    
    }
    repr->doc_array = new DeltaCodes(doc_len,numdocs, DELTA_SAMPLING);
    saveText((unsigned char*)doc_len, repr->docs*sizeof(unsigned int), tmp_filename_doc);
    delete[] doc_len;
    *representation = repr;
	return 0;
}

	/* Saves the [compressed] representation of the collection on disk by using 
	 * single or multiple files, having proper extensions. */

int save_representation (void *representation, char *basename) {
	char basenametext[2048]; sprintf(basenametext,"%s.%s",basename,"lztext");
    char fn_base[2048];    
    char fn_start[2048];
    char fn_len[2048];
    char fn_chars[2048];
    char fn_doclen[2048];
    snprintf(fn_base,2048,"cp %s %s",tmp_filename,basenametext);
    snprintf(fn_start,2048,"cp %s %s.start",tmp_filename_sources, basenametext);
    snprintf(fn_len,2048,"cp %s %s.len",tmp_filename_len, basenametext);
    snprintf(fn_chars,2048,"cp %s %s.char",tmp_filename_char,basenametext);
    snprintf(fn_doclen,2048,"cp %s %s.doclen",tmp_filename_doc,basenametext);
    int ret;
    ret = system(fn_base);
    ret = system(fn_start);
    ret = system(fn_len);
    ret = system(fn_chars);
    ret = system(fn_doclen);
    return 0;
}

	/*  Loads the [compressed] representation from one or more file(s) named 
	 * filename, possibly adding the proper extensions. */

int load_representation (void **representation, char *basename){
	char basenametext[2048]; sprintf(basenametext,"%s.%s",basename,"lztext");

    char fn_doclen[2048];
    snprintf(fn_doclen,2048,"%s.doclen",basenametext);
    lzdoc* repr = (lzdoc*)malloc(sizeof(lzdoc));
    unsigned int* doc_len = (unsigned int*)readArray(fn_doclen,(int*)(&(repr->docs)),sizeof(unsigned int));
    repr->doc_array = new DeltaCodes(doc_len,repr->docs, DELTA_SAMPLING);
    free(doc_len);
    repr->extractor = new ExtractorEnd2(basenametext, DELTA_SAMPLING);
    *representation = repr;
    return 0;
}

	/* Frees the memory occupied by the [compressed] representation. */

int free_representation (void *representation) {
    delete (ExtractorEnd2*)(((lzdoc*)representation)->extractor);
    delete (DeltaCodes*)(((lzdoc*)representation)->doc_array);
    free(representation);
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

    return 0;
}

	/* Gives the memory occupied by the [compressed] representation, in bytes */

int size_representation(void *representation, uint *size){
    /*printf("lzdoc: %d\n",sizeof(lzdoc));
    printf("lzend: %d\n",((ExtractorEnd2*)(((lzdoc*)representation)->extractor))->bits_size/8);
    printf("dcode: %d\n",((DeltaCodes*)(((lzdoc*)representation)->doc_array))->size());*/
    //*size = sizeof(lzdoc) + ((ExtractorEnd2*)(((lzdoc*)representation)->extractor))->bits_size/8 + ((DeltaCodes*)(((lzdoc*)representation)->doc_array))->size();
    *size = sizeof(lzdoc) + ((ExtractorEnd2*)(((lzdoc*)representation)->extractor))->size() + ((DeltaCodes*)(((lzdoc*)representation)->doc_array))->size();
    return 0;
}

	/* Recovers the text of i-th document of the collection of documents */

int extract_doc_representation (void *representation, uint i, unsigned char **doc, unsigned int *len) {
    DeltaCodes* doc_array = (DeltaCodes*)(((lzdoc*)representation)->doc_array);
    unsigned int start = doc_array->select(i)+1;
    unsigned int end = doc_array->select(i+1);
    *doc = ((ExtractorEnd2*)(((lzdoc*)representation)->extractor))->extract(start,end);
    *len = end-start+1;
    return 0;
}


	/* Recovers the snippet [from,to] in the collection of documents */

int extract_from_to_char (void *representation, uint from, uint to, unsigned char **snippet, unsigned int *slen) {
	printf("\n unimplemented extract_from_to_char() function **\n");fflush(stdout);fflush(stderr);
	*snippet = NULL;
	*slen =0;
	return 0;
}

/* Sebas, Miguel, esta funcion no tenéis que implementarla! */
/* La usé sólo para debugging */
void printTextInOffset(void *representation, uint offset, uint len) {
		
}

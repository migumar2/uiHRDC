#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
/*construction sa*/
/*#include "interface.h"*/
#include "divsufsort.h"
#include "lfs.h"

#include <sys/time.h>
#include <time.h>

struct timeval tv1,tv2;

void startTime(){
    gettimeofday(&tv1,NULL);
}
unsigned long endTime(){
    gettimeofday(&tv2,NULL);
    fprintf(stderr,"Total time: %.4f\n",((tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec))/1000000.0);
    return ((tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec));
}
unsigned long endTime2(){
    gettimeofday(&tv2,NULL);
    return ((tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec));
}
/*Returns the lenght of file filename*/
int lenText(char* filename){
    FILE* file = NULL;
    int len;
    file = fopen(filename,"r");
    /*Check for validity of the file.*/
    if(file == NULL){
        fprintf(stderr,"ERROR: Can't open file: %s\n",filename);
        return NULL;
    }
    /*Calculate the file length in bytes. This will be the length of the source string.*/
    fseek(file, 0, SEEK_END);
    len = (unsigned int)ftell(file);
    fclose(file);
    return len;
}
/* reads the content of file filename and returns it as an \0-ended array
 * it also set length to the length of the array
 * when the file is \0-ended the length is the length of the file
 * when the file is not \0-ended the length is the length of the file plus one
 */
unsigned char* readText(char* filename,unsigned int* length){
    unsigned char* str = NULL;    
    FILE* file = NULL;
    int read;
    int len;
    file = fopen(filename,"r");
    /*Check for validity of the file.*/
    if(file == NULL){
        fprintf(stderr,"ERROR: Can't open file: %s\n",filename);
        return NULL;
    }
    /*Calculate the file length in bytes. This will be the length of the source string.*/
    fseek(file, 0, SEEK_END);
    len = (unsigned int)ftell(file);
    fseek(file, 0, SEEK_SET);
    str = (unsigned char*)malloc((len+1)*sizeof(char));
    if(str == NULL){
        fprintf(stderr,"ERROR: Out of memory\n");
        return NULL;
    }
    read=  fread(str, sizeof(char), len, file);
    if(read<len){
        fprintf(stderr,"ERROR: Read less chars than size of file\n");
        return NULL;
    }
    if(str[len-1]!='\0'){
        //printf("last: %x",str[len-1]);
        str[len] = '\0';
        *length = len+1;
    }else{
        str[len-1] = '\0';
        *length = len;
    }
    fclose(file);
    return str;
}

/* save the text in file filename
 * returns 0 on success
 */
int saveText(unsigned char* text,int length,char* filename){
    FILE* fd;
    filename = basename(filename);
    fd = fopen(filename,"w");
    int written;
    if(fd!=NULL){
        written=fwrite(text,sizeof(unsigned char),length,fd);
        fclose(fd);
        if(written<length)return -1;
        return 0;
    }
    return 1;
}
/* computes the inverse permutation of SA
 * i.e inverseSA[SA[i]] = i
 */
unsigned int* invertSA(unsigned int* sa,int len){
    unsigned int* sai = NULL;
    int i;
    fprintf(stderr,"Trying to alloc %d bytes\n",len*sizeof(unsigned int));
    sai = (unsigned int*)malloc(len*sizeof(unsigned int));
    if(sai!=NULL){
        for(i=0;i<len;i++)
            sai[sa[i]]=i;
    }else{
        fprintf(stderr,"SAI is NULL\n");
    }
    return sai;
}

/* read the SA from file filename
 * sets salen to the length of the SA
 */
unsigned int* readArray(char* filename,int* alen,unsigned int size){
    unsigned int* array = NULL;
	int len=0;
	FILE* file = NULL;
	    file = fopen(filename,"r");
    /*Check for validity of the file.*/
    if(file == NULL){
        fprintf(stderr,"ERROR: can't open file %s.\n",filename);
        return NULL;
    }
    /*Calculate the file length in bytes. This will be the length of the source string.*/
    fseek(file, 0, SEEK_END);
    len = ftell(file);
    fseek(file, 0, SEEK_SET);
    array = (unsigned int*)malloc(len*sizeof(char));
    if(array == NULL){
        fprintf(stderr,"ERROR: Out of memory.\n");
        return NULL;
    }
    int ret = fread(array, sizeof(char), len, file);
    fclose(file);
    if(ret!=len){
        free(array);
        return NULL;
    }
    *alen=len/size;
    return array;
}

/* get the SA of text
 * if filename is given search if the SA is already computed
 */
unsigned int* getSA(unsigned char* text, int len, char* filename, int* salen){
    void* saindex;
    unsigned int* sa;
    char filename_temp[] = "stdin";
    char filename_sa[1024];
    if(filename==NULL)
        filename=filename_temp;
    filename =  basename(filename);
    snprintf(filename_sa,1024,"%s.sa",filename);
    if(access(filename_sa,R_OK)!=0 || strcmp(filename,"stdin")==0 || strcmp(filename,"stdin.rev")==0){
        sa = (unsigned int*)malloc(sizeof(unsigned int)*len);
        unsigned int err = divsufsort(text,(saidx_t*)sa,len);
        if(err){
            fprintf(stderr,"Error constructing the SA\n");
        }
        FILE* fp = fopen(filename_sa,"w");
        err = fwrite(sa, sizeof(unsigned int), len, fp);
        if(err!=len){
            fprintf(stderr,"Error writing the SA\n");
        }        
        fclose(fp);
	*salen = len;
        /*build_index(text,len-1,NULL,&saindex);
        save_index(saindex,filename);
        free_index(saindex);*/
    }
    sa = readArray(filename_sa,salen,sizeof(unsigned int));
    return sa;
}

/* reverse a '\0'-ended string leaving the '\0' at the end
 */
unsigned char* reverseText(unsigned char *text,int n){
    int i;
    unsigned char* rtext=(unsigned char*)malloc(n*sizeof(unsigned char));
    if(rtext!=NULL){
        for(i=0;i<n-1;i++)
            rtext[i]=text[n-2-i];
        rtext[n-1]='\0';
    }
    return rtext;
}

/* get array L from SA
 */
unsigned char* getL(unsigned int* sa,unsigned char* text,unsigned int len){
    unsigned int i;
    int t;   
    unsigned char* L=(unsigned char *)malloc(len*sizeof(unsigned char));
    if(L!=NULL){  
        for(i=0;i<len;i++){
            t=sa[i]-1;
            if(t==-1)t+=len;
            L[i]=text[t];
        }
    }
    return L;
}

/* get C array from SA
 */
unsigned int* getC(unsigned int* sa,unsigned char* text,unsigned int len){
    unsigned int i,j;   
    unsigned int* C=(unsigned int*)malloc(257*sizeof(unsigned int));
    if(C!=NULL){
        j=0;
        for(i=0;i<256;i++){
            for(;j<len && text[sa[j]]<i;j++);
            C[i]=j;
        }
        C[256]=len;
    }
    return C;
}

/* get the LCP array of text
 * if filename is given search if the SA is already computed
 */
/*unsigned int* getLCP(unsigned char* text, int len, char* filename, int q,int *lcplen){
    unsigned int* lcp;
    char filename_temp[] = "stdin";
    char filename_sa[1024];
    char filename_lcp[1024];
    char command[1024];
    int rc;
    if(q>=len)q=len/2;
    FILE* fp;
    if(filename==NULL)
        filename=filename_temp;
    filename = basename(filename);
    snprintf(filename_sa,1024,"%s.sa",filename);
    snprintf(filename_lcp,1024,"%s.lcp",filename);
    //filename_sa = basename(filename_sa);
    //filename_lcp = basename(filename_lcp);
    if(access(filename_lcp,R_OK)!=0  || strcmp(filename,"stdin")==0 || strcmp(filename,"stdin.rev")==0){
        snprintf(command,1024,"cp %s %s",filename_sa,filename_lcp);
        fprintf(stderr,"command %s\n",command);
        rc = system(command);
        if(rc==-1){
            fprintf(stderr,"ERROR: could not copy %s to %s\n",filename_sa,filename_lcp);
            return NULL;
        }
        fp = fopen(filename_lcp,"r+");
        if(fp == NULL){
            fprintf(stderr,"ERROR: could not open %s for reading/writing\n",filename_lcp);
            return NULL;
        }
        sec_splcp_initialize(text,fp,len-1,q);
        sec_splcp_construct();
        sec_splcp_free();
    }
    lcp = readArray(filename_lcp,lcplen,sizeof(unsigned int));
    return lcp;
}*/

/*returns a pointer to the basename of filename*/
/*char* basename(const char* filename){
    char* bn;
    bn = strrchr(filename, '/');
    if(bn == NULL)bn=filename;
    else bn = bn+1;
    return bn;
}

unsigned int* getCompressedArray(char* filename, int* alen, int bits){
    unsigned char* array = (unsigned char*)readArray(filename, alen, sizeof(unsigned char));
    unsigned int ctlen = uint_len(alen, bits);
    unsigned int* ctext = (unsigned int*)calloc(ctlen*sizeof(unsigned int));
    //for(unsigned int i=0; i<ctlen; i++)ctext[i]=0;
    for(unsigned int i=0; i<=alen; i++)
        set_field(ctext, bits, i, array[i]);
    free(array);
    return ctext;
}*/

/*unsigned int* compressArray(unsigned int* array, int len, int bits){
    unsigned int ctlen = uint_len(len, bits);
    unsigned int* ctext = (unsigned int*)calloc(ctlen*sizeof(unsigned int));
    //for(unsigned int i=0; i<ctlen; i++)ctext[i]=0;
    for(unsigned int i=0; i<=alen; i++)
        set_field(ctext, bits, i, array[i]);
    //free(array);
    return ctext;
}*/
/*unsigned int* getBitmap(char* filename, int* alen){
    unsigned int* array = readArray(filename, alen, sizeof(unsigned int));
    unsigned int size = alen%W==0?alen/W:alen/W+1;
    unsigned int* bitmap = (unsigned int*)calloc(size*sizeof(unsigned int));
    for(unsigned int i=0; i<=alen;i++){
        bitset(bitmap, array[i]);
    }
    free(array);
    return bitmap;
}*/
/*len is the size of sst_a and len-1 the size of prev_a*/
/*unsigned int getRangeArray(unsigned int* sst_a, unsigned int* prev_a, unsigned int len){
    unsigned int* range_a = (unsigned int*)malloc((len-1)*sizeof(unsigned int));
    for(unsigned int i=0;i<len-1;i++){
        for(unsigned int j=0;j<len;j++){
            if(sst_a[j]==prev_a[i]+1){
                range_a[i]=j;
                break;
            }
        }
    }
    return range_a;
}*/
/* given an array of integeres return a new array in wich ech value correspond to the rank of the value
 * used to compute the identifiers array from the positions array
 */
/*unsigned int getRankArray(unsigned int* array, unsigned int len){
    unsigned int* rank_a = (unsigned int*)malloc(len*sizeof(unsigned int));
    for(unsigned int i=0;i<len;i++){
        unsigned int lower=0;
        for(unsigned int j=0;j<len;j++){
            if(array[j]<array[i]){
                lower++;
            }
        }
        rank_a[i]=lower;
    }
    return rank_a;
}
*/

#include "utils.h"

/* construction sa libdivsufsort */
#include "divsufsort.h"
#include "basics.h"
//#include "lfs.h"

#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <algorithm>

#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>
#include <iostream>

namespace lz77index{

namespace utils{

struct timeval tv1,tv2;
double start_time;

double time(){
	double usertime, systime;
	struct rusage usage;
	getrusage (RUSAGE_SELF, &usage);
	usertime = (double) usage.ru_utime.tv_sec + (double) usage.ru_utime.tv_usec / 1000000.0;
	systime = (double) usage.ru_stime.tv_sec + (double) usage.ru_stime.tv_usec / 1000000.0;
	return (usertime + systime);
}

void startTime(){
    //gettimeofday(&tv1,NULL);
    start_time = time();
}

double endTimeV(){
    //gettimeofday(&tv2,NULL);
    //printf("Total time: %.4f\n",((tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec))/1000000.0);
    //return ((tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec));
    double t = time();
    printf("Total time: %.4f\n",t-start_time);
    return t-start_time;    
}

double endTime(){
    //gettimeofday(&tv2,NULL);
    //return ((tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec));
    double t = time();
    return t-start_time;
}

/* Returns the lenght of file filename
 * returns -1 on error
 */
int fileLength(char const* filename){
    FILE* file = NULL;
    int len;
    file = fopen(filename,"r");
    /*Check for validity of the file.*/
    if(file == NULL){
        fprintf(stderr,"ERROR: Can't open file: %s\n",filename);
        return -1;
    }
    /*Calculate the file length in bytes. This will be the length of the source string.*/
    fseek(file, 0, SEEK_END);
    len = (unsigned int)ftell(file);
    fclose(file);
    return len;
}
/* reads the content of file filename and stores it in an arrays of size length+1 */
unsigned char* readFile(char const* filename, unsigned int* length){
    unsigned char* str = NULL;    
    FILE* file = NULL;
    int read;
    int len;
    *length = 0; 
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
    //str = (unsigned char*)malloc((len+1)*sizeof(char));
    str = new unsigned char[len+1];
    if(str == NULL){
        fprintf(stderr,"ERROR: readFile: Out of memory when trying to alloc %d bytes\n",len+1);
        return NULL;
    }
    read = fread(str, sizeof(char), len, file);
    fclose(file);
    if(read<len){
        fprintf(stderr,"ERROR: readFile: Read less chars than size of file\n");
        return NULL;
    }
    *length = len;
    return str;
}

/* reads the content of file filename and returns it as an \0-ended array
 * it also set length to the length of the array
 * when the file is \0-ended the length is the length of the file
 * when the file is not \0-ended the length is the length of the file plus one
 */
unsigned char* readText(char const* filename, unsigned int* length){
    unsigned char* str = readFile(filename, length);
    if(str != NULL){
        if(str[*length-1]!='\0' && str[*length-1]!='\xA'){/*removes trailing \n*/
            str[*length] = '\0';
            *length = *length+1;
        }else{
            str[*length-1] = '\0';
            /**length = *length;*/
        }
    }
    return str;
}

/* save the text in file filename
 * returns 0 on success
 */
int saveArray(unsigned char const* array, int length, char const* filename){
    FILE* file;
    /*filename = basename(filename);*//*not sure about this*/
    file = fopen(filename,"w");
    int written;
    if(file != NULL){
        written = fwrite(array, sizeof(unsigned char), length, file);
        fclose(file);
        if(written < length)
            return -1;
        return 0;
    }
    return 1;
}

/* read an array from file filename
 * sets alen to the length of the array
 * size is the number of bytes of each element   
 */
unsigned char* readArray(char const* filename, unsigned int* length, unsigned int size){
    unsigned char* array = readFile(filename, length);
    if(array != NULL){
        if(*length % size != 0){
            fprintf(stderr,"ERROR: readArray: not so many elements in array\n");
        }
        *length = *length/size;
    }
    return array;
}

/* computes the inverse permutation of SA
 * i.e inverseSA[SA[i]] = i
 */
unsigned int* invertSA(unsigned int const* sa,int len){
    unsigned int* sai = NULL;
    int i;
    sai = (unsigned int*)malloc(len*sizeof(unsigned int));
    if(sai!=NULL){
        for(i=0;i<len;i++)
            sai[sa[i]]=i;
    }else{
        fprintf(stderr,"ERROR: invertSA: no enough space when trying to alloc %d bytes\n",len*sizeof(unsigned int));
    }
    return sai;
}

/* get the SA of text
 * if filename is given search if the SA is already computed
 */
unsigned int* getSA(unsigned char const* text, int length, const char* filename){
    char filename_temp[] = "stdin";
    char filename_sa[1024];
    unsigned int* sa;
    if(filename == NULL)
        filename = filename_temp;
    /*filename = basename(filename);*/ /*not sure about this*/
    snprintf(filename_sa,1024,"%s.sa",filename);
    /* checks if we already computed the SA*/
    if(access(filename_sa, R_OK)!=0 || strcmp(filename,"stdin") == 0 || strcmp(filename, "stdin.rev") == 0){
        sa = new unsigned int[length];
        unsigned int err = divsufsort(text, (saidx_t*)sa, (saidx_t)length);
        if(err){
            fprintf(stderr, "ERROR: SA: constructing the SA\n");
        }
        err = saveArray((unsigned char*)sa, length * sizeof(unsigned int), filename_sa);
        if(err != 0){
            fprintf(stderr,"ERROR: SA: saving the SA\n");
        }        
    }else{
        unsigned int salen;
        sa = (unsigned int*)readArray(filename_sa, &salen, sizeof(unsigned int));
    }
    return sa;
}

/* reverse a '\0'-ended string leaving the '\0' at the end
 */
unsigned char* reverseText(unsigned char const* text,int n){
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
unsigned char* getL(unsigned int const* sa,unsigned char const* text,unsigned int len){
    int t;   
    unsigned char* L=(unsigned char *)malloc(len*sizeof(unsigned char));
    if(L!=NULL){  
        for(unsigned int i=0;i<len;i++){
            t=sa[i]-1;
            if(t==-1)t+=len;
            L[i]=text[t];
        }
    }
    return L;
}

/* get C array from SA
 */
unsigned int* getC(unsigned int const* sa,unsigned char const* text,unsigned int len){
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

unsigned int* compressArray(unsigned int* array, unsigned int len, unsigned char* bits){
    unsigned int max_val = *std::max_element(array,array+len);
    std::cout<<"Max: "<<max_val<<std::endl;
    *bits = basics::bits(max_val);
    unsigned int ctlen = basics::uint_len(len, *bits);
    unsigned int* ctext = new unsigned int[ctlen];
    int s=0;
    for(unsigned int i=0; i<ctlen; i++)
        ctext[i]=0;
    for(unsigned int i=0; i<len; i++){
        basics::set_field(ctext, *bits, i, array[i]);
        s+=array[i];
    }
    std::cout<<"Mean: "<<s*1.0/len<<std::endl;
    return ctext;
}

/*returns a pointer to the basename of filename*/
/*char* basename(char const* filename){
    char* bn;
    bn = strrchr(filename, '/');
    if(bn == NULL)bn=filename;
    else bn = bn+1;
    return bn;
}*/

}//namespace utils
}//namespace lz77index


#include "LZend.h"
#include <stdlib.h>
#include <iostream>

#include "utils.h"
#include "treelist.h"
#include "basics.h"
#include "static_bitsequence_builder.h"
//#include <unistd.h>

LZend::LZend(char* filename):LZparser(filename){
    int temp;
    char filename_rev[2048];
    //std::cout<<"start"<<std::endl; 
    //sleep(10);
    unsigned char* rtext = reverseText(this->text,this->text_len);
    //std::cout<<"rev"<<std::endl;
    //sleep(10);
    snprintf(filename_rev,2048,"%s.rev",filename);
    saveText(rtext,this->text_len,filename_rev);
    this->sa = (int*)getSA(rtext, this->text_len, filename_rev, &temp);
    unsigned int a = this->sa[0]; 
    std::cout<<a<<std::endl;
    //std::cout<<"sa"<<std::endl;
    //sleep(10);
    this->C =  getC((unsigned int*)this->sa,rtext,this->text_len);
    this->La = getL((unsigned int*)this->sa,rtext,this->text_len);
    free(rtext);
    //std::cout<<"free"<<std::endl;
    //sleep(10);
    //this->sai = invertSA((unsigned int*)this->sa,this->text_len);
    
    /*computing permutation*/
    //uint b = bits(this->text_len-1);
    //uint s = uint_len(this->text_len,b);
    //unsigned int* array = new unsigned int[s];
    //for(int i=0;i<this->text_len;i++)set_field(array,b,i,this->text_len - 1 - this->sa[i]);
    for(unsigned int i=0;i<this->text_len;i++)this->sa[i]=this->text_len - 1 - this->sa[i];    
    int t = 32;
    static_bitsequence_builder* bsb = new static_bitsequence_builder_brw32(20);
    this->pi = new static_permutation_mrrr((unsigned int*)this->sa,this->text_len,t,bsb);
    delete bsb;
    //std::cout<<"perm"<<std::endl;
    //sleep(10);
    /*end permutation*/    
    //this->bucket_size=(this->text_len)/(64*256)+1;
    this->bucket_size = 256 * 3;//256 * 4 higher means less memory
    //this->bucket_size=5;   
    this->precomputed = precomputeOcc(this->bucket_size,this->text_len);
    //std::cout<<"precom"<<std::endl;
    //sleep(10);
    this->rmq = new RMQ(this->sa,this->text_len);
    //std::cout<<"Listo"<<std::endl;
    //sleep(10);
    //check();
}
LZend::~LZend(){
    /*if(this->sai!=NULL)
        free(this->sai);*/
    if(this->sa!=NULL)
        free(this->sa);
    if(this->C!=NULL)
        free(this->C);
    if(this->La!=NULL)
        free(this->La);
    if(this->precomputed!=NULL)
        free(this->precomputed);
    if(this->rmq!=NULL)
        delete this->rmq;
    if(this->pi!=NULL)
        delete this->pi;
}

/* generates the LZ parsing of filename and returns the number of phrases
 * creates 3 different files (arrays):
 *    filename.start (4*len bytes)   
 *    filename.len   (4*len bytes)
 *    filename.char  (1*len bytes) */
unsigned int LZend::parse(){
    unsigned int total=0;
    int c=0;
    unsigned int i,j;
    unsigned int factorpos;
    unsigned int factorlen;
    unsigned int maxpos;
    int nmin,nmax;
    int first,last;
    int a;
    TreeList* factors;
    factors = newTreeList();
    factors->val=this->SAI(this->text_len - 2 - 0);
    this->report(0,1,this->text[0],0);
    for(i=1;i<this->text_len;i++){
        //printf("%d %d\n",i,count);
        factorpos=0;
        factorlen=0;
        first=-1;last=-1;
        for(j=1;i+j<this->text_len;j++){
            /*printf("j:%d\n",j);*/
            total++;
            this->get_rows(&nmin,&nmax,first,last,this->text[i+j-1],C);
            //for(l=nmin-1;l<=nmax-1;l++){
            //   printf("%d ",sa[l]);
            //}printf("\n");
            //printf("Max: %d,j: %d, i:%d all:%d\n",sa[getMax(sa,nmin-1,nmax-1)],j,i,(len - 2 - sa[getMax(sa,nmin-1,nmax-1)]-(j-1)));
            //maxpos = getMax(nmin-1,nmax-1);
            maxpos = this->rmq->minpos(nmin-1,nmax-1);
            if(this->text_len - 2 - (this->SA(maxpos))-(j-1)>=i )break;
            a=nextValue(factors,nmin-1);
            //b=prevValue(factors,nmax-1);
            //printf("i:%d j:%d->%d %d  %d %d    %d\n",i,j,a,b,nmin,nmax,sai[len-2-(i+j-1)]);
            //if(a<=b && a!=-1){
            if(a!=~0 && a<=nmax-1){
                //factorpos=len-2-sa[a];
                factorlen=j;
                //factorpos=this->text_len-2-factorlen-this->sa[a];
                factorpos=this->text_len-1-factorlen-(this->SA(a));
            }
            first=nmin;
            last=nmax;
        }
        c+=j-factorlen;
        if(this->text_len>=2+i+factorlen){/*the conditions must be in this way because len is unsigned int*/
            addValue(factors,this->SAI(this->text_len-2-i-factorlen));
        }      
        this->report(factorpos,factorlen+1,this->text[i+factorlen],i);
        i+=factorlen;
    }
    freeTreeList(factors);
    this->close();
    std::cout<<"Total work done: "<<total<<std::endl;
    return this->factors;
}

unsigned int LZend::SAI(unsigned int pos){
    return this->pi->rev_pi(this->text_len- 1 - pos);
    //return this->sai[pos];
}
unsigned int LZend::SA(unsigned int pos){
    return this->text_len - 1 - this->sa[pos];
    //return this->text_len - 1 - this->pi->pi(pos);
    //return (unsigned int)((-1)*this->sa[pos]);
}

/*precomputed[i*c+c]=Occ(c,L[0,(i)*bucket_size-1])*/
unsigned int* LZend::precomputeOcc(int bucket_size,unsigned int len){
   unsigned int i,j;
   unsigned int occ_temp[256];
   unsigned int* precomputed=(unsigned int*)malloc(((len/bucket_size)+1)*256*sizeof(unsigned int));
   //printf("precomputed:%p %d\n",(void*)precomputed,((len/bucket_size)+1)*256*sizeof(unsigned int));
   for(j=0;j<256;j++){
      precomputed[j]=0;
      occ_temp[j]=0;
   }
   for(i=1;i<(len/bucket_size)+1;i++){
      for(j=bucket_size*(i-1);j<bucket_size*i && j<len;j++){
         occ_temp[this->La[j]]++;//this->L(j)
      }
      for(j=0;j<256;j++){
         precomputed[i*256+j]=occ_temp[j];
         /*if(occ_temp[j]!=0)printf("%c:%d ",j,occ_temp[j]);
         printf("\n");*/
      }
   }
   /*for(i=0;i<(len/bucket_size)+1;i++){
      printf("precomputed[a,%d]=%d\n",i,precomputed[i*256+'a']);
   }*/
   return precomputed;
}

unsigned int LZend::Occ(unsigned char c,int pos){
   int i;
   int bucket_pos;
   unsigned int occ;
   if(pos==0)return 0;
   pos--;
   bucket_pos=pos/this->bucket_size;
   occ=precomputed[bucket_pos*256+c];
   for(i=(bucket_pos)*bucket_size;i<=pos;i++)
      if(this->La[i]==c)occ++;//this->L(i)
   return occ;
}
void LZend::get_rows(int* nmin,int* nmax,int first,int last,unsigned char c,unsigned int* C){
   if(last==first && last==-1){
      first=this->C[c]+1;
      last=this->C[c+1];
   }else{
      /*printf("**> %d %d %d %d %d %c\n",C[c],Occ(c,first-1),Occ(c,last),first,last,c);*/
      first=this->C[c]+this->Occ(c,first-1)+1;
      last=this->C[c]+this->Occ(c,last);
   }
   if(first>last){
      *nmin=-1;
      *nmax=-1;
   }else{
      *nmin=first;
      *nmax=last;
   }
}

unsigned char LZend::L(int pos){
    return this->La[pos];
    /*int t = this->SA(pos);
    if(t==0)return '\0';
    return this->text[this->text_len - 2 - (t-1)];*/
}
/*void LZend::check(){
    for(int i=0;i<this->text_len;i++){
        if(this->L[i]!=this->LL(i))std::cout<<L[i]<<" "<<LL(i)<<" "<<i<<std::endl;
    }  
}*/
int getMax(unsigned int* sa, int start, int end){
    int maxpos=start;
    int i;
    if(start>end)return -1;
    for(i=start+1;i<=end;i++){
        if(sa[i]>sa[maxpos])maxpos=i;
    }
    return maxpos;
}

unsigned int LZend::add_bits(unsigned int pos, unsigned int len){
    unsigned int b=0;
    b += this->log_sigma;//trailing character
    b += this->log(this->factors);//start of source
    b += 1 + 2*(this->log(this->log(len))-1) + (this->log(len)-1);//len of source 
    return b;
}


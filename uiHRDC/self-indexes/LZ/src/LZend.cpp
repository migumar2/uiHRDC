#include "LZend.h"
#include "utils.h"
#include "treelist.h"
#include "basics.h"
#include "static_bitsequence_builder.h"

#include <cstdlib>
#include <iostream>

namespace lz77index{
namespace parsing{

LZEnd::LZEnd(const char* filename):LZparser(filename){
    //int temp;
    char filename_rev[2048];
    //std::cout<<"start"<<std::endl; 
    //sleep(10);
    unsigned char* rtext = utils::reverseText(this->text,this->text_len);
    //std::cout<<"rev"<<std::endl;
    //sleep(10);
    snprintf(filename_rev,2048,"%s.rev",filename);
    utils::saveArray(rtext,this->text_len,filename_rev);
    std::cout<<"SA";
    utils::startTime();
    this->sa = (int*)utils::getSA(rtext, this->text_len, filename_rev);//, &temp);
    utils::endTimeV();
    //std::cout<<"sa\n**********************"<<std::endl;
    //sleep(10);
    this->C =  utils::getC((unsigned int*)this->sa,rtext,this->text_len);
    this->La = utils::getL((unsigned int*)this->sa,rtext,this->text_len);
    free(rtext);
    //std::cout<<"free"<<std::endl;
    //sleep(10);
    //this->sai = invertSA((unsigned int*)this->sa,this->text_len);
    
    /*computing permutation*/
    //unsigned int b = bits(this->text_len-1);
    //unsigned int s = uint_len(this->text_len,b);
    //unsigned int* array = new unsigned int[s];
    //for(int i=0;i<this->text_len;i++)set_field(array,b,i,this->text_len - 1 - this->sa[i]);
    for(unsigned int i=0;i<this->text_len;i++)this->sa[i]=this->text_len - 1 - this->sa[i];    
    int t = 32;
    basics::static_bitsequence_builder* bsb = new basics::static_bitsequence_builder_brw32(20);
    this->pi = new basics::static_permutation_mrrr2((unsigned int*)this->sa,this->text_len,t,bsb);
    delete bsb;
    //std::cout<<"perm"<<std::endl;
    //sleep(10);
    /*end permutation*/    
    //this->bucket_size=(this->text_len)/(64*256)+1;
    this->bucket_size = 256 * 3;//256 * 4 higher means less memory
    //this->bucket_size=5;   
    //this->precomputed = precomputeOcc2(this->bucket_size,this->text_len);
    precomputeOcc(&(this->precomputed1),&(this->precomputed2),this->text_len);
    //std::cout<<"precom"<<std::endl;
    //sleep(10);
    this->rmq = new RMQ(this->sa,this->text_len);
    //std::cout<<"Listo"<<std::endl;
    //sleep(10);
    //check();
}
LZEnd::~LZEnd(){
    /*if(this->sai!=NULL)
        free(this->sai);*/
    if(this->sa!=NULL)
        delete [] this->sa;
    if(this->C!=NULL)
        free(this->C);
    if(this->La!=NULL)
        free(this->La);
    //if(this->precomputed!=NULL)
    //    free(this->precomputed);
    if(this->precomputed1!=NULL)
        free(this->precomputed1);
    if(this->precomputed2!=NULL)
        free(this->precomputed2);
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
unsigned int LZEnd::parse(){
    //long t_bws=0;
    //long t_tree=0;
    //long t_pi=0;
    //long t_rmq = 0;
    unsigned int total=0;
    int c=0;
    unsigned int i,j;//,l;
    unsigned int factorpos;
    unsigned int factorlen;
    unsigned int maxpos;
    int nmin,nmax;
    int first,last;
    int a;//,b;
    TreeList* factors;
    //startTime();
    factors = newTreeList();
    factors->val=this->SAI(this->text_len - 2 - 0);
    //t_tree += endTime2();
    this->report(0,1,this->text[0],0);
    for(i=1;i<this->text_len;i++){
        //printf("%d %d\n",i,count);
        factorpos=0;
        factorlen=0;
        first=-1;last=-1;
        for(j=1;i+j<this->text_len;j++){
            /*printf("j:%d\n",j);*/
            total++;
            //startTime();
            this->get_rows(&nmin,&nmax,first,last,this->text[i+j-1],C);
            //t_bws += endTime2();
            //for(l=nmin-1;l<=nmax-1;l++){
            //   printf("%d ",sa[l]);
            //}printf("\n");
            //printf("Max: %d,j: %d, i:%d all:%d\n",sa[getMax(sa,nmin-1,nmax-1)],j,i,(len - 2 - sa[getMax(sa,nmin-1,nmax-1)]-(j-1)));
            //maxpos = getMax(nmin-1,nmax-1);
            //startTime();
            maxpos = this->rmq->minpos(nmin-1,nmax-1);
            //t_rmq += endTime2();
            //if(maxpos>=this->text_len)std::cout<<"error computing the maxpos "<<maxpos<<" "<<this->text_len<<" "<<nmin-1<<" "<<nmax-1<<std::endl;
            //if(this->text_len - 2 - (this->SA(maxpos))-(j-1)>=i )break;
            if( this->sa[maxpos]- j >=i)break;
            //startTime();
            a=nextValue(factors,nmin-1);
            //t_tree += endTime2();
            //b=prevValue(factors,nmax-1);
            //printf("i:%d j:%d->%d %d  %d %d    %d\n",i,j,a,b,nmin,nmax,sai[len-2-(i+j-1)]);
            //if(a<=b && a!=-1){
            if(a!=~0 && a<=nmax-1){
                //factorpos=len-2-sa[a];
                factorlen=j;
                //factorpos=this->text_len-2-factorlen-this->sa[a];
                //factorpos=this->text_len-1-factorlen-(this->SA(a));
                factorpos = this->sa[a]-factorlen;
            }
            first=nmin;
            last=nmax;
        }
        c+=j-factorlen;
        if(this->text_len>=2+i+factorlen){/*the conditions must be in this way because len is unsigned int*/
            //startTime();
            unsigned int rpos = this->pi->rev_pi(1 + factorlen + i);//this->SAI(this->text_len-2-i-factorlen)
            //t_pi += endTime2();
            //startTime();
            addValue(factors,rpos);
            //t_tree += endTime2();
        }      
        this->report(factorpos,factorlen+1,this->text[i+factorlen],i);
        i+=factorlen;
    }
    freeTreeList(factors);
    this->close();
    //std::cout<<"RMQ : "<<t_rmq/1000000.0<<std::endl;
    //std::cout<<"Tree: "<<t_tree/1000000.0<<std::endl;
    //std::cout<<"BWS : "<<t_bws/1000000.0<<std::endl;
    //std::cout<<"Pi  : "<<t_pi/1000000.0<<std::endl;
    //std:cout<<"Total work done: "<<total<<std::endl;
    return this->factors;
}

unsigned int LZEnd::SAI(unsigned int pos){
    return this->pi->rev_pi(this->text_len- 1 - pos);
    //return this->sai[pos];
}
unsigned int LZEnd::SA(unsigned int pos){
    return this->text_len - 1 - this->sa[pos];
    //return this->text_len - 1 - this->pi->pi(pos);
    //return (unsigned int)((-1)*this->sa[pos]);
}

/*precomputed[i*c+c]=Occ(c,L[0,(i)*bucket_size-1])*/
void LZEnd::precomputeOcc(unsigned int** pc1, unsigned short int** pc2,unsigned int len){
   int b_size = 512;
   unsigned int i,j;
   unsigned int occ_temp[256];
   unsigned int* precomputed1 = (unsigned int*)calloc(((len/65536)+1)*256,sizeof(unsigned int));
   unsigned short int* precomputed2 = (unsigned short int*)calloc(((len/b_size)+1)*256,sizeof(unsigned short int));
   //printf("precomputed:%p %d\n",(void*)precomputed,((len/bucket_size)+1)*256*sizeof(unsigned int));
   for(j=0;j<256;j++){
      precomputed1[j]=0;
      precomputed2[j]=0;
      occ_temp[j]=0;
   }
   for(i=1;i<(len/b_size)+1;i++){
      for(j=b_size*(i-1);j<b_size*i && j<len;j++){
         occ_temp[this->La[j]]++;//this->L(j)
      }
      if(i%128==0){
          for(j=0;j<256;j++){
             precomputed1[(i<<1)+j] = occ_temp[j];
          }
      }
      for(j=0;j<256;j++){
         //std::cout<<"Index:"<<i<<" "<<((i<<8)+j)<<" "<<((len/bucket_size)+1)*256<<std::endl;
         precomputed2[(i<<8)+j] = occ_temp[j] - precomputed1[((i>>7)<<8)+j];
         //if(i%128==0 && precomputed2[(i<<8)+j]!=0)std::cerr<<"Should be 0"<<std::endl;
         /*if(occ_temp[j]!=0)printf("%c:%d ",j,occ_temp[j]);
         printf("\n");*/
      }
   }
   /*for(i=0;i<(len/bucket_size)+1;i++){
      printf("precomputed[a,%d]=%d\n",i,precomputed[i*256+'a']);
   }*/
   *pc1 = precomputed1;
   *pc2 = precomputed2;
    //check
    /*for(unsigned int i=0;i<len/65536;i++){
        for(unsigned j=0;j<256;j++){
            if(precomputed1[i*256+j]!=Occ2((unsigned char)j,(int)i*65536))
                std::cerr<<"Precomputed1: error "<<i<<" "<<j<<"-"<<precomputed1[i*256+j]<<" "<<Occ2((unsigned char)j,(int)i*65536)<<std::endl;
        }
    }
    for(unsigned int i=0;i<len/512;i++){
        for(unsigned j=0;j<256;j++){
            if(precomputed2[i*256+j]!=Occ2((unsigned char)j,(int)i*512))
                std::cerr<<"Precomputed2: error "<<i<<" "<<j<<"-"<<precomputed2[i*256+j]<<" "<<Occ2((unsigned char)j,(int)i*512)<<std::endl;
        }
    }*/
}
unsigned int* LZEnd::precomputeOcc2(int bucket_size,unsigned int len){
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
unsigned int LZEnd::Occ2(unsigned char c,int pos){
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
unsigned int LZEnd::Occ(unsigned char c,int pos){
   int i;
   int bucket_pos1,bucket_pos2;
   unsigned int occ;
   if(pos==0)return 0;
   pos--;
   bucket_pos1 = pos>>16;
   bucket_pos2 = pos>>9;
   occ = precomputed1[(bucket_pos1<<8)+c];
   occ += precomputed2[(bucket_pos2<<8)+c];
   for(i=(bucket_pos2)<<9;i<=pos;i++)
      if(this->La[i]==c)occ++;//this->L(i)
   /*if(occ!=Occ2(c,pos+1)){
        unsigned int o=0;
        for(unsigned int i=0;i<=pos+1;i++)
            if(this->La[i]==c)o++;//this->L(i)
        std::cerr<<"Error Occ:"<<occ<<" "<<Occ2(c,pos+1)<<" "<<o<<std::endl;
        std::cerr<<pos<<" "<<bucket_pos1<<" "<<bucket_pos2<<std::endl;
        return Occ2(c,pos+1);
    }*/
   return occ;
}
void LZEnd::get_rows(int* nmin,int* nmax,int first,int last,unsigned char c,unsigned int* C){
   if(last==first && last==-1){
      first=this->C[c]+1;
      last=this->C[c+1];
   }else{
      /*printf("**> %d %d %d %d %d %c\n",C[c],Occ(c,first-1),Occ(c,last),first,last,c);*/
      first=this->C[c]+this->Occ(c,first-1)+1;
      last=this->C[c]+this->Occ(c,last);
   }
   if(first>last){
      //std::cerr<<"error in get rows "<<first<<" "<<last<<std::endl;
      *nmin=-1;
      *nmax=-1;
   }else{
      *nmin=first;
      *nmax=last;
   }
}

unsigned char LZEnd::L(int pos){
    return this->La[pos];
    /*int t = this->SA(pos);
    if(t==0)return '\0';
    return this->text[this->text_len - 2 - (t-1)];*/
}
/*void LZEnd::check(){
    for(int i=0;i<this->text_len;i++){
        if(this->L[i]!=this->LL(i))std::cout<<L[i]<<" "<<LL(i)<<" "<<i<<std::endl;
    }  
}*/
/*int getMax(unsigned int* sa, int start, int end){
    int maxpos=start;
    int i;
    if(start>end)return -1;
    for(i=start+1;i<=end;i++){
        if(sa[i]>sa[maxpos])maxpos=i;
    }
    return maxpos;
}*/

unsigned int LZEnd::add_bits(unsigned int pos, unsigned int len){
    unsigned int b=0;
    b += this->log_sigma;//trailing character
    b += this->log(this->factors);//start of source
    //b += this->log(pos);//start of source
    b += 1 + 2*(this->log(this->log(len))-1) + (this->log(len)-1);//len of source 
    return b;
}

}
}



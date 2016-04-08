#include "utils_index.h"
#include "basics.h"

#include <cstdlib>
#include <iostream>
#include <algorithm>

namespace lz77index{
namespace utils_index{

/*len is the size of sst_a and len-1 the size of prev_a*/
/*unsigned int getRangeArray(unsigned int* sst_a, unsigned int* prev_a, unsigned int len){
    unsigned int* range_a = new unsigned int[len-1];
    for(unsigned int i=0;i<len-1;i++){
        for(unsigned int j=0;j<len;j++){
            if(sst_a[j] == prev_a[i] + 1){
                range_a[i]=j;
                break;
            }
        }
    }
    return range_a;
}*/
//len is the size of both arrays
unsigned int* getRangeArray(unsigned int* sst_a, unsigned int* prev_a, unsigned int len){
    unsigned int* range_a = (unsigned int*)malloc(sizeof(unsigned int)*(1+len));//we use malloc becuase it will be freed by range
    range_a[0] = 0;
    for(unsigned int i=0;i<len;i++){//x
        for(unsigned int j=0;j<len;j++){//y
            if(sst_a[i] == prev_a[j]){
                range_a[i+1] = j+1;
                break;
            }
        }
    }
    return range_a;
}
template<class T> struct index_cmp {
    index_cmp(const T arr) : arr(arr) {}
    bool operator()(const size_t a, const size_t b) const{ 
        return arr[a] < arr[b];
    }
    const T arr;
};

/*bool cmp_pairs(pair a, pair b){
     return a.first < b.first;
}*/
unsigned int* getRangeArray2(unsigned int* sst_a, unsigned int* prev_a, unsigned int len){
    //std::pair<unsigned int, unsigned int>* aux = new std::pair<unsigned int , unsigned int>[len];
    size_t* idx = new size_t[len];
    for(unsigned int i=0;i<len;i++){
        //aux[i] = std::pair<unsigned int,unsigned int>(prev_a[i],i);
        idx[i] = i;
    }
    //std::sort(aux,aux+len,cmp_pairs);
    std::sort(idx,idx+len,index_cmp<unsigned int*>(prev_a));
    unsigned int* range_a = (unsigned int*)malloc(sizeof(unsigned int)*(1+len));//we use malloc becuase it will be freed by range
    range_a[0] = 0;
    for(unsigned int i=0;i<len;i++){//x
        //range_a[i+1] = aux[i].second + 1;        
        range_a[i+1] = idx[sst_a[i]] + 1;
        /*for(unsigned int j=0;j<len;j++){//y
            if(sst_a[i] == prev_a[j]){
                range_a[i+1] = j+1;
                break;
            }
        }*/
    }
    delete [] idx;
    return range_a;
}

bool comparePhrases(Phrase p1,Phrase p2){
    return (p1.start_pos < p2.start_pos || (p1.start_pos == p2.start_pos && p1.length > p2.length) );
}

//returns the maximum depth
unsigned char* computeDepth(Phrase* phrases, unsigned int n){
    unsigned char* depth =  new unsigned char[n];
    std::sort(phrases, phrases + n, comparePhrases);
    //calcula mal los depth
    /*std::stack<Phrase> stack = std::stack<Phrase>();
    for(unsigned int i=0;i<n;i++){
        while(!stack.empty()){
            Phrase p = stack.top();
            if(p.start_pos + p.length > phrases[i].start_pos + phrases[i].length || (p.start_pos < phrases[i].start_pos && p.start_pos + p.length == phrases[i].start_pos + phrases[i].length) ){
                if(stack.size()+1>255){
                    std::cerr<<"ERROR: the depth is higher than 255 change to unsigned int"<<std::endl;
                    exit(1);
                }
                depth[i] = stack.size() + 1;
                stack.push(phrases[i]);
                break;
            }else{
                stack.pop();
            }
        }
        if(stack.empty()){
            stack.push(phrases[i]);
            depth[i] = 1;
        }
    }*/
    std::cout<<"N:"<<n<<std::endl;
    unsigned int from = 0;
    for(unsigned int i=0;i<n;i++){
        depth[i] = 0;
        while(phrases[from].start_pos + phrases[from].length < phrases[i].start_pos){
                from++;
        }
        for(unsigned j=from;j<i;j++){
            if((phrases[j].start_pos + phrases[j].length > phrases[i].start_pos + phrases[i].length || (phrases[j].start_pos < phrases[i].start_pos && phrases[j].start_pos + phrases[j].length == phrases[i].start_pos + phrases[i].length) ) && depth[j]>=depth[i]){
                depth[i] = depth[j]+1;
            }
        }
    }
    
    //damos vuelta las frases que empiezan en la misma posicion
    unsigned int prev = phrases[0].start_pos;
    unsigned int prev_i = 0;
    for(unsigned int i=1;i<n;i++){
        if(phrases[i].start_pos != prev){
            std::reverse(phrases+prev_i,phrases+i);
            std::reverse(depth+prev_i,depth+i);
            prev_i = i;
            prev = phrases[i].start_pos;
        }
    }
    std::reverse(phrases+prev_i,phrases+n);
    std::reverse(depth+prev_i,depth+n);
    
    unsigned int dt = 0;
    for(unsigned int i=0;i<n;i++){
        dt += depth[i];
    }
    std::cout<<"Mean Depth: "<<dt*1.0/n<<" Max Depth:"<<(unsigned int)*std::max_element(depth,depth + n)<<std::endl;
    return depth;
}

unsigned int* computeSourcesBitmap(Phrase* phrases, unsigned int n){
    //std::sort(phrases, phrases + n, comparePhrases); sorted by computeDepth
    unsigned int* ret =  new unsigned int[n];
    ret[0] = phrases[0].start_pos + 1;//that should be equal to 1 
    for(unsigned int i=1;i<n;i++){
        ret[i] = phrases[i].start_pos - phrases[i-1].start_pos + 1;
    }
    return ret;
}
unsigned int* computePermutationArray(Phrase* phrases, unsigned int n){
    //std::sort(phrases, phrases + n, comparePhrases); sorted by computeDepth
    //invert permutation
    unsigned int* temp = new unsigned int[n];
    for(unsigned int i=0; i<n ; i++){
            temp[phrases[i].id] = i;
    }
    unsigned int b = basics::bits(n-1);
    unsigned int s = basics::uint_len(n,b);
    unsigned int* array = new unsigned int[s];
    for(unsigned int i=0;i<s;i++)array[i]=0;
    for(unsigned int i=0;i<n;i++){
        basics::set_field(array,b,i,temp[i]);
    }
    delete [] temp;
    return array;
}


/* given an array of integeres return a new array in wich each value correspond to the rank of the value
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
}*/

}
}

#include "static_selfindex_lz77.h"

#include <algorithm>
#include "utils.h"
#include <string>

namespace lz77index{
    void hstats(char* file){
        std::string sfile(file);
        unsigned int n = utils::fileLength(file);
        unsigned int len;
        unsigned int* length = (unsigned int*)utils::readArray((sfile+".len").c_str(),&len,4);
        unsigned int* start = (unsigned int*)utils::readArray((sfile+".start").c_str(),&len,4);
        unsigned int* h = new unsigned int[n+2];
        for(unsigned int i=0;i<n+2;i++)h[i]=0;
        unsigned int max=0;
        double sum=0.0;
        unsigned int pos=0;
        for(unsigned int i=0; i<len; i++){
            for(unsigned int j=0;j<length[i]-1;j++){
                h[pos]=h[start[i]+j]+1;
                if(h[pos]>max)max=h[pos];
                sum+=h[pos];
                pos++;
            }
            h[pos]=1;
            if(h[pos]>max)max=h[pos];
            sum+=h[pos];
            pos++;
        }
        /*std::cout<<"pos:"<<pos<<std::endl;
        std::cout<<"len:"<<len<<std::endl;
        std::cout<<"n:"<<n<<std::endl;*/
        std::cout<<sum/n<<" "<<max<<std::endl;
    }
}
int main(int argc, char** argv){
    lz77index::hstats(argv[1]);
}

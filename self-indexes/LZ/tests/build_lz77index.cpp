#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../static_selfindex.h"
#include "../utils.h"

int main(int argc,char** argv){
    std::cout << "Creating Index"<<std::endl;
    unsigned char br=0;
    unsigned char bs=0;
    unsigned char ss=0;
    for(int i=3;i<argc;i++){
        if(strcmp(argv[i],"brev")==0)br=1;
        else if(strcmp(argv[i],"bsst")==0)bs=1;
        else if(strcmp(argv[i],"ssst")==0)ss=1;
    }
    if(!bs){
        ss = 0;
    }
    lz77index::utils::startTime();
    lz77index::static_selfindex* idx = lz77index::static_selfindex_lz77::build(argv[1],argv[2], br, bs, ss);
    lz77index::utils::endTimeV();
    if(idx==NULL){
        std::cerr<<"Index is NULL"<<std::endl;
        return EXIT_FAILURE;
    }
    delete idx;
    return EXIT_SUCCESS;
}


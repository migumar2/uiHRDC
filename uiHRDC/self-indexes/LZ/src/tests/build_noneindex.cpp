#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../static_selfindex.h"
#include "../utils.h"

int main(int argc,char** argv){
    std::cout << "Creating Index"<<std::endl;
    lz77index::utils::startTime();
    lz77index::static_selfindex* idx = lz77index::static_selfindex_none::build(argv[1]);
    lz77index::utils::endTimeV();
    if(idx==NULL){
        std::cerr<<"Index is NULL"<<std::endl;
        return EXIT_FAILURE;
    }
    delete idx;
    return EXIT_SUCCESS;
}


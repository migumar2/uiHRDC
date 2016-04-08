#ifndef __UTILS_H__
#define __UTILS_H__

namespace lz77index{

namespace utils{

    void startTime();
    double endTimeV();
    double endTime();
    int fileLength(char const* filename);
    unsigned char* readText(char const* filename, unsigned int* length);
    unsigned char* readFile(char const* filename, unsigned int* length);
    unsigned char* readArray(char const* filename, unsigned int* length, unsigned int size);
    int saveArray(unsigned char const* array,int length,char const* filename);
    unsigned char* reverseText(unsigned char const* text, int length);
    unsigned int* getSA(unsigned char const* text, int length, char const* filename);
    unsigned int* invertSA(unsigned int const* sa, int length);
    unsigned char* getL(unsigned int const* sa, unsigned char const* text, unsigned int length);
    unsigned int* getC(unsigned int const* sa, unsigned char const* text, unsigned int length);
    unsigned int* compressArray(unsigned int* array, unsigned int len, unsigned char* bits);

}
}


#endif

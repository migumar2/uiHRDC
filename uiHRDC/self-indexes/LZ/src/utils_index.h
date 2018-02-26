namespace lz77index{
namespace utils_index{
unsigned int* getRangeArray(unsigned int* sst_a, unsigned int* prev_a, unsigned int len);//time n^2
unsigned int* getRangeArray2(unsigned int* sst_a, unsigned int* prev_a, unsigned int len);//time nlogn
typedef struct sPhrase{
    unsigned int start_pos;
    unsigned int length;
    unsigned int id;
}Phrase;
//returns the depth array
unsigned char* computeDepth(Phrase* phrases, unsigned int n);
unsigned int* computeSourcesBitmap(Phrase* phrases, unsigned int n);
unsigned int* computePermutationArray(Phrase* phrases, unsigned int n);

}
}

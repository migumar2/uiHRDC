#include <fstream>
#include <libcdsBasics.h>

using namespace std;
using namespace cds_utils;

class DeltaCodes{
    public:
	DeltaCodes();
        DeltaCodes(unsigned int* array, unsigned int n, unsigned int sampling);
        virtual ~DeltaCodes();

        unsigned int select(unsigned int total);
        unsigned int rank(unsigned int pos);
        unsigned int access(unsigned int pos);
        unsigned int rank_select(unsigned int pos, unsigned int* select);
        unsigned int size();
	unsigned int elements();

	int save(ofstream & output);
	static DeltaCodes* load(ifstream & input);

    private:
        unsigned int* deltacodes;
        unsigned int* sampled_pointer;
        unsigned int* sampled_total;
        unsigned int n;
        unsigned int sampling;
        unsigned int total_size;
        unsigned int decodeDelta(unsigned int* deltaCodes, unsigned int* pos);
        void encodeDelta(unsigned int val, unsigned int* deltaCodes, unsigned int* pos);
        unsigned int bits(unsigned int n);
};

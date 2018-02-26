
#include "basics.h"  //*añadido por fari 2013, para get_fieldLL incluido en subclases.

class DeltaCodes{
    public:
        DeltaCodes(unsigned int* array, unsigned int n, unsigned int sampling);
        virtual ~DeltaCodes();
        unsigned int select(unsigned int total);
        unsigned int rank(unsigned int pos);
        unsigned int access(unsigned int pos);
        unsigned int rank_select(unsigned int pos, unsigned int* select);
        unsigned int size();
	
	unsigned int get_n(); /**fari 2013, only for statistics in ilist.cpp **/
	unsigned int get_total_size_bits(); /**fari 2013, only for statistics in ilist.cpp **/
	unsigned int get_sampled_pointer_bits(); /**fari 2013, only for statistics in ilist.cpp **/
	unsigned int get_sampled_pointer_nints(); /**fari 2013, only for statistics in ilist.cpp **/
	
    private:
        unsigned int* deltacodes;

	unsigned int* sampled_pointer;
	unsigned int sampled_pointer_bits, sampled_pointer_nints;
	
        unsigned int* sampled_total;
        unsigned int n;
        unsigned int sampling;
        unsigned int total_size;
        unsigned int decodeDelta(unsigned int* deltaCodes, unsigned int* pos);
        void encodeDelta(unsigned int val, unsigned int* deltaCodes, unsigned int* pos);
        unsigned int bits(unsigned int n);
};

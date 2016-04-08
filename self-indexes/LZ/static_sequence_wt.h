#ifndef _STATIC_SEQUENCE_WT_H
#define _STATIC_SEQUENCE_WT_H

#define STATIC_SEQUENCE_WT_HDR 5
#include "wt_node.h"

namespace lz77index{

class static_sequence_wt{
	protected:
	    wt_int_node* root;
        unsigned int length;
	public:
	    unsigned char alphabet_size;
	    static_sequence_wt(unsigned char const* seq, unsigned int n, unsigned char sigma);
        static_sequence_wt();
	    ~static_sequence_wt();
	    unsigned int rank(unsigned int symbol, unsigned int pos) const;
	    unsigned int select(unsigned int symbol, unsigned int cant) const;
	    unsigned char access(unsigned int pos) const;
        unsigned int prevLessSymbol(unsigned int pos, unsigned int symbol) const;
	    unsigned int size() const;
        unsigned int save(FILE *fp) const;
        static static_sequence_wt* load(FILE *fp);
};

}
#endif

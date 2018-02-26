#ifndef _WT_NODE_H
#define _WT_NODE_H
#include <cstdio>

#define WT_INT_NODE_HDR 2
#define WT_EXT_NODE_HDR 3
namespace lz77index{
class wt_node{
	public:
    virtual ~wt_node(){}
	virtual unsigned int rank(unsigned char symbol, unsigned int pos, unsigned char alphabet_size) const = 0;
	virtual unsigned int select(unsigned char symbol, unsigned int cant, unsigned char alphabet_size) const = 0;
	virtual unsigned char access(unsigned int pos, unsigned char alphabet_size) const = 0;
    virtual unsigned int prevLessSymbol(unsigned char symbol, unsigned int pos, unsigned char alphabet_size) const = 0;
	virtual unsigned int size() const = 0;
    virtual unsigned int save(FILE *fp) const = 0;
    static wt_node* load(FILE *fp);
};
}
#endif

#ifndef _WT_INT_NODE_H
#define _WT_INT_NODE_H
#include "static_bitsequence.h"
namespace lz77index{
class wt_int_node:public wt_node{
	protected:
	    basics::static_bitsequence* bitmap;
	    wt_node* left;
	    wt_node* right;
	public:
	    wt_int_node(unsigned char const* seq, unsigned int n, unsigned char sigma, unsigned char min_symbol);
        wt_int_node();
	    ~wt_int_node();
	    unsigned int rank(unsigned char symbol, unsigned int pos, unsigned char alphabet_size) const;
	    unsigned int select(unsigned char symbol, unsigned int cant, unsigned char alphabet_size) const;
	    unsigned char access(unsigned int pos, unsigned char alphabet_size) const;
        unsigned int prevLessSymbol(unsigned char symbol, unsigned int pos, unsigned char alphabet_size) const;
	    unsigned int size() const;
        unsigned int save(FILE *fp) const;
        static wt_int_node* load(FILE *fp);
};
#endif
}
#ifndef _WT_EXT_NODE_H
#define _WT_EXT_NODE_H
namespace lz77index{
class wt_ext_node:public wt_node{
	protected:
	    unsigned int count;
	    unsigned char symbol;
	public:
	    wt_ext_node(unsigned int symbol, unsigned int cant);
        wt_ext_node();
        ~wt_ext_node();
	    unsigned int rank(unsigned char symbol,unsigned int pos, unsigned char alphabet_size) const;
	    unsigned int select(unsigned char symbol, unsigned int cant, unsigned char alphabet_size) const;
	    unsigned char access(unsigned int pos, unsigned char alphabet_size) const;
        unsigned int prevLessSymbol(unsigned char symbol, unsigned int pos, unsigned char alphabet_size) const;
	    unsigned int size() const;
        unsigned int save(FILE *fp) const;
        static wt_ext_node* load(FILE *fp);
};
}
#endif

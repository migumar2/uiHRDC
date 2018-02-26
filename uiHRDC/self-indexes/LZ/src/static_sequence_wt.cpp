#include "static_sequence_wt.h" 
#include <iostream>
namespace lz77index{

static_sequence_wt::static_sequence_wt(unsigned char const* seq, unsigned int n, unsigned char sigma){
    if(sigma == 1)//this is to avoid border cases 
        sigma = 2;
	root = new wt_int_node(seq, n, sigma, 0);
	alphabet_size = sigma;
    length = n;
}

static_sequence_wt::static_sequence_wt(){
	root = NULL;
	alphabet_size = 0;
    length = 0;
}

static_sequence_wt::~static_sequence_wt(){
	if(root != NULL)
        delete root;
}

unsigned int static_sequence_wt::rank(unsigned int symbol, unsigned int pos) const{
    if(symbol >= alphabet_size)
        return 0;
	return root->rank(symbol, pos, alphabet_size);
}

unsigned int static_sequence_wt::select(unsigned int symbol, unsigned int cant) const{
	if(cant == 0 || symbol >= alphabet_size){
        return 0xFFFFFFFF;
    }
	return root->select(symbol, cant, alphabet_size) - 1;
}

unsigned char static_sequence_wt::access(unsigned int pos) const{
    if(pos>length){
        return 0XFF;
    }
	return root->access(pos, alphabet_size);
}

unsigned int static_sequence_wt::prevLessSymbol(unsigned int pos, unsigned int symbol) const{
    if(pos>=length){
        return pos=length-1;
    }
	return root->prevLessSymbol(symbol, pos, alphabet_size);
}

unsigned int static_sequence_wt::size() const{
	return root->size()+sizeof(static_sequence_wt);
}

unsigned int static_sequence_wt::save(FILE * fp) const{ 
    unsigned int wr;
    unsigned char hdr = STATIC_SEQUENCE_WT_HDR;
    wr = fwrite(&hdr, sizeof(unsigned char), 1, fp);
    if(wr != 1)
        return 1;
    wr = fwrite(&alphabet_size, sizeof(unsigned char), 1,fp);
    if(wr != 1)
        return 1;
    wr = fwrite(&length, sizeof(unsigned int), 1, fp);
    if(wr != 1)
        return 1;
    if(root->save(fp))
        return 1;
    return 0;
}

static_sequence_wt* static_sequence_wt::load(FILE *fp) {
    unsigned char hdr;
    if(fread(&hdr, sizeof(unsigned char), 1, fp) != 1)
        return NULL;
    if(hdr != STATIC_SEQUENCE_WT_HDR)
        return NULL;
    static_sequence_wt* ret = new static_sequence_wt();
    if(fread(&ret->alphabet_size, sizeof(unsigned char), 1, fp) != 1){
        delete ret;
        return NULL;
    }
    if(fread(&ret->length, sizeof(unsigned int), 1, fp) != 1){
        delete ret;
        return NULL;
    }
    ret->root = wt_int_node::load(fp);
    if(ret->root == NULL){
        delete ret;
        return NULL;
    }
    return ret;
}

}

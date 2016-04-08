#include "wt_node.h"
#include "static_bitsequence_brw32.h"
#include "basics.h"

namespace lz77index{
/**************************************************/
/* Node                    ************************/
/**************************************************/
wt_node* wt_node::load(FILE *fp) {
    unsigned char hdr;
    if(fread(&hdr, sizeof(unsigned char), 1, fp) != 1)
        return NULL;
    fseek(fp,-sizeof(unsigned char),SEEK_CUR);
    switch(hdr) {
        case WT_INT_NODE_HDR:
            return wt_int_node::load(fp);
        case WT_EXT_NODE_HDR:
            return wt_ext_node::load(fp);
    }
    return NULL;
}

/**************************************************/
/* Int Node                ************************/
/**************************************************/
wt_int_node::wt_int_node(unsigned char const* seq, unsigned int n, unsigned char alphabet_size, unsigned char min_symbol){
	unsigned int* bm = new unsigned int[n/32+1];
    bm[n/32] = 0;
    //for(int i=0;i<n/32+1;i++)bm[i] = 0;
	unsigned int med = (alphabet_size+1)/2;
	unsigned int left_c = 0;
    /*count how many characters go to the left*/
	for(unsigned int i=0;i<n;i++){
		if(seq[i]<med){//left
			basics::bitclean(bm,i);
			left_c++;
		}else{//rigth
			basics::bitset(bm,i);
		}
	}
    //std::cout<<"Left: "<<left_c<<std::endl;
	bitmap = new basics::static_bitsequence_brw32(bm,n,20);
    delete[] bm;
    /*compute left and right sequences*/
	unsigned char* left_seq = new unsigned char[left_c];
	unsigned char* right_seq = new unsigned char[n-left_c];
	int lpos = 0;
	int rpos = 0;
	for(unsigned int i=0;i<n;i++){
		if(seq[i]<med){
			left_seq[lpos] = seq[i];
			lpos++;
		}else{
			right_seq[rpos] = seq[i]-med;
			rpos++;		
		}
	}
    /*recursively builts the subtrees if sigma=1 then is a leaf*/
    /*the count could be 0, may be better to leave it as NULL*/
	if(med==1){
		left = new wt_ext_node(min_symbol,left_c);
	}else{
		left = new wt_int_node(left_seq, left_c, med, min_symbol);
	}
	if(alphabet_size - med == 1){
		right = new wt_ext_node(med + min_symbol, n - left_c);
	}else{
		right = new wt_int_node(right_seq, n - left_c, alphabet_size - med, med + min_symbol);		
	}
    delete[] left_seq;
    delete[] right_seq;
}

wt_int_node::wt_int_node(){
    bitmap = NULL;
    left = NULL;
    right = NULL;
}
wt_int_node::~wt_int_node(){
    if(bitmap != NULL)
        delete bitmap;
    if(left != NULL)
        delete left;
    if(right != NULL)
        delete right;
}
	
unsigned int wt_int_node::rank(unsigned char symbol, unsigned int pos, unsigned char alphabet_size) const{
    unsigned char med = (alphabet_size+1)/2; 
	if(symbol < med){//left
			if(bitmap->rank0(pos) == 0) 
                return 0;
			return left->rank(symbol, bitmap->rank0(pos) - 1, med);
	}else{//right
    	if(bitmap->rank1(pos) == 0) 
            return 0;
	    return right->rank(symbol - med, bitmap->rank1(pos) - 1, alphabet_size - med);
    }
}

unsigned int wt_int_node::select(unsigned char symbol, unsigned int cant, unsigned char alphabet_size) const{
	unsigned char med = (alphabet_size+1)/2;
    unsigned int s,chs;
    if(symbol<med){//left
        chs = left->select(symbol, cant, med);
        if(chs == 0xFFFFFFFF) s = 0xFFFFFFFF;
        else s = bitmap->select0(chs)+1;
    }else{//right
        chs = right->select(symbol-med, cant, alphabet_size-med);
        if(chs == 0xFFFFFFFF) s = 0xFFFFFFFF;
        else s = bitmap->select1(chs)+1;
    }
    return s;
}

unsigned char wt_int_node::access(unsigned int pos, unsigned char alphabet_size) const{
	if(bitmap->access(pos) == 0){//left
		return left->access(bitmap->rank0(pos)-1,(alphabet_size+1)/2);
    }else{//right
    	return right->access(bitmap->rank1(pos)-1,alphabet_size-(alphabet_size+1)/2);
    }
}

unsigned int wt_int_node::prevLessSymbol(unsigned char symbol, unsigned int pos, unsigned char alphabet_size) const{
    unsigned char med = (alphabet_size + 1)/2;
    unsigned int r,pl,pos2,mr0;
    if(symbol < med){//left
        r = bitmap->rank0(pos);//rank0
        if(r == 0){
            return 0xFFFFFFFF;
        }
        pl = left->prevLessSymbol(symbol, r - 1, med);
        pos2 = 0xFFFFFFFF;
        if(pl != 0xFFFFFFFF){
            pos2 = bitmap->select0(pl+1);
        }
        return pos2;
    }else{//right
        if(bitmap->access(pos)){
            r = bitmap->rank1(pos);//rank1
            //most right zero          
            mr0 = bitmap->select0(pos+1-r);
            pl = right->prevLessSymbol(symbol-med, r-1, alphabet_size-med);
            pos2 = 0xFFFFFFFF;
            if(pl != 0xFFFFFFFF){
                pos2 = bitmap->select1(pl+1);
                if(mr0==0xFFFFFFFF)
                    return pos2; 
                return max(pos2,mr0); 
            }
            return mr0;
        }else{
            return pos;
        }
    }

}

unsigned int wt_int_node::size() const{
	return sizeof(wt_int_node) + left->size() + right->size() + bitmap->size();
}

unsigned int wt_int_node::save(FILE *fp) const {
    unsigned int wr;
    unsigned char hdr = WT_INT_NODE_HDR;
    wr = fwrite(&hdr, sizeof(unsigned char), 1, fp);
    if(wr != 1)
        return 1;
    if(bitmap->save(fp))
        return 1;
    if(left->save(fp))
        return 1;
    if(right->save(fp))
        return 1;
    return 0;
}

wt_int_node* wt_int_node::load(FILE *fp) {
    unsigned char hdr;
    if(fread(&hdr, sizeof(unsigned char), 1, fp) != 1)
        return NULL;
    if(hdr != WT_INT_NODE_HDR)
        return NULL;
    wt_int_node* ret = new wt_int_node();
    ret->bitmap = basics::static_bitsequence::load(fp);
    if(ret->bitmap == NULL){
        delete ret;
        return NULL;
    }
    ret->left = wt_node::load(fp);
    if(ret->left == NULL){
        delete ret;
        return NULL;
    }
    ret->right = wt_node::load(fp);
    if(ret->right == NULL){
        delete ret;
        return NULL;
    }
    return ret;
}

/**************************************************/
/* Ext Node                ************************/
/**************************************************/
wt_ext_node::wt_ext_node(unsigned int symbol, unsigned int cant){
	this->count = cant;
	this->symbol = symbol;
    //std::cout<<"Ext Node created:"<<(unsigned int)this->symbol<<" "<<this->count<<std::endl;
}

wt_ext_node::wt_ext_node(){
	this->count = 0;
	this->symbol = 0;
}

wt_ext_node::~wt_ext_node(){
}

unsigned int wt_ext_node::rank(unsigned char symbol,unsigned int pos,unsigned char alphabet_size) const{
	if(pos < count)
		return pos + 1;
	return 0xFFFFFFFF;
}

unsigned int wt_ext_node::select(unsigned char symbol, unsigned int cant, unsigned char alphabet_size) const{
	if(cant<=count && cant!=0){
        return cant;
    }
    return 0xFFFFFFFF;
}

unsigned char wt_ext_node::access(unsigned int pos, unsigned char alphabet_size) const{
	return symbol;
}

unsigned int wt_ext_node::prevLessSymbol(unsigned char symbol, unsigned int pos, unsigned char alphabet_size) const{
    if(pos >= this->count){
        return 0xFFFFFFFF;
    }
    return pos;
}

unsigned int wt_ext_node::size() const{
	return sizeof(wt_ext_node);
}

unsigned int wt_ext_node::save(FILE *fp) const{
    unsigned int wr;
    unsigned char hdr = WT_EXT_NODE_HDR;
    wr = fwrite(&hdr, sizeof(unsigned char), 1, fp);
    wr += fwrite(&symbol, sizeof(unsigned char), 1, fp);
    wr += fwrite(&count, sizeof(unsigned int), 1, fp);
    return wr-3;
}

wt_ext_node* wt_ext_node::load(FILE *fp) {
    unsigned int rd;
    unsigned char hdr;
    if(fread(&hdr, sizeof(unsigned char), 1, fp) != 1) 
        return NULL;
    if(hdr != WT_EXT_NODE_HDR) 
        return NULL;
    wt_ext_node* ret = new wt_ext_node();
    rd = fread(&(ret->symbol), sizeof(unsigned char),1,fp);
    rd += fread(&(ret->count), sizeof(unsigned int),1,fp);
    if(rd != 2){
        delete ret;
        return NULL;
    }
    return ret;
}

}


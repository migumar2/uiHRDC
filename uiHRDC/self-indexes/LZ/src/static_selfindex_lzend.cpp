#include "static_selfindex_lzend.h"

#include <algorithm>

#include "utils.h"
#include "utils_index.h"
#include "mapper.h"
#include "LZparser.h"
#include "LZ77.h"
#include "LZend.h"
#include "patricia.h"
#include "static_bitsequence_builder.h"
#include "directcodes.h"

static const int DELTA_SAMP = 16;
static const int PERM_EPS = 8;
static const int BITM_STEP = 20;


#include <string>
namespace lz77index{
static_selfindex_lzend* static_selfindex_lzend::build(char* filename, char* filenameidx, unsigned char binaryRev, unsigned char binarySst, unsigned char store_sst_ids){

    unsigned char bits;
    std::string sfile = std::string((char*)filename);

    std::cout<<"Building the index to file "<<filenameidx<<std::endl;
    FILE* lz77index_fp = fopen(filenameidx,"w");
    //0) Save the header
    unsigned char hdr = SELFINDEX_LZEND_HDR;
    if(fwrite(&hdr,sizeof(unsigned char),1,lz77index_fp)!=1){
        std::cerr<<"ERROR: while saving the header"<<std::endl;
        return NULL;
    }
    //0') Save the options
    if(fwrite(&binaryRev,sizeof(unsigned char),1,lz77index_fp)!=1){
        std::cerr<<"ERROR: while saving option"<<std::endl;
        return NULL;
    }
    if(fwrite(&binarySst,sizeof(unsigned char),1,lz77index_fp)!=1){
        std::cerr<<"ERROR: while saving option"<<std::endl;
        return NULL;
    }
    if(fwrite(&store_sst_ids,sizeof(unsigned char),1,lz77index_fp)!=1){
        std::cerr<<"ERROR: while saving option"<<std::endl;
        return NULL;
    }
    /************************************************************************************************************************************************************************************************/
    //1) Read the file into an array
    unsigned int tlen;
    unsigned char* text = utils::readText((char*)filename,&tlen);
    if(text == NULL){
        std::cerr<<"ERROR: reading file"<<sfile<<std::endl;
        return NULL;
    }
    if(fwrite(&tlen,sizeof(unsigned int),1,lz77index_fp)!=1){
        std::cerr<<"ERROR: while saving the text size"<<std::endl;
        return NULL;
    }
    /************************************************************************************************************************************************************************************************/
    //2) Create the alphabet mapper
    mapper* sigma_mapper = new mapper(text,tlen);
    unsigned int sigma = sigma_mapper->sigma();
    std::cout<<"Sigma: "<<sigma<<std::endl;
    /************************************************************************************************************************************************************************************************/
    //3) Convert the text to the new alphabet and save it to a file
    sigma_mapper->map(text,tlen);
    utils::saveArray(text,tlen,(sfile+std::string(".alph")).c_str());
    /************************************************************************************************************************************************************************************************/
    //4) Save mapper
    if(sigma_mapper->save(lz77index_fp)!=0){
        std::cerr<<"ERROR: while saving the alphabet mapper"<<std::endl;
        return NULL; 
    }
    std::cout<<"(S)Mapper: "<<sigma_mapper->size()<<std::endl;
    /************************************************************************************************************************************************************************************************/
    //5) Free memory(mapper and text)
    delete sigma_mapper;sigma_mapper=NULL;
    delete[] text;
    /************************************************************************************************************************************************************************************************/
    //6) Compute LZ77(or like) parsing over the new text, it will generate three different files, .len, .start and .char
    parsing::LZparser* parser = new parsing::LZEnd((sfile+std::string(".alph")).c_str());
    parser->parse();
    delete parser;
    /************************************************************************************************************************************************************************************************/    
    //7) Load .char array and save it to lz77index_fp using nlog(sigma) bits
    unsigned int parsing_len;
    unsigned char* trailing_char = (unsigned char*)utils::readArray((sfile+std::string(".alph.char")).c_str(),&parsing_len,1);
    std::cout<<"Factors: "<<parsing_len<<std::endl;
    //we will store all character up to parsing_len-1 as char[i]-1, and the last one as 0
    //because we know that always the last character is 0, thus we reduce sigma by one
    unsigned int* trailing_char_compressed = new unsigned int[basics::uint_len(parsing_len,basics::bits(sigma-1))];
    for(unsigned int i=0;i<basics::uint_len(parsing_len,basics::bits(sigma-1));i++){
        trailing_char_compressed[i] = 0;
    }
    for(unsigned int i=0;i<parsing_len-1;i++){
        basics::set_field(trailing_char_compressed, basics::bits(sigma-1), i, trailing_char[i]-1);
    }
    basics::set_field(trailing_char_compressed, basics::bits(sigma-1), parsing_len-1, 0);
    if(fwrite(&parsing_len,sizeof(unsigned int),1,lz77index_fp)!=1){
        std::cerr<<"ERROR: while saving the parsing size"<<std::endl;
        return NULL; 
    }
    if(fwrite(trailing_char_compressed,sizeof(unsigned int),basics::uint_len(parsing_len,basics::bits(sigma-1)),lz77index_fp)!=basics::uint_len(parsing_len,basics::bits(sigma-1))){
        std::cerr<<"ERROR: while saving the trailing character array"<<std::endl;
        return NULL; 
    }
    std::cout<<"(S)Trailing character: "<<sizeof(unsigned int)*basics::uint_len(parsing_len,basics::bits(sigma-1))<<std::endl;
    delete[] trailing_char;
    delete[] trailing_char_compressed;
    /************************************************************************************************************************************************************************************************/
    //8) Compute bitmap of phrases    
    unsigned int* length = (unsigned int*)lz77index::utils::readArray((sfile+".alph.len").c_str(),&parsing_len,4);
    DeltaCodes* phrases = new DeltaCodes(length,parsing_len,DELTA_SAMP);//sampling=8
    //delete [] length;
    if(phrases->save(lz77index_fp)!=0){
        std::cerr<<"ERROR: while saving the DeltaCodes bitmap"<<std::endl;
        return NULL;
    }
    std::cout<<"(S)Phrases: "<<phrases->size()<<std::endl;
    delete phrases;

    /************************************************************************************************************************************************************************************************/   
    //8) Load text and compute sparse suffix tree
    text = lz77index::utils::readText((sfile+".alph").c_str(),&tlen);
    patricia* pat_tree = new patricia(text,tlen);
    pat_tree->add(length[0]);//second phrase starts at position length[0]=1

    for(unsigned int i=1;i<parsing_len-1;i++){
        length[i] = length[i] + length[i-1];
        pat_tree->add(length[i]);
    }
    length[parsing_len-1] = length[parsing_len-1] + length[parsing_len-2];

    unsigned int* ids = pat_tree->leafIds();

    unsigned int* dfuds_bm=NULL;unsigned char* dfuds_labels=NULL; unsigned int* dfuds_skips=NULL;
    if(!binarySst){
    dfuds_bm = pat_tree->dfuds();//size = (patricia->nodes*2)/32+1
    dfuds_labels = pat_tree->dfuds_labels();//size=patricia->nodes
    dfuds_skips = pat_tree->dfuds_skips();//size=patricia->nodes
    }
    std::cout<<pat_tree->ids()<<" "<<(pat_tree->nodes()*2)/32+1<<" "<<pat_tree->nodes()<<std::endl;
    unsigned int nodes = pat_tree->nodes();

    delete pat_tree;
    basics::FTRep* temp_skip;
    if(!binarySst){
    basics::dfuds* sst = new basics::dfuds(dfuds_bm, nodes, dfuds_labels);
    delete [] dfuds_bm;
    if(sst->save(lz77index_fp) != 0){
        std::cerr<<"ERROR: while saving the sparse suffix tree"<<std::endl;
        return NULL; 
    }
    std::cout<<"(S)sst: "<<sst->size()<<std::endl;
    delete sst;//deletes also dfuds_labels

    temp_skip = basics::createFT(dfuds_skips,nodes); 
    std::cout<<"(S)sst skips: "<<basics::sizeFT(temp_skip)<<std::endl;
    basics::saveFT(temp_skip,lz77index_fp);
    basics::destroyFT(temp_skip);
    delete [] dfuds_skips;
    }
    /************************************************************************************************************************************************************************************************/
    //9 compute reverse trie(similar to previous step)
    patricia* rev_pat_tree = new patricia(text,tlen);
    rev_pat_tree->addReverse(0,1);//first phrase is of length 1 and start in position 0

    for(unsigned int i=1;i<parsing_len-1;i++){//we dont want the last phrase
        rev_pat_tree->addReverse(length[i]-1,length[i]-length[i-1]);
    }
    //rev_pat_tree->print();
    delete [] text;
    delete [] length;
    unsigned int* ids_rev = rev_pat_tree->leafIds();//size = patricia->ids()
    //delete [] ids_rev;
    unsigned int* dfuds_bm_rev=NULL;unsigned char* dfuds_labels_rev=NULL;unsigned int* dfuds_skips_rev=NULL;
    if(!binaryRev){
    dfuds_bm_rev = rev_pat_tree->dfuds();//size = (patricia->nodes*2)/32+1
    dfuds_labels_rev = rev_pat_tree->dfuds_labels();//size=patricia->nodes
    dfuds_skips_rev = rev_pat_tree->dfuds_skips();//size=patricia->nodes
    }
    std::cout<<rev_pat_tree->ids()<<" "<<(rev_pat_tree->nodes()*2)/32+1<<" "<<rev_pat_tree->nodes()<<std::endl;
    unsigned int nodes_rev= rev_pat_tree->nodes();
    delete rev_pat_tree;
    if(!binaryRev){
    basics::dfuds* rev_trie = new basics::dfuds(dfuds_bm_rev, nodes_rev, dfuds_labels_rev);
    delete [] dfuds_bm_rev;
    if(rev_trie->save(lz77index_fp) != 0){
        std::cerr<<"ERROR: while saving the rev trie"<<std::endl;
        return NULL; 
    }
    std::cout<<"(S)rev_trie: "<<rev_trie->size()<<std::endl;
    delete rev_trie;

    temp_skip = basics::createFT(dfuds_skips_rev,nodes_rev); 
    std::cout<<"(S)rev_trie skips: "<<basics::sizeFT(temp_skip)<<std::endl;
    basics::saveFT(temp_skip,lz77index_fp); 
    basics::destroyFT(temp_skip);
    delete [] dfuds_skips_rev;
    }
    /************************************************************************************************************************************************************************************************/
    //10 compute range
    //if we store the ids of rev_tree we createRange(ids,rev_ids) otherwise createRange(rev_ids,ids)
    //this has direct realtion to the skips stored, if we dont save rev_ids we have to store the skips
    //as is faster to extract right to left is better to store the skips of sst, thus storing rev_ids
    std::cout<<"Creating range ..."<<std::endl;
    unsigned int* range_array = utils_index::getRangeArray2(ids,ids_rev,parsing_len-1);//too slow convert to nlogn
    //unsigned int* range_array2 = utils_index::getRangeArray2(ids,ids_rev,parsing_len-1);//too slow convert to nlogn
    std::cout<<"Range done"<<std::endl;
    //recompute the rev_ids to use less memory    
    unsigned int* temp_ids = utils::compressArray(ids_rev, parsing_len-1, &bits);
    delete [] ids_rev;
    if(fwrite(&bits,sizeof(unsigned char),1,lz77index_fp)!=1){
        std::cerr<<"ERROR: while saving the bits of the rev ids"<<std::endl;
        return NULL; 
    }
    if(fwrite(temp_ids,sizeof(unsigned int),basics::uint_len(parsing_len-1,bits),lz77index_fp) != basics::uint_len(parsing_len-1,bits)){
        std::cerr<<"ERROR: while saving the compressed rev ids"<<std::endl;
        return NULL; 
    }
    std::cout<<"(S)rev_ids: "<<sizeof(unsigned int)*basics::uint_len(parsing_len-1,bits)<<std::endl;
    delete [] temp_ids;
    if(store_sst_ids){
    //recompute the ids to use less memory    
    temp_ids = utils::compressArray(ids, parsing_len-1, &bits);
    if(fwrite(&bits,sizeof(unsigned char),1,lz77index_fp)!=1){
        std::cerr<<"ERROR: while saving the bits of the ids"<<std::endl;
        return NULL; 
    }
    if(fwrite(temp_ids,sizeof(unsigned int),basics::uint_len(parsing_len-1,bits),lz77index_fp) != basics::uint_len(parsing_len-1,bits)){
        std::cerr<<"ERROR: while saving the compressed ids"<<std::endl;
        return NULL; 
    }
    std::cout<<"(S)ids: "<<sizeof(unsigned int)*basics::uint_len(parsing_len-1,bits)<<std::endl;
    delete [] temp_ids;
    }
    delete [] ids;

    static_range* range = new static_range_chazelle(range_array,parsing_len);//delete range_array
    if(range->save(lz77index_fp) != 0){
        std::cerr<<"ERROR: while saving the range"<<std::endl;
        return NULL;
    }
    std::cout<<"(S)range: "<<range->size()<<std::endl;
    delete range;
    //11 Compute depth array and bitmap(it is actually a prevless structure)
    //12 compute permutation
    unsigned int len;
    length = (unsigned int*)utils::readArray((sfile+".alph.len").c_str(),&len,4);
    unsigned int* start = (unsigned int*)utils::readArray((sfile+".alph.start").c_str(),&len,4);
    for(unsigned int i=0;i<len;i++){
        length[i] -= 1;
        if(length[i]!=0){
            start[i] += 1;
        }else{
            start[i] = 0;
        }
    }
    utils_index::Phrase* aux_phrases = new utils_index::Phrase[len];
    for(unsigned int i=0;i<len;i++){
        aux_phrases[i].start_pos = start[i];
        aux_phrases[i].length = length[i];
        aux_phrases[i].id = i;
    }
    //std::cout<<std::endl;
    delete [] length;
    delete [] start;
    unsigned char* depth = utils_index::computeDepth(aux_phrases,len);

    unsigned int sigma_d = *std::max_element(depth,depth+len)+1;
    std::cout<<"Sigma depth: "<<sigma_d<<std::endl;
    static_sequence_wt* wt_depth = new static_sequence_wt(depth,len,(unsigned char)sigma_d);
    if(wt_depth->save(lz77index_fp)!=0){
        std::cerr<<"ERROR: while saving the depth WT"<<std::endl;
        return NULL; 
    }
    std::cout<<"(S)depth: "<<wt_depth->size()<<std::endl;
    delete wt_depth;
    delete [] depth;
    unsigned int* permutation_array = utils_index::computePermutationArray(aux_phrases,len);
    basics::static_bitsequence_builder* bmb = new basics::static_bitsequence_builder_brw32(BITM_STEP);
    basics::static_permutation* perm = new basics::static_permutation_mrrr(permutation_array,len,PERM_EPS,bmb);//probar que valor funciona mejor
    delete bmb;
    if(perm->save(lz77index_fp) != 0){
        std::cerr<<"ERROR: while saving the permutation"<<std::endl;
        return NULL;
    }
    std::cout<<"(S)perm: "<<perm->size()<<std::endl;

    delete perm;
    unsigned int* bitmap_sources = utils_index::computeSourcesBitmap(aux_phrases,len);
    phrases = new DeltaCodes(bitmap_sources,len,DELTA_SAMP);//sampling=8

    if(phrases->save(lz77index_fp)!=0){
        std::cerr<<"ERROR: while saving the DeltaCodes sources bitmap"<<std::endl;
        return NULL;
    }
    std::cout<<"(S)sources: "<<phrases->size()<<std::endl;
    delete phrases;
    delete [] bitmap_sources;
    delete [] aux_phrases;
    fclose(lz77index_fp);
    /**************************************************************************/
    /**************************************************************************/
    /**************************************************************************/
    /**************************************************************************/
    std::cout<<"Loading the index from file "<<(filenameidx)<<std::endl;
    static_selfindex_lzend* ret;

    lz77index_fp = fopen(filenameidx,"r");
    ret = static_selfindex_lzend::load(lz77index_fp);
    fclose(lz77index_fp);
    return ret;
}
static_selfindex_lzend::~static_selfindex_lzend(){
    if(trailing_char != NULL) delete [] trailing_char;
    if(phrases != NULL) delete phrases;
    if(sources != NULL) delete sources;
    if(sst != NULL) delete sst;
    if(rev_trie != NULL) delete rev_trie;
    if(sst_skips != NULL) delete [] sst_skips;
    if(rev_trie_skips != NULL) delete [] rev_trie_skips;
    if(rev_ids != NULL)delete [] rev_ids;
    if(sst_ids != NULL)delete [] sst_ids;
    if(range != NULL)delete range;
    if(perm != NULL)delete perm;
    if(wt_depth != NULL)delete wt_depth;
    if(dac_sst_skips != NULL)basics::destroyFT(dac_sst_skips);
    if(dac_rev_skips != NULL)basics::destroyFT(dac_rev_skips);
}
unsigned int static_selfindex_lzend::size(){
    return sizeof(static_selfindex_lzend) + 
    sigma_mapper->size() +
    sizeof(unsigned int)*basics::uint_len(parsing_len,basics::bits(sigma_mapper->sigma()-1)) +
    phrases->size() + 
    (binarySst ? 0 : sst->size() + basics::sizeFT(dac_sst_skips) ) +
    (binaryRev ? 0: rev_trie->size() + basics::sizeFT(dac_rev_skips) ) +
    sizeof(unsigned int)*basics::uint_len(parsing_len-1,rev_ids_bits) + 
    (store_sst_ids? sizeof(unsigned int)*basics::uint_len(parsing_len-1,sst_ids_bits) : 0) +
    range->size() +
    wt_depth->size() + 
    perm->size() + 
    sources->size();
}
/** Saves the index to a file */
unsigned int static_selfindex_lzend::save(FILE *fp){
    return 1;
}
/** Loads the index from a file */
static_selfindex_lzend* static_selfindex_lzend::load(FILE * fp){
    unsigned char hdr;
    if(fread(&hdr,sizeof(unsigned char),1,fp)!=1 || hdr != SELFINDEX_LZEND_HDR){
        std::cerr<<"ERROR: load: wrong header"<<std::endl;
        return NULL;
    }
    static_selfindex_lzend* ret = new static_selfindex_lzend();
    /**************************************************************************/
    /* Load Options                                                           */
    if(fread(&(ret->binaryRev),sizeof(unsigned char),1,fp)!=1){
        std::cerr<<"ERROR: load: option"<<std::endl;
        return NULL;
    }
    if(fread(&(ret->binarySst),sizeof(unsigned char),1,fp)!=1){
        std::cerr<<"ERROR: load: option"<<std::endl;
        return NULL;
    }
    if(fread(&(ret->store_sst_ids),sizeof(unsigned char),1,fp)!=1){
        std::cerr<<"ERROR: load: option"<<std::endl;
        return NULL;
    }
    /**************************************************************************/
    if(fread(&(ret->tlen),sizeof(unsigned int),1,fp) != 1){
        delete ret; return NULL;
    }    
    ret->sigma_mapper = mapper::load(fp);
    if(ret->sigma_mapper == NULL){
        std::cerr<<"ERROR: load: mapper "<<std::endl;
        delete ret; return NULL;
    }
    if(fread(&(ret->parsing_len),sizeof(unsigned int),1,fp) != 1){
        std::cerr<<"ERROR: load: parsing len "<<std::endl;
        delete ret; return NULL;
    }

    ret->trailing_char_bits = basics::bits(ret->sigma_mapper->sigma()-1);
    ret->trailing_char = new unsigned int[basics::uint_len(ret->parsing_len, ret->trailing_char_bits)];
    if(ret->trailing_char == NULL){
        std::cerr<<"ERROR: load: allocating trailing char "<<std::endl;
        delete ret; return NULL;
    }
    if(fread(ret->trailing_char,sizeof(unsigned int),basics::uint_len(ret->parsing_len, ret->trailing_char_bits), fp) != basics::uint_len(ret->parsing_len, ret->trailing_char_bits)){
        std::cerr<<"ERROR: load: trailing char "<<std::endl;
        delete ret; return NULL;
    }
    ret->phrases = DeltaCodes::load(fp);
    if(ret->phrases == NULL){
        std::cerr<<"ERROR: load: deltacodes "<<std::endl;
        delete ret; return NULL;
    }
    if(!ret->binarySst){
    ret->sst = basics::dfuds::load(fp);
    if(ret->sst == NULL){
        std::cerr<<"ERROR: load: sst "<<std::endl;
        delete ret; return NULL;
    }
    ret->dac_sst_skips = basics::loadFT(fp);
    }
    if(!ret->binaryRev){
    ret->rev_trie = basics::dfuds::load(fp);
    if(ret->rev_trie == NULL){
        std::cerr<<"ERROR: load: rev trie "<<std::endl;
        delete ret; return NULL;
    }
    ret->dac_rev_skips = basics::loadFT(fp);
    }
    if(fread(&(ret->rev_ids_bits),sizeof(unsigned char),1,fp)!=1){
        std::cerr<<"ERROR: load: rev ids bits "<<std::endl;
        delete ret; return NULL;
    }
    ret->rev_ids = new unsigned int[basics::uint_len(ret->parsing_len-1,ret->rev_ids_bits)];
    if(ret->rev_ids == NULL){
        std::cerr<<"ERROR: load: allocating rev ids "<<std::endl;
        delete ret; return NULL;
    }
    if(fread(ret->rev_ids,sizeof(unsigned int),basics::uint_len(ret->parsing_len-1,ret->rev_ids_bits),fp) != basics::uint_len(ret->parsing_len-1,ret->rev_ids_bits)){
        std::cerr<<"ERROR: load: rev ids "<<std::endl;
        delete ret; return NULL;
    }
    if(ret->store_sst_ids){
    if(fread(&(ret->sst_ids_bits),sizeof(unsigned char),1,fp)!=1){
        std::cerr<<"ERROR: load: sst ids bits "<<std::endl;
        delete ret; return NULL;
    }
    ret->sst_ids = new unsigned int[basics::uint_len(ret->parsing_len-1,ret->sst_ids_bits)];
    if(ret->sst_ids == NULL){
        std::cerr<<"ERROR: load: allocating sst ids "<<std::endl;
        delete ret; return NULL;
    }
    if(fread(ret->sst_ids,sizeof(unsigned int),basics::uint_len(ret->parsing_len-1,ret->sst_ids_bits),fp) != basics::uint_len(ret->parsing_len-1,ret->sst_ids_bits)){
        std::cerr<<"ERROR: load: sst ids "<<std::endl;
        delete ret; return NULL;
    }
    }
    ret->range = static_range::load(fp);
    if(ret->range == NULL){
        std::cerr<<"ERROR: load: range "<<std::endl;
        delete ret; return NULL;
    }
    ret->wt_depth = static_sequence_wt::load(fp);
    if(ret->wt_depth == NULL){
        std::cerr<<"ERROR: load: depth WT"<<std::endl;
        return NULL; 
    }
    ret->perm = basics::static_permutation::load(fp);
    if(ret->perm == NULL){
        std::cerr<<"ERROR: load: permutation "<<std::endl;
        delete ret; return NULL;
    }
    ret->sources = DeltaCodes::load(fp);
    if(ret->sources == NULL){
        std::cerr<<"ERROR: load: deltacodes sources"<<std::endl;
        delete ret; return NULL;
    }

    return ret;
}

/*default constructor*/
static_selfindex_lzend::static_selfindex_lzend(){
        sigma_mapper = NULL;
        tlen = 0;
        parsing_len=0;
        trailing_char=NULL;
        trailing_char_bits=0;
        phrases=NULL;
        sources = NULL;
        sst=NULL;
        rev_trie=NULL;
        sst_skips=NULL;
        sst_skips_bits=0;
        rev_trie_skips=NULL;
        rev_trie_skips_bits=0;
        dac_sst_skips=NULL;
        dac_rev_skips=NULL;
        rev_ids=NULL;
        rev_ids_bits=0;
        sst_ids=NULL;
        sst_ids_bits=NULL;
        range=0;
        perm = NULL;
        wt_depth = NULL;
        store_sst_ids = 1;
        binaryRev = 0;
        binarySst = 0;
}

bool static_selfindex_lzend::_exist(unsigned char* pattern, unsigned int plen){
    std::vector<unsigned int>* occ_pos = new std::vector<unsigned int>();
    _primaryOcc(pattern,plen,occ_pos,1);
    unsigned int size = occ_pos->size();
    delete occ_pos;
    return size>0;
}

/** Locates the occurrences of pattern in the text. Returns a new allocated array and stores the number of elementes in count. The returned array could be arger than count.*/
/*std::vector<unsigned int>* static_selfindex_lzend::_locate(unsigned char* pattern, unsigned int plen, unsigned int* count){
    *count=0;
    std::vector<unsigned int>* occ_pos = new std::vector<unsigned int>();
    _primaryOcc(pattern,plen,occ_pos,0);
    _secondaryOcc(plen,occ_pos,0);
    *count = occ_pos->size();
    return occ_pos;
}*/

/** Locates the occurrences of pattern in the text. Returns a new allocated array and stores the number of elementes in count. The returned array could be arger than count.*/
std::vector<unsigned int>* static_selfindex_lzend::_locate(unsigned char* pattern, unsigned int plen, unsigned int* count, unsigned int max_occs){
    *count=0;
    std::vector<unsigned int>* occ_pos = new std::vector<unsigned int>();
    _primaryOcc(pattern,plen,occ_pos,0);
    if(max_occs==0 || occ_pos->size()<max_occs){
        _secondaryOcc(plen,occ_pos,max_occs);   
    }
    *count = occ_pos->size();
    return occ_pos;
}

/** Finds primary occurrences*/
void static_selfindex_lzend::_primaryOcc(unsigned char* pattern, unsigned int plen, std::vector<unsigned int>* occ_pos, unsigned int max_occs){
    //special case
    unsigned char* rev_pattern = new unsigned char[plen+1];
    unsigned char* orevpattern = rev_pattern;
    unsigned char* opattern = pattern;
    for(unsigned int i=0;i<plen;i++){
        rev_pattern[i] = pattern[plen-1-i];
    }
    rev_pattern[plen] = '\0';
    //special case: the pattern is completely contained in a phrase
    _computeSpecialOcc(opattern,rev_pattern,plen,occ_pos);
    if(max_occs>0 && occ_pos->size()>=max_occs){
        delete [] orevpattern;
        return;
    }
    rev_pattern += plen-1;
    pattern += 1;
    for(unsigned int i=1;i<plen;i++){
        _computePrimaryOcc(opattern,rev_pattern,i,pattern,plen-i,occ_pos);
        if(max_occs>0 && occ_pos->size()>=max_occs){
            delete [] orevpattern;
            return;
        }
        rev_pattern -= 1;
        pattern += 1;
    }
    delete [] orevpattern;
}

void static_selfindex_lzend::_computeSpecialOcc(unsigned char* pattern, unsigned char* rev_pattern,unsigned int plen, std::vector<unsigned int>* occ_pos){
    unsigned int Ll,Lr;
    if(binaryRev){
        _binarySearchRev(rev_pattern, plen, &Ll, &Lr);
    }else{
        _searchTreeRev(rev_pattern, plen, &Ll, &Lr);
    }
    if(Lr>=Ll){
        unsigned int phrase_len;
        unsigned int phrase_end = phrases->select2(basics::get_field(rev_ids,rev_ids_bits,Ll-1),&phrase_len);
        phrase_end += phrase_len+1;//phrase end is the first character of the next phrase
        if(plen > phrase_len){
            return;
        }
        unsigned int start_pos = phrase_end - plen;
        unsigned char* extract_str = _display(start_pos,start_pos+plen-1);
        if(strcmp((char*)extract_str,(char*)pattern) != 0){//no match so we discard all range occs
            delete[] extract_str;
            return;            
        }else{
            occ_pos->push_back(start_pos);
            delete[] extract_str;         
        }
        for(unsigned int i=Ll;i<Lr;i++){
            occ_pos->push_back(phrases->select(basics::get_field(rev_ids,rev_ids_bits,i)+1)+1-plen);
        }
    }    
}

void static_selfindex_lzend::_computePrimaryOcc(unsigned char* pattern, unsigned char* rev_patternL, unsigned int plenL, unsigned char* patternR, unsigned int plenR, std::vector<unsigned int>* occ_pos){
    unsigned int Rl,Rr,Ll,Lr;

    if(binaryRev){
        _binarySearchRev(rev_patternL, plenL, &Ll, &Lr);
    }else{
        _searchTreeRev(rev_patternL, plenL, &Ll, &Lr);
    }

    if(Lr>=Ll){
        if(binarySst){
            _binarySearchSst(patternR, plenR, &Rl, &Rr);
        }else{
            _searchTreeSst(patternR, plenR, &Rl, &Rr);
        }
        if(Rr>=Rl){
            std::vector<unsigned int>* range_res = this->range->search(Rl,Rr,Ll,Lr);
            if(range_res->size()>0){
                unsigned int phrase_len;
                unsigned int phrase_end = phrases->select2(basics::get_field(rev_ids,rev_ids_bits,(*range_res)[0]-1),&phrase_len);
                phrase_end += phrase_len+1;//phrase end is the first character of the next phrase
                if(plenL > phrase_len){
                    delete range_res;
                    return;
                }

                unsigned int start_pos = phrase_end - plenL;
                unsigned char* extract_str = _display(start_pos,start_pos+plenL+plenR-1);
                if(strcmp((char*)extract_str,(char*)pattern)!=0){//no match so we discard all range occs
                    delete[] extract_str;
                    delete range_res;
                    return;            
                }else{
                    occ_pos->push_back(start_pos);                    
                    delete[] extract_str;         
                }
            }
            for(unsigned int i=1;i<range_res->size();i++){
                unsigned int start_pos = phrases->select(basics::get_field(rev_ids,rev_ids_bits,(*range_res)[i]-1)+1)+1-plenL;
                occ_pos->push_back(start_pos);                    
            }
            delete range_res;
        }
    }
}

void static_selfindex_lzend::_searchTreeSst(unsigned char* pattern, unsigned int plen, unsigned int* L, unsigned int* R){
    unsigned int node = 1;//start from root
    unsigned int pos=0;
    unsigned int s_pos;
    while(pos<plen && !sst->inspect(node)){//sst->isleaf(node)
        node = sst->labeled_child(node,pattern[pos],&s_pos);
        if(node == (unsigned int)-1){//no label
            *L = 1; *R = 0; //set the val to invalid
            return;
        }
        pos += basics::accessFT(dac_sst_skips,s_pos+1);
    }
    *L = sst->leftmost_leaf_rank(node);
    *R = sst->rightmost_leaf_rank(node);
}

void static_selfindex_lzend::_searchTreeRev(unsigned char* pattern, unsigned int plen, unsigned int* L, unsigned int* R){
    unsigned int node = 1;//start from root
    unsigned int pos=0;
    unsigned int s_pos;
    while(pos<plen && !rev_trie->inspect(node)){//rev_trie->isleaf(node)
        node = rev_trie->labeled_child(node,pattern[pos],&s_pos);
        if(node == (unsigned int)-1){//no label
            *L = 1; *R = 0; //set the val to invalid
            return;
        }
        pos += basics::accessFT(dac_rev_skips,s_pos+1);
    }
    *L = rev_trie->leftmost_leaf_rank(node);
    *R = rev_trie->rightmost_leaf_rank(node);
}
/** Finds secondary occurrences*/
void static_selfindex_lzend::_secondaryOcc(unsigned int plen, std::vector<unsigned int>* occ_pos,unsigned int max_occs){
    for(unsigned int i=0;i<occ_pos->size();i++){
        _computeSecondaryOcc((*occ_pos)[i],plen,occ_pos,max_occs);
        if(max_occs>0 && occ_pos->size()>=max_occs){
            return;
        }
    }
}

void static_selfindex_lzend::_computeSecondaryOcc(unsigned int pos, unsigned int plen, std::vector<unsigned int>* occ_pos,unsigned int max_occs){
    unsigned int source_id;
    unsigned int p = sources->select0_rank1(pos+2,&source_id);//pos starts from 0, but we are doing select and also have an additional 0 for sources strting at -1
    source_id -= 1;
    unsigned int source_start = p-source_id-1;//source-id+1=rank_1
    unsigned char v = wt_depth->alphabet_size;
    if(source_start == (unsigned int)-1){
        return;
    }
    while(1){
        unsigned int s_length;
        unsigned int p_pos = phrases->select2(perm->rev_pi(source_id),&s_length);

        while(source_start+s_length-1 >= pos+plen){
            occ_pos->push_back(p_pos+1+pos-source_start);
            if(max_occs>0 && occ_pos->size()>=max_occs){
                return;
            }

            source_id = wt_depth->prevLessSymbol(source_id-1, v);
            if(source_id == (unsigned int)-1){
                v=0;
                break;
            }
            source_start = sources->select(source_id+1)-source_id-1;//rank_0(source_id+1)
            if(source_start == (unsigned int)-1){
                v=0;
                break;
            }
            p_pos = phrases->select2(perm->rev_pi(source_id),&s_length);
        }
        if(v==0)break;
        v = wt_depth->access(source_id)-1;
        source_id = wt_depth->prevLessSymbol(source_id-1, v);
        if(source_id == (unsigned int)-1){
                break;
        }
        source_start = sources->select(source_id+1)-source_id-1;//rank_0(source_id+1)
        if(source_start == (unsigned int)-1){
                break;
        }
    }
}

/** Returns the substring of the text in a new allocated array*/
unsigned char* static_selfindex_lzend::_display(unsigned int start, unsigned int end){
    unsigned char* answer = new unsigned char[end-start+2];//we add a trailing '\0' to the returned string
    answer[end-start+1] = '\0';
    _charextractLZend(answer,start,end,0,end-start);
    return answer;
}

void static_selfindex_lzend::_charextractLZend(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end){
    while(1){
        unsigned int last_phrase = 0;
        unsigned int r = this->phrases->rank_select(end,&last_phrase);
      
        if(last_phrase == end){
            return _charextractLZend2(answer,start,end,offset_start,offset_end);
        }else{
            //std:cerr<<"sel: "<<b<<" "<<r<<std::endl;
            unsigned int source = _source(r);
            if(start>last_phrase){
                start = source + (start - last_phrase) -1;
                end = source + (end - last_phrase) -1;
            }else{
                _charextractLZend(answer, source, source + (end - last_phrase) -1, offset_end - (end-last_phrase-1), offset_end);
                return _charextractLZend2(answer, start, last_phrase, offset_start, offset_end - (end-last_phrase));
            }
        }
    }
}

void static_selfindex_lzend::_charextractLZend2(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end){
    while(1){
        if(end==0){
            answer[offset_end] = basics::get_field(this->trailing_char,this->trailing_char_bits,0)+1;
            return;
        }
        unsigned int last_phrase = 0;
        unsigned int r = this->phrases->rank_select(end-1,&last_phrase);

        answer[offset_end] = basics::get_field(this->trailing_char,this->trailing_char_bits,r)+1;

        if(start<end){
            unsigned int source = _source(r);
            if(start>last_phrase){
                start = source + (start - last_phrase) -1;
                end = source + (end - last_phrase) - 2;
                offset_end = offset_end - 1;
            }else{
                if(last_phrase+1!=end){
                    _charextractLZend2(answer, source, source + (end - last_phrase) - 2, offset_end - (end-last_phrase-1), offset_end - 1);
                }
                offset_end = offset_end - (end-last_phrase);
                end = last_phrase;
            }
        }else{
            return;
        }
    }
}

void static_selfindex_lzend::_charextractLZ77(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end){
    while(1){//start<=end
        unsigned int last_phrase = 0;
        unsigned int r = this->phrases->rank_select(end,&last_phrase);
        if(last_phrase == end){
            answer[offset_end] = basics::get_field(this->trailing_char,this->trailing_char_bits,r-1)+1;//the array doesnt store the 0
            if(start<end){
                end = end - 1;
                offset_end = offset_end - 1;
            }else{
                return;
            }
        }else{
            unsigned int source = _source(r);
            if(start>last_phrase){
                start = source + (start - last_phrase) -1;
                end = source + (end - last_phrase) -1;
            }else{
                _charextractLZ77(answer, source, source + (end - last_phrase) -1, offset_end - (end-last_phrase-1), offset_end);
                answer[offset_end - (end-last_phrase)] = basics::get_field(this->trailing_char,this->trailing_char_bits,r-1)+1;//the array doesnt store the 0
                if(start<last_phrase){
                    offset_end = offset_end - (end-last_phrase) - 1;
                    end = last_phrase - 1;
                }else{
                    return;
                }
            }
        }
    }
}

int static_selfindex_lzend::_source(unsigned int i){
    unsigned int r = perm->pi(i)+1;
    int pos = this->sources->select(r);
    return pos-r;
}
unsigned int static_selfindex_lzend::_source_length(unsigned int i){
    unsigned int length;
    phrases->select2(i,&length);
    return length;
}

//binary search over rev_ids
void static_selfindex_lzend::_binarySearchRev(unsigned char* pattern, unsigned int plen, unsigned int* left, unsigned int* right){
    int lower = 0;
    int upper = (parsing_len-1)-1;
    unsigned char* extracted;
    unsigned int l;
    while(lower<=upper){
        unsigned int m = (lower+upper)/2;
        unsigned int phrase_len;
        unsigned int phrase_end = phrases->select2(basics::get_field(rev_ids,rev_ids_bits,m),&phrase_len);
        phrase_end += phrase_len;
        l = std::min(plen,phrase_len);

        extracted = _display(phrase_end-l+1,phrase_end);
        std::reverse(extracted,extracted+l);
        int cmp = strncmp((char*)pattern, (char*)extracted, l);
        if(cmp > 0 || (cmp==0 && plen>phrase_len)){//pattern > extracted
            lower = m+1;
        }else if(cmp < 0){//pattern < extracted
            upper = m-1;
        }else{
            delete [] extracted;
            break;
        }
        delete [] extracted;
    }
    if(lower<=upper){//found
        int lower_a,upper_a;
        lower_a = lower;
        upper_a = (lower+upper)/2; 
        while(lower_a <= upper_a){
            unsigned int m = (lower_a+upper_a)/2;
            unsigned int phrase_len;
            unsigned int phrase_end = phrases->select2(basics::get_field(rev_ids,rev_ids_bits,m),&phrase_len);
            phrase_end += phrase_len;
            l = std::min(plen,phrase_len);
            extracted = _display(phrase_end-l+1,phrase_end);
            std::reverse(extracted,extracted+l);
            int cmp = strncmp((char*)pattern, (char*)extracted, l);
            if(cmp > 0 || (cmp==0 && plen>phrase_len)){//pattern > extracted
                lower_a = m+1;
            }else{//pattern <= extracted
                upper_a = m-1;
            }
            delete [] extracted;
        }
        *left=upper_a+1;//lower_a
        lower_a = (lower+upper)/2; 
        upper_a = upper;
        while(lower_a <= upper_a){
            unsigned int m = (lower_a+upper_a)/2;
            unsigned int phrase_len;
            unsigned int phrase_end = phrases->select2(basics::get_field(rev_ids,rev_ids_bits,m),&phrase_len);
            phrase_end += phrase_len;
            l = plen;
            extracted = _display(phrase_end-l+1,phrase_end);
            std::reverse(extracted,extracted+l);
            int cmp = strncmp((char*)pattern, (char*)extracted, l);
            if(cmp >= 0){//pattern >= extracted
                lower_a = m+1;
            }else{//pattern < extracted
                upper_a = m-1;
            }
            delete [] extracted;
        }
        *right = lower_a-1;//upper_a
        *right = *right+1;
        *left = *left+1;
    }else{
        *left=1;
        *right=0;
    }    
}

//binary search over ids
void static_selfindex_lzend::_binarySearchSst(unsigned char* pattern, unsigned int plen, unsigned int* left, unsigned int* right){
    int lower = 0;
    int upper = (parsing_len-1)-1;
    unsigned char* extracted;
    unsigned int l;
    while(lower<=upper){
        unsigned int m = (lower+upper)/2;
        unsigned int phrase_len;
        unsigned int p;
        if(store_sst_ids){
            p = basics::get_field(sst_ids,sst_ids_bits,m);
        }else{
            std::vector<unsigned int>* res = range->search(m+1,m+1,1,parsing_len-1);
            p = basics::get_field(rev_ids,rev_ids_bits,(*res)[0]-1);
            delete res;
        }
        unsigned int phrase_start = phrases->select2(p+1,&phrase_len)+1;

        extracted = _display(phrase_start,std::min(phrase_start+plen-1,tlen-2));
        int cmp = strncmp((char*)pattern, (char*)extracted, plen);
        if(cmp > 0 ){//pattern > extracted
            lower = m+1;
        }else if(cmp < 0){//pattern < extracted
            upper = m-1;
        }else{
            delete [] extracted;
            break;
        }
        delete [] extracted;
    }
    if(lower<=upper){//found
        int lower_a,upper_a;
        lower_a = lower;
        upper_a = (lower+upper)/2; 
        while(lower_a <= upper_a){
            unsigned int m = (lower_a+upper_a)/2;
            unsigned int phrase_len;
            unsigned int p;
            if(store_sst_ids){
                p = basics::get_field(sst_ids,sst_ids_bits,m);
            }else{
                std::vector<unsigned int>* res = range->search(m+1,m+1,1,parsing_len-1);
                p = basics::get_field(rev_ids,rev_ids_bits,(*res)[0]-1);
                delete res;
            }
            unsigned int phrase_start = phrases->select2(p+1,&phrase_len)+1;
            l = std::min(plen,phrase_len);
            extracted = _display(phrase_start,std::min(phrase_start+plen-1,tlen-2));
            int cmp = strncmp((char*)pattern, (char*)extracted, plen);
            if(cmp > 0){//pattern > extracted
                lower_a = m+1;
            }else{//pattern <= extracted
                upper_a = m-1;
            }
            delete [] extracted;
        }
        *left=upper_a+1;//lower_a
        lower_a = (lower+upper)/2; 
        upper_a = upper;
        while(lower_a <= upper_a){
            unsigned int m = (lower_a+upper_a)/2;
            unsigned int phrase_len;
                    unsigned int p;
            if(store_sst_ids){
                p = basics::get_field(sst_ids,sst_ids_bits,m);
            }else{
                std::vector<unsigned int>* res = range->search(m+1,m+1,1,parsing_len-1);
                p = basics::get_field(rev_ids,rev_ids_bits,(*res)[0]-1);
                delete res;
            }
            unsigned int phrase_start = phrases->select2(p+1,&phrase_len)+1;
            extracted = _display(phrase_start,std::min(phrase_start+plen-1,tlen-2));
            int cmp = strncmp((char*)pattern, (char*)extracted, plen);
            if(cmp >= 0){//pattern >= extracted
                lower_a = m+1;
            }else{//pattern < extracted
                upper_a = m-1;
            }
            delete [] extracted;
        }
        *right = lower_a-1;//upper_a
        *right = *right+1;
        *left = *left+1;
    }else{
        *left=1;
        *right=0;
    }    
}



}


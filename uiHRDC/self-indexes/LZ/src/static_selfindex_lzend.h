/* static_selfindex_lzend.h
 * Copyright (C) 2009, Sebastian Kreft C., all rights reserved.
 *
 * static_selfindex_lzend definition
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef __STATIC_SELFINDEX_LZEND_H__
#define __STATIC_SELFINDEX_LZEND_H__

#include <vector>
#include "static_selfindex.h"
#include "deltacodes.h"
#include "dfuds.h"
#include "static_range.h"
#include "static_permutation.h"
#include "static_sequence_wt.h"
#include "directcodes.h"

namespace lz77index{
/** "Selfindex" that uses BMH to find occurrences.
 *  It's not really a selfindex, it is used just for testing.
 *
 *  @author Sebastian Kreft C.
 */
class static_selfindex_lzend : public static_selfindex {
    public:
        ~static_selfindex_lzend();
        unsigned int size();
        /** Saves the index to a file */
        unsigned int save(FILE *fp);
        /** Loads the index from a file */
        static static_selfindex_lzend* load(FILE * fp);
        static static_selfindex_lzend* build(char* filename, char* filenameidx, unsigned char binaryRev, unsigned char binarySst, unsigned char store_sst_ids);
    protected:
        /** Locates the occurrences of pattern in the text. Returns a new allocated array and stores the number of elementes in count. The returned array could be arger than count.*/
        //std::vector<unsigned int>* _locate(unsigned char* pattern,unsigned int plen,unsigned int* count);
        std::vector<unsigned int>* _locate(unsigned char* pattern,unsigned int plen,unsigned int* count, unsigned int max_occs);
        /** Return the number of occurences of pattern in the text. */
        //virtual unsigned int count(unsigned char* pattern);
        /** Returns true if the pattern exists */
        bool _exist(unsigned char* pattern,unsigned int plen);
        /** Returns the substring of the text in a new allocated array*/
        unsigned char* _display(unsigned int start, unsigned int end);
        /** Returns the size of the index */
        static_selfindex_lzend();
        /** Finds primary occurrences*/
        void _primaryOcc(unsigned char* pattern, unsigned int plen, std::vector<unsigned int>* occ_pos, unsigned int max_occs);
        void _computePrimaryOcc(unsigned char* pattern, unsigned char* rev_patternL, unsigned int plenL, unsigned char* patternR, unsigned int plenR, std::vector<unsigned int>* occ_pos);
        void _computeSpecialOcc(unsigned char* pattern, unsigned char* rev_pattern,unsigned int plen, std::vector<unsigned int>* occ_pos);
        void _searchTreeSst(unsigned char* pattern, unsigned int plen, unsigned int* L, unsigned int* R);
        void _searchTreeRev(unsigned char* pattern, unsigned int plen, unsigned int* L, unsigned int* R);
        void _binarySearchSst(unsigned char* pattern, unsigned int plen, unsigned int* L, unsigned int* R);
        void _binarySearchRev(unsigned char* pattern, unsigned int plen, unsigned int* L, unsigned int* R);
        /** Finds secondary occurrences*/
        void _secondaryOcc(unsigned int plen, std::vector<unsigned int>* occ_pos, unsigned int max_occs);
        void _computeSecondaryOcc(unsigned int pos, unsigned int plen, std::vector<unsigned int>* occ_pos, unsigned int max_occs);
        /** Extracts a substring from a LZ77 parse*/        
        void _charextractLZ77(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end);
        void _charextractLZend(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end);
        void _charextractLZend2(unsigned char* answer, unsigned start, unsigned end, unsigned int offset_start, unsigned int offset_end);
        /** returns the start position of the source of phrase i, i starting from 0 */
        int _source(unsigned int i);        
        /** returns the length of a source*/
        unsigned int _source_length(unsigned int i);
        /** instance variables*/
        unsigned int parsing_len;
        unsigned int* trailing_char;
        unsigned char trailing_char_bits;
        DeltaCodes* phrases;
        DeltaCodes* sources;
        basics::dfuds* sst;
        basics::dfuds* rev_trie;
        unsigned int* sst_skips;
        unsigned char sst_skips_bits;
        unsigned int* rev_trie_skips;
        unsigned char rev_trie_skips_bits;
        unsigned int* rev_ids;
        unsigned char rev_ids_bits;
        unsigned int* sst_ids;
        unsigned char sst_ids_bits;
        basics::FTRep* dac_sst_skips;
        basics::FTRep* dac_rev_skips;
        static_range* range;
        basics::static_permutation* perm;
        static_sequence_wt* wt_depth;
        //configuration variables
        unsigned char binaryRev;
        unsigned char binarySst;
        unsigned char store_sst_ids;
};

}
#endif /* _STATIC_SELFINDEX_LZ77_H */

/* BitSequenceRG.h
   Copyright (C) 2005, Rodrigo Gonzalez,  Francisco Claude(2008), Roberto Konow (2014) all rights reserved.

   New RANK, SELECT, SELECT-NEXT and SPARSE RANK implementations.
   Adaptation to libcds by Francisco Claude

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#ifndef _STATIC_BITSEQUENCE_RGK_H
#define _STATIC_BITSEQUENCE_RGK_H

#include <libcdsBasics.h>
#include <BitString.h>
#include <BitSequence.h>

using namespace cds_utils;

namespace cds_static {

/////////////
//Rank(B,i)//
/////////////
//_factor = 0  => s=W*lgn
//_factor = P  => s=W*P
//Is interesting to notice
//factor=2 => overhead 50%
//factor=3 => overhead 33%
//factor=4 => overhead 25%
//factor=20=> overhead 5%

/** Implementation of Rodrigo Gonzalez et al. practical rank/select solution [1].
*  The interface was adapted.
*
*  [1] Rodrigo Gonzalez, Szymon Grabowski, Veli Makinen, and Gonzalo Navarro.
*      Practical Implementation of Rank and Select Queries. WEA05.
*
*  @author Rodrigo Gonzalez
*/
class BitSequenceRGK : public BitSequence {
private:
    //bool owner;
    size_t n, integers;
    size_t factor, b, s, select1sampling, select0sampling;
    uint *Rs;             //superblock array
    uint *Stones;            // Select Table
    uint *Stzero;            // Select Table
    bool fastselect1, fastselect0;


    //uso interno para contruir el indice rank
    size_t BuildRankSub(size_t ini, size_t fin);

    void BuildRank();     //crea indice para rank
    BitSequenceRGK();

    size_t SpaceRequirementInBits() const;

    size_t SpaceRequirement() const;

public:
    uint *data;

    /** Build the BitSequenceRG with a sampling factor <code>factor</code>
    * The <code>factor</code> value has to be either 2,3,4 or 20, being the first one the fastest/bigger.
    */
    BitSequenceRGK(uint *bitarray, size_t n, uint factor, uint selectsampling = 256);

    /** Build the BitSequenceRG with a sampling factor <code>factor</code>
    * The <code>factor</code> value has to be either 2,3,4 or 20, being the first one the fastest/bigger.
    */
    BitSequenceRGK(const BitString &bs, uint factor, uint selectsampling = 256);

    ~BitSequenceRGK();     //destructor

    virtual bool access(const size_t i) const;

    //Nivel 1 bin, nivel 2 sec-pop y nivel 3 sec-bit
    virtual size_t rank1(const size_t i) const;

    size_t select1_t(const size_t x) const;

    size_t select1_s(const size_t x) const;

    size_t select0_t(const size_t x) const;

    size_t select0_s(const size_t x) const;

    // gives the largest index i<=start such that IsBitSet(i)=true
    virtual size_t selectPrev1(const size_t start) const;

    // gives the smallest index i>=start such that IsBitSet(i)=true
    virtual size_t selectNext1(const size_t start) const;

    // gives the position of the x:th 1.
    virtual size_t select0(size_t x) const;

    // gives the position of the x:th 1.
    virtual size_t select1(size_t x) const;

    virtual size_t getSize() const;

    /*load-save functions*/
    virtual void save(ofstream &f) const;

    static BitSequenceRGK *load(ifstream &f);

    class Builder {
    public:
        bool fastselect1_;
        bool fastselect0_;
        uint factor_;
        uint sonesamp_;
        uint szerosamp_;
        BitString *bs_;

        Builder() {
            fastselect1_ = false;
            fastselect0_ = false;
            factor_ = 20;
            sonesamp_ = 256;
            szerosamp_ = 256;
            bs_ = NULL;
        }

        Builder fastselect1() {
            fastselect1_ = true;
            // cout << "fastselect1_ = " << fastselect1_ << endl;
            return *this;
        }

        Builder fastselect0() {
            fastselect0_ = true;
            return *this;
        }

        Builder s1factor(int val) {
            fastselect1_ = true;
            // cout << "fastselect1_ = " << fastselect1_ << endl;
            sonesamp_ = val;
            return *this;
        }

        Builder s0factor(int val) {
            fastselect0_ = true;
            // cout << "fastselect1_ = " << fastselect1_ << endl;

            szerosamp_ = val;
            return *this;
        }

        Builder bitstring(BitString &bs) {
            // cout << "fastselect1_ = " << fastselect1_ << endl;
            bs_ = &bs;
            return *this;
        }

        Builder rfactor(int val) {
            factor_ = val;
            return *this;
        }

        BitSequenceRGK *build() {
            // cout << "fastselect1_ = " << fastselect1_ << endl;
            return new BitSequenceRGK(*this);
        }

    };

    static Builder builder;

    BitSequenceRGK(Builder b);
};
}
#endif

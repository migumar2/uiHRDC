/* BitSequenceRGK.cpp
   Copyright (C) 2005, Rodrigo Gonzalez, Francisco Claude, Roberto Konow 
   all rights reserved.

   New RANK, SELECT, SELECT-NEXT and SPARSE RANK implementations.
   Addaptation to libcds by Francisco Claude

   NEW SELECT_T Operation, 2 times faster select by Roberto Konow

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
   Foundation, Inc., 51 Franklin Stones, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include <BitSequenceRGK.h>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <map>

// #include <sys/types.h>
cds_static::BitSequenceRGK::Builder cds_static::BitSequenceRGK::builder;

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


BitSequenceRGK::BitSequenceRGK() {
    data = NULL;
    //  this->owner = true;
    this->n = 0;
    this->factor = 0;
}

BitSequenceRGK::BitSequenceRGK(Builder builder) {
    BitString &bs = *builder.bs_;
    uint _factor = builder.factor_;
    select1sampling = builder.sonesamp_;
    select0sampling = builder.szerosamp_;
    fastselect0 = builder.fastselect0_;
    fastselect1 = builder.fastselect1_;

    const uint *bitarray = bs.getData();
    size_t _n = bs.getLength();
    if (_factor == 0) exit(-1);
    data = new uint[_n / W + 1];
    for (size_t i = 0; i < uint_len(_n, 1); i++)
        data[i] = bitarray[i];
    for (size_t i = uint_len(_n, 1); i < _n / W + 1; i++)
        data[i] = 0;
    //this->owner = true;
    this->n = _n;
    uint lgn = bits(n - 1);
    this->factor = _factor;
    if (_factor == 0) this->factor = lgn;
    else this->factor = _factor;
    b = 32;
    s = b * this->factor;
    integers = n / W + 1;
    BuildRank();
    this->length = n;
    this->ones = rank1(n - 1);
    if (fastselect1) {
        // cout << "entered here" << endl;
        this->Stones = new uint[ones / select1sampling + 1];
        Stones[0] = 0;
        for (size_t i = 0; i <= ones; i += select1sampling) {
            if (i == 0) continue;
            Stones[i / select1sampling] = this->select1_s(i);
        }
    }
    if (fastselect0) {
        this->Stzero = new uint[(n - ones) / select0sampling + 1];
        Stzero[0] = 0;
        for (size_t i = 0; i <= (n - ones); i += select0sampling) {
            if (i == 0) continue;
            Stzero[i / select0sampling] = this->select0_s((i));
        }
    }
}


BitSequenceRGK::BitSequenceRGK(const BitString &bs, uint _factor, uint selectsampling) {
    const uint *bitarray = bs.getData();
    size_t _n = bs.getLength();
    // cout << _factor << endl;
    if (_factor == 0) exit(-1);
    // cout << "_n/W+1=" << _n/W+1 << endl;
    data = new uint[_n / W + 1];
    // cout << "_n/W+1=" << _n/W+1 << endl;
    for (size_t i = 0; i < uint_len(_n, 1); i++)
        data[i] = bitarray[i];
    for (size_t i = uint_len(_n, 1); i < _n / W + 1; i++)
        data[i] = 0;
    //this->owner = true;
    this->n = _n;
    uint lgn = bits(n - 1);
    this->factor = _factor;
    if (_factor == 0) this->factor = lgn;
    else this->factor = _factor;
    b = 32;
    s = b * this->factor;
    integers = n / W + 1;
    BuildRank();
    this->length = n;
    this->ones = rank1(n - 1);
    fastselect0 = true;
    fastselect1 = true;
    select1sampling = selectsampling;
    select0sampling = selectsampling;
    if (fastselect1) {
        // cout << "entered here" << endl;
        this->Stones = new uint[ones / select1sampling + 1];
        Stones[0] = 0;
        for (size_t i = 0; i <= ones; i += select1sampling) {
            if (i == 0) continue;
            Stones[i / select1sampling] = this->select1_s(i);
        }
    }
    if (fastselect0) {
        this->Stzero = new uint[(n - ones) / select0sampling + 1];
        Stzero[0] = 0;
        for (size_t i = 0; i <= (n - ones); i += select0sampling) {
            if (i == 0) continue;
            Stzero[i / select0sampling] = this->select0_s((i));
        }
    }
}


BitSequenceRGK::BitSequenceRGK(uint *bitarray, size_t _n, uint _factor, uint selectsampling) {
    if (_factor == 0) exit(-1);
    data = new uint[_n / W + 1];
    for (size_t i = 0; i < uint_len(_n, 1); i++)
        data[i] = bitarray[i];
    for (size_t i = uint_len(_n, 1); i < _n / W + 1; i++)
        data[i] = 0;
    //this->owner = true;
    this->n = _n;
    uint lgn = bits(n - 1);
    this->factor = _factor;
    if (_factor == 0) this->factor = lgn;
    else this->factor = _factor;
    b = 32;
    s = b * this->factor;
    integers = n / W + 1;
    BuildRank();
    this->length = n;
    this->ones = rank1(n - 1);
    fastselect0 = true;
    fastselect1 = true;
    select1sampling = selectsampling;
    select0sampling = selectsampling;
    if (fastselect1) {
        // cout << "entered here" << endl;
        this->Stones = new uint[ones / select1sampling + 1];
        Stones[0] = 0;
        for (size_t i = 0; i <= ones; i += select1sampling) {
            if (i == 0) continue;
            Stones[i / select1sampling] = this->select1_s(i);
        }
    }
    if (fastselect0) {
        this->Stzero = new uint[(n - ones) / select0sampling + 1];
        Stzero[0] = 0;
        for (size_t i = 0; i <= (n - ones); i += select0sampling) {
            if (i == 0) continue;
            Stzero[i / select0sampling] = this->select0_s((i));
        }
    }
}

BitSequenceRGK::~BitSequenceRGK() {
    delete[] Rs;
    delete[] data;
    if (fastselect1)
        delete[] Stones;
    if (fastselect0)
        delete[] Stzero;
}

size_t BitSequenceRGK::select1_t(size_t x) const {
    if (x > ones) return (uint) (-1);
    uint pos = (x / select1sampling);
    uint bits_restantes = 0;
    uint r = Stones[pos] / s;
    uint ini = 0;
    if (s < select1sampling) {
        size_t incr = 1;
        while (Rs[r + incr] < x) {
            incr = incr * 2;
            if (r + incr > n / s)
                break;
        }
        r = r + incr / 2 - 1;
        while (Rs[r + 1] < x) {
            r = r + 1;
            if (r >= n / s) {
                r = r - 1;
                break;
            }
        }
    }
    if (pos != 0) {
        if (Stones[pos] >= r * s) {
            ini = Stones[pos] + 1;
            bits_restantes = x - pos * select1sampling;
        } else {
            // cout << "here2!" << endl;
            ini = r * factor * W;
            bits_restantes = x - Rs[r];
        }
    } else {
        bits_restantes = x;
        ini = 0;
    }
    uint posini = ini / 32;
    uint dataj = data[posini];
    dataj = dataj >> ini % 32;
    uint ones = __builtin_popcount(dataj);
    while (ones < bits_restantes) {
        bits_restantes -= ones;
        posini++;
        if (posini > integers) return n;
        dataj = data[posini];
        ones = __builtin_popcount(dataj);
        ini = posini * 32;
    }
    uint rankmid = __builtin_popcount(dataj & 0xff);
    if (rankmid < bits_restantes) {
        dataj = dataj >> 8;
        bits_restantes -= rankmid;
        ini += 8;
        rankmid = __builtin_popcount(dataj & 0xff);
        if (rankmid < bits_restantes) {
            dataj = dataj >> 8;
            bits_restantes -= rankmid;
            ini += 8;
            rankmid = __builtin_popcount(dataj & 0xff);
            if (rankmid < bits_restantes) {
                dataj = dataj >> 8;
                bits_restantes -= rankmid;
                ini += 8;
            }
        }
    }
    while (bits_restantes > 0) {
        if (dataj & 1) bits_restantes--;
        dataj = dataj >> 1;
        ini++;
    }
    return ini - 1;
}


size_t BitSequenceRGK::select0_t(size_t x) const {

    if (x > n - ones) return (uint) (-1);
    uint pos = (x / select0sampling);
    uint bits_restantes = 0;
    uint ini = 0;
    uint r = Stzero[pos] / s;
    if (s < select0sampling) {
        size_t incr = 1;
        while (((r + incr) * s - Rs[r + incr]) < x) {
            incr = incr * 2;
            if (r + incr > n / s)
                break;
        }
        r = r + incr / 2 - 1;
        // cout << "r = \t" << r << endl;
        while ((((r + 1) * s) - Rs[r + 1]) < x) {
            r = r + 1;
            if (r >= n / s) {
                r = r - 1;
                break;
            }
        }
    }
    if (pos != 0) {
        if (Stzero[pos] >= r * s) {
            ini = Stzero[pos] + 1;
            bits_restantes = x - pos * select0sampling;
        } else {
            ini = r * factor * W;
            bits_restantes = x - (r * s - Rs[r]);
        }
    } else {
        ini = 0;
        bits_restantes = x;
    }

    uint posini = ini / 32;
    uint dataj = data[posini];
    uint mod = ini % 32;
    dataj = dataj >> mod;
    uint zeros = 32 - __builtin_popcount(dataj) - mod;
    // cout << "posini \t  = " << posini << endl;

    while (zeros < bits_restantes) {
        bits_restantes -= zeros;
        posini++;
        if (posini > integers) return n;
        dataj = data[posini];
        zeros = W - __builtin_popcount(dataj);
        ini = posini * 32;
    }

    uint rankmid = 8 - popcount8(dataj);
    if (rankmid < bits_restantes) {
        dataj = dataj >> 8;
        bits_restantes -= rankmid;
        ini += 8;
        rankmid = 8 - popcount8(dataj);
        if (rankmid < bits_restantes) {
            // cout << "here3" << endl;
            dataj = dataj >> 8;
            bits_restantes -= rankmid;
            ini += 8;
            rankmid = 8 - popcount8(dataj);
            if (rankmid < bits_restantes) {
                // cout << "here4" << endl;
                dataj = dataj >> 8;
                bits_restantes -= rankmid;
                ini += 8;
            }
        }
    }
    while (bits_restantes > 0) {
        if (dataj % 2 == 0) bits_restantes--;
        dataj = dataj >> 1;
        ini++;
    }
    ini--;
    if (ini > n + 1) return n;
    else return ini;
}

void BitSequenceRGK::BuildRank() {
    size_t num_sblock = n / s;
    // +1 pues sumo la pos cero
    Rs = new uint[num_sblock + 5];
    for (uint i = 0; i < num_sblock + 5; i++)
        Rs[i] = 0;
    size_t j;
    Rs[0] = 0;
    for (j = 1; j <= num_sblock; j++) {
        Rs[j] = Rs[j - 1];
        Rs[j] += BuildRankSub((j - 1) * factor, factor);
    }
}

size_t BitSequenceRGK::BuildRankSub(size_t ini, size_t bloques) {
    uint rank = 0, aux;
    for (uint i = ini; i < ini + bloques; i++) {
        if (i < integers) {
            aux = data[i];
            rank += popcount(aux);
        }
    }
    return rank;             //retorna el numero de 1's del intervalo

}

size_t BitSequenceRGK::rank1(const size_t i1) const {
    uint i = (uint) i1;
    ++i;
    uint resp = Rs[i / s];
    uint aux = (i / s) * factor;
    for (uint a = aux; a < i / W; a++)
        resp += popcount(data[a]);
    resp += popcount(data[i / W] & ((1 << (i & mask31)) - 1));
    return resp;
}

bool BitSequenceRGK::access(const size_t i) const {
    return (1u << (i % W)) & data[i / W];
}

void BitSequenceRGK::save(ofstream &f) const {
    uint wr = BRW32_RGK;
    saveValue(f, wr);
    saveValue(f, n);
    saveValue(f, factor);
    saveValue(f, data, integers);
    saveValue(f, Rs, n / s + 1);


    saveValue(f, static_cast<uint>(fastselect0));
    saveValue(f, static_cast<uint>(fastselect1));

    if (fastselect0) {
        saveValue(f, select0sampling);
        saveValue(f, Stzero, (n - ones) / select0sampling + 1);
    }
    if (fastselect1) {
        saveValue(f, select1sampling);
        saveValue(f, Stones, ones / select1sampling + 1);
    }

}

BitSequenceRGK *BitSequenceRGK::load(ifstream &f) {
    assert(f.good());
    uint type = loadValue<uint>(f);
    if (type != BRW32_RGK) {     // throw exception
        abort();
    }
    BitSequenceRGK *ret = new BitSequenceRGK();
    ret->n = loadValue<size_t>(f);
    ret->b = 32;
    ret->factor = loadValue<size_t>(f);
    ret->s = ret->b * ret->factor;
    ret->integers = (ret->n + 1) / W + ((ret->n + 1) % W != 0 ? 1 : 0);
    ret->data = loadValue<uint>(f, ret->integers);
    ret->Rs = loadValue<uint>(f, ret->n / ret->s + 1);
    ret->length = ret->n;
    ret->ones = ret->rank1(ret->n - 1);
    ret->fastselect0 = static_cast<bool>(loadValue<uint>(f));
    ret->fastselect1 = static_cast<bool>(loadValue<uint>(f));
    if (ret->fastselect0) {
        ret->select0sampling = loadValue<uint>(f);
        ret->Stzero = loadValue<uint>(f, (ret->n - ret->ones) / ret->select0sampling + 1);
    }
    if (ret->fastselect1) {
        ret->select1sampling = loadValue<uint>(f);
        ret->Stones = loadValue<uint>(f, (ret->ones) / ret->select1sampling + 1);
    }
    return ret;
}

size_t BitSequenceRGK::SpaceRequirementInBits() const {
    return getSize() * 8; // note that his might be wrong
}

size_t BitSequenceRGK::getSize() const {
    size_t total = n / 8;    // raw bits
    total += n / s * 4;    // rank table
    total += sizeof(this);    // rest
    if (fastselect0) {                // select0 table
        total += ((n - ones) / select0sampling) * 4;
    }
    if (fastselect1) {                // select1 table
        total += (ones / select1sampling) * 4;
    }
    return total;
    // return /*sizeof(BitSequenceRGK)+*/SpaceRequirementInBits()/8;
}

size_t BitSequenceRGK::SpaceRequirement() const {
    return getSize();
}

size_t BitSequenceRGK::selectPrev1(const size_t start) const {
    // returns the position of the previous 1 bit before and including start.
    // tuned to 32 bit machine

    uint i = start >> 5;
    int offset = (start % W);
    uint aux2 = data[i] & (-1u >> (31 - offset));

    if (aux2 > 0) {
        if ((aux2 & 0xFF000000) > 0) return i * W + 23 + prev_tab[(aux2 >> 24) & 0xFF];
        else if ((aux2 & 0xFF0000) > 0) return i * W + 15 + prev_tab[(aux2 >> 16) & 0xFF];
        else if ((aux2 & 0xFF00) > 0) return i * W + 7 + prev_tab[(aux2 >> 8) & 0xFF];
        else return i * W + prev_tab[aux2 & 0xFF] - 1;
    }
    for (uint k = i - 1; ; k--) {
        aux2 = data[k];
        if (aux2 > 0) {
            if ((aux2 & 0xFF000000) > 0) return k * W + 23 + prev_tab[(aux2 >> 24) & 0xFF];
            else if ((aux2 & 0xFF0000) > 0) return k * W + 15 + prev_tab[(aux2 >> 16) & 0xFF];
            else if ((aux2 & 0xFF00) > 0) return k * W + 7 + prev_tab[(aux2 >> 8) & 0xFF];
            else return k * W + prev_tab[aux2 & 0xFF] - 1;
        }
    }
    return 0;
}

size_t BitSequenceRGK::selectNext1(const size_t k1) const {
    uint k = (uint) k1;
    uint count = k;
    uint des, aux2;
    des = count % W;
    aux2 = data[count / W] >> des;
    if (aux2 > 0) {
        if ((aux2 & 0xff) > 0) return count + select_tab[aux2 & 0xff] - 1;
        else if ((aux2 & 0xff00) > 0) return count + 8 + select_tab[(aux2 >> 8) & 0xff] - 1;
        else if ((aux2 & 0xff0000) > 0) return count + 16 + select_tab[(aux2 >> 16) & 0xff] - 1;
        else {return count + 24 + select_tab[(aux2 >> 24) & 0xff] - 1;}
    }

    for (uint i = count / W + 1; i < integers; i++) {
        aux2 = data[i];
        if (aux2 > 0) {
            if ((aux2 & 0xff) > 0) return i * W + select_tab[aux2 & 0xff] - 1;
            else if ((aux2 & 0xff00) > 0) return i * W + 8 + select_tab[(aux2 >> 8) & 0xff] - 1;
            else if ((aux2 & 0xff0000) > 0) return i * W + 16 + select_tab[(aux2 >> 16) & 0xff] - 1;
            else {return i * W + 24 + select_tab[(aux2 >> 24) & 0xff] - 1;}
        }
    }
    return n;
}

size_t BitSequenceRGK::select1(const size_t x1) const {
    if (this->fastselect1) {
        return this->select1_t(x1);
    } else {
        return this->select1_s(x1);
    }
}

size_t BitSequenceRGK::select0(const size_t x1) const {
    if (this->fastselect0) {
        return this->select0_t(x1);
    } else {
        return this->select0_s(x1);
    }
}

size_t BitSequenceRGK::select1_s(const size_t x1) const {
    uint x = x1;
    // returns i such that x=rank(i) && rank(i-1)<x or n if that i not exist
    // first binary search over first level rank structure
    // then sequential search using popcount over a int
    // then sequential search using popcount over a char
    // then sequential search bit a bit
    if (x > ones) return (uint) (-1);

    //binary search over first level rank structure
    uint l = 0, r = n / s;
    uint mid = (l + r) / 2;
    uint rankmid = Rs[mid];
    while (l <= r) {
        if (rankmid < x)
            l = mid + 1;
        else
            r = mid - 1;
        mid = (l + r) / 2;
        rankmid = Rs[mid];
    }
    //sequential search using popcount over a int
    uint left;
    left = mid * factor;
    x -= rankmid;
    uint j = data[left];
    uint ones = popcount(j);
    while (ones < x) {
        x -= ones;
        left++;
        if (left > integers) return n;
        j = data[left];
        ones = popcount(j);
    }
    //sequential search using popcount over a char
    left = left * b;
    rankmid = popcount8(j);
    if (rankmid < x) {
        j = j >> 8;
        x -= rankmid;
        left += 8;
        rankmid = popcount8(j);
        if (rankmid < x) {
            j = j >> 8;
            x -= rankmid;
            left += 8;
            rankmid = popcount8(j);
            if (rankmid < x) {
                j = j >> 8;
                x -= rankmid;
                left += 8;
            }
        }
    }

    // then sequential search bit a bit
    while (x > 0) {
        if (j & 1) x--;
        j = j >> 1;
        left++;
    }
    return left - 1;
}

size_t BitSequenceRGK::select0_s(const size_t x1) const {
    uint x = (uint) x1;
    // returns i such that x=rank_0(i) && rank_0(i-1)<x or n if that i not exist
    // first binary search over first level rank structure
    // then sequential search using popcount over a int
    // then sequential search using popcount over a char
    // then sequential search bit a bit
    if (x > n - ones) return (uint) (-1);

    //binary search over first level rank structure
    if (x == 0) return 0;
    uint l = 0, r = n / s;
    uint mid = (l + r) / 2;
    uint rankmid = mid * factor * W - Rs[mid];
    while (l <= r) {
        if (rankmid < x)
            l = mid + 1;
        else
            r = mid - 1;
        mid = (l + r) / 2;
        rankmid = mid * factor * W - Rs[mid];
    }
    //sequential search using popcount over a int
    uint left;
    left = mid * factor;
    x -= rankmid;
    uint j = data[left];
    uint zeros = W - popcount(j);
    while (zeros < x) {
        x -= zeros;
        left++;
        if (left > integers) return n;
        j = data[left];
        zeros = W - popcount(j);
    }
    //sequential search using popcount over a char
    left = left * b;
    rankmid = 8 - popcount8(j);
    if (rankmid < x) {
        j = j >> 8;
        x -= rankmid;
        left += 8;
        rankmid = 8 - popcount8(j);
        if (rankmid < x) {
            j = j >> 8;
            x -= rankmid;
            left += 8;
            rankmid = 8 - popcount8(j);
            if (rankmid < x) {
                j = j >> 8;
                x -= rankmid;
                left += 8;
            }
        }
    }

    // then sequential search bit a bit
    while (x > 0) {
        if (j % 2 == 0) x--;
        j = j >> 1;
        left++;
    }
    left--;
    if (left > n) return n;
    else return left;
}
};

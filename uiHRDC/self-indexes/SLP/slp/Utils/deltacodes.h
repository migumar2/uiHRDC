/* deltacodes.h
 *
 * This class implements a rank/select bitmap which performs differential
 * encoding of (sparse)1-bits.
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

#ifndef DELTACODES_H_
#define DELTACODES_H_

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

	void save(ofstream & output);
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

#endif /*DELTACODES_H_*/

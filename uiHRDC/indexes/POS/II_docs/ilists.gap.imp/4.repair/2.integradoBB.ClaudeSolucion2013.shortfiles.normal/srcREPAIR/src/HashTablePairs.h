
#ifndef HASHTABLEPAIRS_H
#define HASHTABLEPAIRS_H

#include "basic.h"

#define maxAlpha 0.6

class HashTablePairs {
	public:
		HashTablePairs(int * buff, uint buffLen);
		~HashTablePairs();
		uint get(uint p1, uint p2);
		bool insert(uint p1, uint p2, uint value);
		uint h(uint p1, uint p2);
		uint s(uint p1, uint p2);

		uint buffLen;
		int *buff;
		long long queries;
		long long inserted;
		long long collisions;
};

#endif


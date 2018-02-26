
#ifndef HEAP_H
#define HEAP_H

#include "basic.h"
#include <cassert>
#include <utility>

using namespace std;

class Heap {
	public:
		Heap(ulong k);
		~Heap();
		void insert(pair<ulong,ulong>* pos);
		
		pair<ulong,ulong> ** elements;
		ulong inserted, k;
};

#endif


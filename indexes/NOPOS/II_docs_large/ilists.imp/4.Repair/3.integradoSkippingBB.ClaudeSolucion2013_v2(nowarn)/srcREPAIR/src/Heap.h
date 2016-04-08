
#ifndef HEAP_H
#define HEAP_H

#include "basic.h"
#include <cassert>
#include <utility>

using namespace std;

class Heap {
	public:
		Heap(uint k);
		~Heap();
		void insert(pair<uint,uint>* pos);
		
		pair<uint,uint> ** elements;
		uint inserted, k;
};

#endif


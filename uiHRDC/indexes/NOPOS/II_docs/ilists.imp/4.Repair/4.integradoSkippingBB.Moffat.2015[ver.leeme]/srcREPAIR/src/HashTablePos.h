
#ifndef HASHTABLEPOS_H
#define HASHTABLEPOS_H

#include "basic.h"

#define maxAlpha 0.6
#define VERBOSE

class HashTablePos {
	public:
		HashTablePos(uint * data, uint dataLen, int * buff, uint buffLen);
		~HashTablePos();
		bool increment(uint pos);
		uint get(uint pos);
		bool insert(uint pos, uint value);
		uint h(uint pos);   //** funcion hash --> devuelve pos dentro de la hash table ( xx % buffLen )
		uint s(uint pos);
		bool cmp(uint pos1, uint pos2);

		uint dataLen, buffLen;
		uint *data; //** toda la memoria reservada, incluido el texto.
		int *buff;  //** buffer que puede ser usado por la hash table (parte final de *data)
		long long queries;
		long long inserted;
		long long collisions;
};

#endif



#include "HashTablePos.h"

HashTablePos::HashTablePos(uint * data, uint dataLen, int * buff, uint buffLen) {
	this->data=data;
	this->dataLen=dataLen;
	this->buff=buff;
	this->buffLen=buffLen;
	for(uint i=0;i<2*buffLen;i++) 
		buff[i] = -1;                         //**valor por defecto = -1 < 0
	inserted=collisions=0;
	queries=0;
}

HashTablePos::~HashTablePos() {
}

//** Inserta un par en la tabla hash, y si ya estaba incrementa su contador de ocurrencias.
bool HashTablePos::increment(uint pos) {
	queries++;
	int p = h(pos);
	uint lcollisions=0;
	while(buff[2*p]!=-1) {    //** el slot [2p, 2p+1] de la tabla hash NO está libre
		if(lcollisions>=buffLen*maxAlpha) {
			return false;
		}
		if(cmp(pos,buff[2*p])){  //** el slot [2p,2p+1] de la tabla hash almacena info del par en la posición <<data[pos],data[pos+1]>>
			buff[2*p+1]++;
			return true;
		}
		collisions++; lcollisions++;
#ifdef VERBOSE
		//if(lcollisions%10000==0) {
		//	printf("collisions: %u\n",lcollisions); fflush(stdout);
		//}
#endif
		p = (p+s(pos))%buffLen;
	}
	
	//** en este momento p es la posición donde estaría el par "data[pos],data[pos+1]" en la tabla hash.
	//** y ese par todavía no está en la TH.
	
	if((double)inserted/buffLen>=maxAlpha) {
		return false;
	}
	//printf("Inserted: %d\tAlpha: %f\n",inserted,(double)inserted/buffLen);
	buff[2*p] = pos;
	buff[2*p+1] = 1;
	inserted++;
	return true;
}

uint HashTablePos::get(uint pos) {
	int p = h(pos);
	uint lcollisions=0;
	while(buff[2*p]!=-1) {
		if(lcollisions>=buffLen*maxAlpha) 
			return 0;
		if(cmp(pos,buff[2*p])){
			return buff[2*p+1]+1;
		}
		lcollisions++;
		p = (p+s(pos))%buffLen;
	}
	return 0;
}

bool HashTablePos::insert(uint pos, uint value) {
	int p = h(pos);
	uint lcollisions=0;
	while(buff[2*p]!=-1) {
		if(lcollisions>=buffLen*maxAlpha) 
			return false;
		if(cmp(pos,buff[2*p])){
			buff[2*p+1] = value;
			return true;
		}
		collisions++; lcollisions++;
		p = (p+s(pos))%buffLen;
	}
	if((double)inserted/buffLen>=maxAlpha)
		return false;
	//printf("Inserted: %d\tAlpha: %f\n",inserted,(double)inserted/buffLen);
	buff[2*p] = pos;
	buff[2*p+1] = value;
	inserted++;
	return true;
}

uint HashTablePos::h(uint pos) {
	return (data[pos]+(data[pos]<<16)^data[pos+1])%buffLen;
}

uint HashTablePos::s(uint pos) {
	return data[pos];
}

bool HashTablePos::cmp(uint pos1, uint pos2) {
	return data[pos1]==data[pos2] && data[pos1+1]==data[pos2+1];
}


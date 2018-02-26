
#include "Heap.h"

Heap::Heap(uint k) {
	elements = new pair<uint,uint>*[k+1];
	for(uint i=0;i<k+1;i++)
		elements[i]=NULL;
	assert(elements!=NULL);
	inserted = 1; this->k=k;
}

Heap::~Heap() {
	for(uint i=0;i<k+1;i++)
		if(elements[i]!=NULL)
			delete elements[i];
	delete [] elements;
}

void Heap::insert(pair<uint,uint> * elem) {
	if(inserted<=k) { // Insercion normal
		elements[inserted] = elem;
		for(uint j=inserted;j>1 && elements[j]->second<elements[j/2]->second;j/=2) {
			pair<uint,uint> *t = elements[j/2];
			elements[j/2] = elements[j];
			elements[j] = t;
		}
		inserted++;
	}
	else if(elements[1]->second<elem->second) { // Debemos eliminar alguno
		delete elements[1];
		elements[1] = elem;
		uint j = 1;
		while(2*j<=k) {
			uint p = 2*j;
			if(p+1<=k && elements[p+1]->second<elements[p]->second)
				p++;
			if(elements[j]->second<elements[p]->second)
				break;
			pair<uint,uint> *t = elements[j];
			elements[j] = elements[p];
			elements[p] = t;
			j = p;
		}
	}
	else
		delete elem;  //** no es más frecuente que los K existentes.
}


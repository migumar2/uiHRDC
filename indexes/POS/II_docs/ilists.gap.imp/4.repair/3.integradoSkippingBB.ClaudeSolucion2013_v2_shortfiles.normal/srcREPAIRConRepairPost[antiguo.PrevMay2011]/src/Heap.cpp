
#include "Heap.h"

Heap::Heap(ulong k) {
	elements = new pair<ulong,ulong>*[k+1];
	for(ulong i=0;i<k+1;i++)
		elements[i]=NULL;
	assert(elements!=NULL);
	inserted = 1; this->k=k;
}

Heap::~Heap() {
	for(ulong i=0;i<k+1;i++)
		if(elements[i]!=NULL)
			delete elements[i];
	delete [] elements;
}

void Heap::insert(pair<ulong,ulong> * elem) {
	if(inserted<=k) { // Insercion normal
		elements[inserted] = elem;
		for(ulong j=inserted;j>1 && elements[j]->second<elements[j/2]->second;j/=2) {
			pair<ulong,ulong> *t = elements[j/2];
			elements[j/2] = elements[j];
			elements[j] = t;
		}
		inserted++;
	}
	else if(elements[1]->second<elem->second) { // Debemos eliminar alguno
		delete elements[1];
		elements[1] = elem;
		ulong j = 1;
		while(2*j<=k) {
			ulong p = 2*j;
			if(p+1<=k && elements[p+1]->second<elements[p]->second)
				p++;
			if(elements[j]->second<elements[p]->second)
				break;
			pair<ulong,ulong> *t = elements[j];
			elements[j] = elements[p];
			elements[p] = t;
			j = p;
		}
	}
	else
		delete elem;  //** no es más frecuente que los K existentes.
}


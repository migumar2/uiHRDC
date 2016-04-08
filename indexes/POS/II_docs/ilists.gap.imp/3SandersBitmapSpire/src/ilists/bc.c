#include "bc.h"

uint writeIntBytes (byte *buffer, uint pos, uint number, uint numBytes) {
		register uint i;
		uint tmp; 
		byte *src;
		tmp = LE_LONG32(number);
		tmp = LINE_UP (tmp,numBytes);
		src = (byte *) &tmp;
		for (i=1;i<= numBytes;i++) buffer[pos++]= *(src++);	
		return pos;	
	}

uint readIntBytes (byte *buffer, uint pos, uint *number, uint numBytes) {
		register uint i, tmp; 
		tmp = buffer[pos++];
		for (i=1;i<numBytes;i++) {
			tmp <<=8;
			tmp +=buffer[pos++];
		}
		*number=tmp;
		return pos;	
	}




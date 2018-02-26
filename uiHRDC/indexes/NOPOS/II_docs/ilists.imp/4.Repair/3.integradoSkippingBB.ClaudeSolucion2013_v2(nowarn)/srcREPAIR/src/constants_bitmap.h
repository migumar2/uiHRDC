//#define  LEN_BITMAP_DIV 8

#define lenBitmapThreshold(maxPostValue,lenBitmapDiv )  ((maxPostValue) / lenBitmapDiv)


	/* checks if an id should use either bitmap or bytecodes: valid during build_il() */
int shouldUseBitmap(uint *lenList, uint id, uint maxPostValue, uint lenBitmapDiv) {  /* 0 <= id < il->nlists */
	if (lenList[id] > lenBitmapThreshold(maxPostValue,lenBitmapDiv))
		return 1;
	return 0;
}



#ifndef BC_INCLUDED
#define BC_INCLUDED


#define CEILLOG_2(x,v)                      \
{                                           \
    register ulong _B_x = (x) ; _B_x--;      \
    (v) = 0;                                \
    for (; _B_x ; _B_x>>=1, (v)++);         \
}                 

#define DEC_TO_BC(bc, pos, num)             \
{ while (num >= 128) {                      \
 bc[pos++] = (byte)((num & 127) + 128);     \
 num = (num >> 7);                          \
}                                           \
bc[pos++] = (byte) num;                     \
}

#define SIZE_DEC_TO_BC(num,size)            \
{uint numtmp=num;                           \
  size=0;                                   \
  while (num >= 128) {                      \
   size++;                                  \
   num = (num >> 7);                        \
  }                                         \
size++; num=numtmp;                         \
}			
			
#define BC_TO_DEC(bc,pos,num) \
 { \
 	 register uint _pot, _x; \
 	 _pot=1; \
 	 _x = bc[pos++]; \
 	 num=0; \
 	 while (_x >= 128) { \
 	 	num = num + (_x-128)*_pot; \
 	 	_x=bc[pos++]; \
 	 	_pot<<=7; \
 	 } \
 	 num=num + _x*_pot; \
 }

//void check_BC_works(uint limit){
//	uint num, size,i;
//	num=0; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6d is encoded with %d bytes",num,size);
//	num=1; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6d is encoded with %d bytes",num,size);
//	num=128-1; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6d is encoded with %d bytes",num,size);
//	num=128+0; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6d is encoded with %d bytes",num,size);
//	num=128+1; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6d is encoded with %d bytes",num,size);
//
//	num=128*128-1; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6d is encoded with %d bytes",num,size);
//	num=128*128+0; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6d is encoded with %d bytes",num,size);
//	num=128*128+1; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6d is encoded with %d bytes",num,size);
//	
//	
//	byte code[10];
//	for (i=0;i<limit;i++) {
//		uint pos =0;
//		num =i;
//		DEC_TO_BC(code, pos, num);
//		pos =0;
//		BC_TO_DEC(code,pos,num);
//		if (i != num) {fprintf(stderr,"\n DISTINTOS !!!!");
//			return ;
//		}
//	}	
//	fflush(stderr);
//}











//some macros to represent a uint "num" inside a (byte * buffer). Depending on the number of 
//bytes needed, to represent "num" in binary --> 1,2,3,4 bytes are written to buffer.
//Also macros to recover (byte * to uint) such a value are provided.

#ifndef uchar
#define uchar unsigned char
#endif
#ifndef uint
#define uint  unsigned int
#endif
#ifndef ulong
#define ulong unsigned long
#endif
#ifndef byte
#define byte unsigned char
#endif

// Uncomment the architecture used.

 #ifndef LITTLE_ENDIAN
 #define LITTLE_ENDIAN
 #endif

//	#ifndef  BIG_ENDIAN
//	#define BIG_ENDIAN
//	#endif

#define LINE_UP(x,numBytes) ((x>>(4-numBytes)*8))    //move "4-bytes" bytes to the right
						   						     //so we can write "bytes" bytes from &x
#ifdef LITTLE_ENDIAN
	#define LE_LONG32(x)  (((x&0xff000000)>>24)|((x&0x00ff0000)>>8)|((x&0x0000ff00)<<8)|((x&0x000000ff)<<24))
#else
	#define LE_LONG32(x) x
#endif

 uint writeIntBytes (byte *buffer, uint pos, uint number, uint numBytes);
 uint readIntBytes (byte *buffer, uint pos, uint *number, uint numBytes);



 #define SIZE_UINT_TO_BYTE(num,size)        \
{uint numtmp=num;                           \
  size=0;                                   \
  while (num >= 256) {                      \
   size++;                                  \
   num = (num >> 8);                        \
  }                                         \
size++; num=numtmp;                         \
}	
 
 #endif


// (byte *buffer, uint pos, uint number, uint numBytes)
#define UINT_TO_BYTE(buffer, pos, number, numBytes)             \
{ 		register uint i, tmp; \
		byte *src; \
		tmp = LE_LONG32(number); \
		tmp = LINE_UP (tmp,numBytes); \
		src = (byte *) &tmp; \
		for (i=1;i<= numBytes;i++) buffer[pos++]= *(src++);	 \
}

// (byte *buffer, uint pos, uint number, uint numBytes)
#define BYTE_TO_UINT(buffer, pos, number, numBytes) \
{ 		register uint i, tmp; \
		tmp = buffer[pos++]; \
		for (i=1;i<numBytes;i++) { \
			tmp <<=8; \
			tmp +=buffer[pos++]; \
		} \
		number=tmp; \
}



//void check_BYTES_2_UINT_works() {
//	uint numAbsBytes;
//	uint maxPostValue= 50000;
//	SIZE_UINT_TO_BYTE(maxPostValue,numAbsBytes);
//		
//	byte *B = (byte *) malloc (100);
//	uint *u;
//	byte *b;
//	b=B;
//	*b=1;b++;
//	*b=2;b++;
//	*b=3;b++;
////	u= (uint *)b; 
////	*u=66666666;u++;
////	b = (byte *) u;
//	
//	uint pos=3;
////	pos = writeIntBytes (B,pos, 999, numAbsBytes);
//	UINT_TO_BYTE(B, pos, 99, numAbsBytes);
//	printf("\n 999 = %d, pos=%d, numAbsBytes = %d",99, pos, numAbsBytes);
//	
//	
//	b=&(B[pos]);
//
//	*b=4;b++;
//	*b=5;b++;
//	
//	b=B;
//	printf("\n 1 = %d",*b++);
//	printf("\n 2 = %d",*b++);
//	printf("\n 3 = %d",*b++);
////	u= (uint *)b; 
////	printf("\n 66666666 = %d",*u++);
////	b = (byte *) u;
//
//	 pos=3; uint temp;
//	//pos = readIntBytes (B,pos, &temp, numAbsBytes);
//	BYTE_TO_UINT(B, pos, temp, numAbsBytes);
//	printf("\n 999 = %d, pos=%d, numAbsBytes = %d",temp,pos, numAbsBytes);
//	b=&(B[pos]);
//
//	printf("\n 4 = %d",*b++);
//	printf("\n 5 = %d",*b++);
//		
//}


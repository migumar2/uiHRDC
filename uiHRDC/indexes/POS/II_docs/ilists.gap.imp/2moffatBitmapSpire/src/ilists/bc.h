

//const unsigned long TABLABASE[5] = {0,128,16512,2113664,270549120};
////decoding for ETDC
//uint decode (byte *etdc, register uint pos, uint *num) {
//	register uint k=pos;
//	register uint j;
//	j=0;
//	while (etdc[pos] >=128) {
//		j = j * 128 + (etdc[pos] - 128);
//		pos++;
//	}
//	j = j * 128 + etdc[pos];
//	j += TABLABASE[pos-k]; //the position of the word in the vocabulary
//	pos++;
//	*num = j;
//	return pos;
//}

#define CEILLOG_2(x,v)                      \
{                                        \
    register unsigned _B_x = (x) - 1;       \
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

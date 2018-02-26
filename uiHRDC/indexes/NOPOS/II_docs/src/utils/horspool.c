 
#include "horspool.h"


/*------------------------------------------------------------------
  Initializes Horspool's d-vector (jumps for each letter)
 ------------------------------------------------------------------*/

void inicialize_d(byte *code, unsigned int len, byte *d) {
	unsigned int c,k;

	for (c=0;c<256;c++) {d[c]=len;}

	for (k=0; k<len-1; k++) {
		c=code[k];
		d[c] = MIN ( d[c], len - k-1);
	}
}





/*---------------------------------------------------------------------
 * Performs search process over the whole file returning the positions
----------------------------------------------------------------------- */

//uint searchPositionsFullFile (uint startOffset, byte *cbeg, byte *cend, byte *code, int len, uint *positions, uint *size) {
uint searchPositionsFullFile (uint startOffset, byte *cbeg, byte *cend, byte *code, int len, 
															uint *positions, uint *size, byte *d) {
	//register int i;
	//unsigned int matches=0;
	unsigned long int cnt = 0;
	
	register byte *cpos;
	register byte *cj;
	register int j;
	register unsigned int m;
	
	//inicialize_d(code, len, d);
		
	m=len;	
	cpos = cbeg + len -1;

	while (1)   {
		j=m-1;
		
		/*** skip loop  ***/		
		while ((cpos<cend) && ( *cpos != code[j] ))  {cpos += d[*cpos];}
				////fprintf(stderr,"matches %d cend %x cpos %x\n",matches,cend,cpos);

		if(cpos>=cend){
		//	printf( "\n... search FINISHED, %d appeareances!!!\n", cnt);;fflush(stderr);
			*size = cnt;
			return cnt;	
		}
		/*** normal loop  ***/
		cj= cpos-1;
		j--;
				
		while ((j>=0) && ((*cj) == code[j])) {	j--;cj--;}  //check backwards
		
		if (j==-1) {  //if the prev byte is a stopper 
				positions[cnt++] = startOffset+cj+1-cbeg;		
		//		printf("\n ### match at position %d",startOffset+cj+1-cbeg);	
		//		printf("\n [%d][%d][%d][%d][%d][%d]",*(cpos-3),*(cpos-2),*(cpos-1),*(cpos),*(cpos+1),*(cpos+2));	
		}
		cpos += d[*cpos];
			
	} //while ( 1 )

	//	printf( "\n... search FINISHED, %d appeareances!!!\n", cnt);;fflush(stderr);
		*size = cnt;
		return cnt;		
}


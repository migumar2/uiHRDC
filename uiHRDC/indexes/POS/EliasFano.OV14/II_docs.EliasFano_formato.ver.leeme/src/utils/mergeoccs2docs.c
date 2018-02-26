#include "mergeoccs2docs.h"



/** *****************************************************************************/
/** Translates absolute occs[] into docs[] positions. So union of occs in the same doc 
/** *****************************************************************************/

/* For each occ[i], finds its corresponding document "doc_i"
 * returns the offset inside doc_i, and the value doc_i.
 * The first noccs integers are positions within docs.
 * The next noccs integers are the doc-ids for such occurrencias.
 * Returns an array of size 2*noccs integers.
 */

int addDocIdsToOccs(uint **occs, uint noccs, uint *docbeg, uint ndocs) {
  uint *src=*occs;
  uint n=noccs;
  
  uint *result = (uint*) malloc(sizeof(uint)*n*2);
  //if (!result) {printf("\n OUT OF MEMORY IN MALLOC: addDocIdsToOccs");exit(-1);} //out of memory.

  uint i=0;  uint ri=0;
  uint currdoc=0;
  
  while (i<n) { //adds one entry (ri) to result per iteration.
	ffsearch3_exp(src[i], docbeg,&currdoc,ndocs);
	result[i]  = src[i] - docbeg[currdoc]; //offset inside the current doc.
	result[i+n]= currdoc;                 //document.
	i++;
	
	while ((i<n) && (src[i]< docbeg[currdoc+1])){ //skip loop (jump over occs in the same doc).
	  result[i]  = src[i] - docbeg[currdoc]; //offset inside the current doc.
	  result[i+n]= currdoc;                 //document.	  
	  i++;
	}	
	currdoc++;
  }    
  free(src);
  
  *occs= result;
}




















/* given occs[], a list of positions within the collection (occs[] is a word rank
 *            from the Sids[] mapping of the original text.
 * and docbeg[], the rank of the beginnings of each document over Sids[],
 * returns the documents that contain those positions (several occs[i.. i+k] can be joined into its docID).
 * the result is set into occs, and *noccs is set property.
 */

/** optimized form 1 ************************************************************/
int mergeOccsIntoDocOccs(uint *occs, uint *noccs, uint *docbeg, uint ndocs) {
  uint *result=occs;
  uint n=*noccs;
  uint i=0;  uint ri=0;
  uint currdoc=0;
  
  while (i<n) { //adds one entry (ri) to result per iteration.
	ffsearch3_exp(occs[i], docbeg,&currdoc,ndocs);
	result[ri]=currdoc;
	i++;
	
	while ((i<n) && (occs[i]< docbeg[currdoc+1])){ //skip loop (jump over occs in the same doc).
	  i++;
	}	
	ri++;
	currdoc++;
  }    
  *noccs = ri;
}

/** simple-unoptimized form 2 (just for checking) *******************************/

uint * mergeOccsIntoDocOccsTrivial(uint *occs, uint noccs, uint *docbeg, uint ndocs, uint *size) {
  uint n= noccs;
  uint *docs = (uint *) malloc (sizeof(uint) * (n));  

  uint ri=0;
  int prevdoc=-1;  
  uint currdoc=0;
  for (uint i=0;i<n;i++) {
	ffsearch1(occs[i], docbeg,&currdoc,ndocs);
	if (currdoc != prevdoc) {
	  docs[ri]=currdoc;
	  ri++;
	  prevdoc = currdoc;
	}			
  }
  *size = ri;
  return docs;
}




/** *****************************************************************************/
/** fsearch-operations                                                          */ 
/** *****************************************************************************/

  /* Search for: first position in docbeg[] such that docbeg[i+1]<value */
void ffsearch1(uint value, uint *docbeg, uint *currdoc, uint ndocs) {
	int i=*currdoc;
	for ( ;i<ndocs;i++) {
	  if ((value >= docbeg[i]) && (value < docbeg[i+1])) {
		*currdoc = i;
		return;
	  }
	}
}

  /* Search for: first position in docbeg[] such that docbeg[i+1]<value */

void ffsearch2(uint value, uint *docbeg, uint *currdoc, uint ndocs) {
	int i=*currdoc;
	while ((i<ndocs) && ((docbeg[i+1]<value)) ) {
	  i++;
	}
	*currdoc = i;
}

  /* Search for: first position in docbeg[] such that docbeg[i+1]<value */
void ffsearch3_exp(uint target, uint *V, uint *curr, uint n) {
	int i=*curr;
	
	int l,r,m,pos; int probe;
	uint val;
	
	l=i;
	probe=1;
	if (l<n){
	  pos = (probe+l);
	  val = V[pos];
	  /** exp search **/
	  while (((probe+l) < n) && (val < target)) {
		l+=probe;
		probe *=2;
		//printf("\n jump = %u", probe);
		if ((l+probe)>n) probe = n-l;
		pos = (probe+l);
		val = V[pos];
	  }
	  r =l+probe;
	  
	  /** bin search **/
	  while (r>(l+1)) {
		m= (l+r)/2;
		pos = m;
		val =V[pos];
		if (val < target) l=m;
		else r=m;
	  }
	}
	
	pos =r;
	val = V[pos];
	
	if (val < target) {
	  l=r;
	}
	else if (val == target) {i=r; *curr=i; return;}
	else {
	  pos = l;
	  val = V[pos];
	  if (val ==target) {i=l; *curr=i; return;}
	  else if (val > target) {i=l;  *curr = i; return; } //not found.  
	}
	*curr = l;	
}

/** *****************************************************************************/
 

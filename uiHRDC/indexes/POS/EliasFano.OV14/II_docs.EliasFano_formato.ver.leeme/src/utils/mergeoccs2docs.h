
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

/* Translates occs-2-doc-ids */
/*  occ(1+log(docs/occ) */
/* occ log(docs) o sino pagar occ(1+log(docs/occ)+log(occ))=occ log(docs) */

/** :: some public funcions in this module:: ------------------------------------
 */

  /** *****************************************************************************/
  /** Translates absolute occs[] into docs[] positions. So union of occs in the same doc 
  /** *****************************************************************************/

/* For each occ[i], finds its corresponding document "doc_i"
 * returns the offset inside doc_i, and the value doc_i.
 * The first noccs integers are positions within docs.
 * The next noccs integers are the doc-ids for such occurrencias.
 * Returns an array of size 2*noccs integers.
 */
  int addDocIdsToOccs(uint **occs, uint noccs, uint *docbeg, uint ndocs);

  
  /** *****************************************************************************/
  /** Translates absolute occs[] into docs[] positions. So union of occs in the same doc 
  /** *****************************************************************************/

  /* given occs[], a list of positions within the collection (occs[] is a word rank
	*            from the Sids[] mapping of the original text.
	* and docbeg[], the rank of the beginnings of each document over Sids[],
	* returns the documents that contain those positions (several occs[i.. i+k] can be joined into its docID).
	* the result is set into occs, and *noccs is set property.
	*/

	/** optimized form 1 ************************************************************/
	int mergeOccsIntoDocOccs(uint *occs, uint *noccs, uint *docbeg, uint ndocs);


	/** simple-unoptimized form 2 (just for checking purposes) **********************/
	uint * mergeOccsIntoDocOccsTrivial(uint *occs, uint noccs, uint *docbeg, uint ndocs, uint *size);


  
/** :: some private funcions in this module:: -------------------------------------
 */

  /** *****************************************************************************/
  /** fsearch-operations                                                          */ 
  /** *****************************************************************************/
	/* Search for: first position in docbeg[] such that docbeg[i+1]<value */
	void ffsearch1(uint value, uint *docbeg, uint *currdoc, uint ndocs);     //seq search- version 1
	void ffsearch2(uint value, uint *docbeg, uint *currdoc, uint ndocs);     //seq search- version 2
	void ffsearch3_exp(uint value, uint *docbeg, uint *currdoc, uint ndocs); //exp search - version 3

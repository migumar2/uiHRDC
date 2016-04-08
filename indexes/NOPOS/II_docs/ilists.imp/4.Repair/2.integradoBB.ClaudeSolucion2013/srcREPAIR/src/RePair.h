
#ifndef REPAIR_H
#define REPAIR_H

#include "basic.h"
#include "Heap.h"
#include "HashTablePos.h"
#include "HashTablePairs.h"

#include <utility>
#include <iostream>
#include <algorithm>

#define FIRST 0
#define REPLACED 1
#define NOT_SEEN 2
#define VERBOSE 

using namespace std;

class RePair {
	public:
	
	  /** Creates a RePair Object ready for compressing 
	   * The buffer is now owned by the RePair object.
	   */
		RePair(uint * buff, uint datalen, uint _bufflen, char * _basename, uint _K);
		
		/** Creates and reads a RePair object stored under basename 
		 */
		//RePair(char * basename);
		
		/** Compresses the data 
		 */
		virtual void compress(uint nodes);
		virtual void compress(uint nodes, double corte);
		
		/** Saves the compressed data 
		 */
		virtual bool save(uint nodes);
		
		/** Frees everything 
		 */
		virtual ~RePair();

  protected:
  
    /** Eliminate 0's from the data 
     */
    virtual void rearrange();
    
    /** Selects the candidates to be replaced 
     */
    virtual uint replacePairs();
    virtual void fillHeap();
    
    /** Loads the RPFile 
     */
//    virtual bool loadRPFile();
   
    /** Loads the CRP File 
     */
//    virtual bool loadCRPFile();
    
    /** Loads the dictionary 
     */
//    virtual bool loadDict();
    
    /** Loads the compressed dictionary
     */
//    virtual bool loadCDict();

    /** Saves the RPFile 
     */
    virtual bool saveRPFile(uint nodes); 
		
    /** Saves the CRPFile 
     */
//    virtual bool saveCRPFile(); 

    /** Saves the dictionary file 
     */
    //virtual bool saveDict(); 

    /** Saves the compressed dictionary file 
     */
//    virtual bool saveCDict(); 
    
    virtual void addRule(uint nvalue, uint p1, uint p2);

	protected:
		
		double CORTE_REPAIR;
	
		uint * data;
		uint bufflen;
		uint max_value, K;
		long mini;
		uint lastpos;
		string basename;
    uint max_assigned;
 		Heap * heap;
 		HashTablePos * hpos;
 		HashTablePairs *hpairs;
 		FILE * fpDict;
		
		// Aun no usado
		uint * symbols_pair;
    uint m; // Tamano final
    uint n; // Tamano original
    uint *symbols; // simbolos con reemplazos
    uint shift;
    uint * symbols_new, symbols_new_len; // diccionario y su largo
    uint nbits;
/*    BitRankW32Int * BR; // 1->original, 0->reemplazado
    BitRankW32Int * BRR; // Se construye pal arbolito*/
};

#endif


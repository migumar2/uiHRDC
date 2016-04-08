
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
		RePair(ulong * buff, ulong datalen, ulong _bufflen, char * _basename, ulong _K);
		
		/** Creates and reads a RePair object stored under basename 
		 */
		//RePair(char * basename);
		
		/** Compresses the data 
		 */
		virtual void compress(ulong nodes);
		virtual void compress(ulong nodes, double corte);
		
		/** Saves the compressed data 
		 */
		virtual bool save(ulong nodes);
		
		/** Frees everything 
		 */
		virtual ~RePair();

  protected:
  
    /** Eliminate 0's from the data 
     */
    virtual void rearrange();
    
    /** Selects the candidates to be replaced 
     */
    virtual ulong replacePairs();
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
    virtual bool saveRPFile(ulong nodes); 
		
    /** Saves the CRPFile 
     */
//    virtual bool saveCRPFile(); 

    /** Saves the dictionary file 
     */
    //virtual bool saveDict(); 

    /** Saves the compressed dictionary file 
     */
//    virtual bool saveCDict(); 
    
    virtual void addRule(ulong nvalue, ulong p1, ulong p2);

	protected:
		
		double CORTE_REPAIR;
	
		ulong * data;
		ulong bufflen;
		ulong max_value, K;
		long mini;
		ulong lastpos;
		string basename;
    ulong max_assigned;
 		Heap * heap;
 		HashTablePos * hpos;
 		HashTablePairs *hpairs;
 		FILE * fpDict;
		
		// Aun no usado
		ulong * symbols_pair;
    ulong m; // Tamano final
    ulong n; // Tamano original
    ulong *symbols; // simbolos con reemplazos
    ulong shift;
    ulong * symbols_new, symbols_new_len; // diccionario y su largo
    ulong nbits;
/*    BitRankW32Int * BR; // 1->original, 0->reemplazado
    BitRankW32Int * BRR; // Se construye pal arbolito*/
};

#endif


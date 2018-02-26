
#ifndef REPAIRDECOMPRESSOR
#define REPAIRDECOMPRESSOR

#include <iostream>
#include <cassert>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "bitrankw32int.h"
#include "basic.h"

using namespace std;

class RepairPost {
  public:
  
	unsigned char *text;
	unsigned long textSize;
	unsigned long blockSize;
	

	RepairPost();
	
		//** Constructor **
		//** Inicializa blockSize y q. No carga aún el texto
	RepairPost(unsigned long block_size);

		//** Constructor **
		//** Comprime la secuencia src[0..len-1] con repair, teniendo en cuenta que los bloques
    //** tienen tamaño "block_size"  
 	RepairPost(unsigned char *src, unsigned long len, unsigned long block_size);   
	
		//**saves the created structure.
	void save (char * filename);   

		//**loads the created structure.
	void load (char * filename);

		//** Devuelve el texto "sin comprimir" del bloque "i" (el primer bloque es el 0). 
		//** Lo usaré posteriormente para el horspool
	void extract_block (uint i, unsigned char *block, unsigned long *len);

		//** Memoria utilizada  (secuencia comprimida + ptr[] + jerarquía frases) 
	unsigned long get_size();

		//** Frees resources **
   ~RepairPost();// ** Libera memoria **
   
   
private:
  BitRankW32Int * BRR; // Se construye pal arbolito
  uint *symbols; // simbolos con reemplazos
  
	uint * ptrs;    
	uint ptrs_len;
	uint plen;    
	uint * info;   //**VECTOR TEMPORAL USADO PARA EXTRAER S�MBOLOS (extract())
	uint pos;

unsigned long getFileSize (char *filename);
	   
};

#endif


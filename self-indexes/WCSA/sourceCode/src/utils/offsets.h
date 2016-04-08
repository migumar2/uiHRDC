//Francisco: Mapear de occ --> <doc, occ>

//Cualquier cosa o condoro que me haya mandado me avisan. La funcion a llamar es: 
//uint * doc_offset_exp(uint *offsets, uint len_offs, uint * positions,uint len_posn),
//le dan los offsets y las posiciones (con sus respectivos 
//largos) y retorna un arreglo del doble del tamanio de positions donde las 
//posiciones impares tienen el doc_id y las pares el offsets (empezando de 1),
//la posicion 0 contiene el largo del arreglo.

//Hay tres versiones segun un parametro MYBS, resulta que la stl tiene una busqueda
//binaria que parece funcionar mejor que mia de manera consistente (el delta es minimo,
//pero ademas estamos mas seguros que esa no tiene bugs :-). Yo no recomendaria cambiar
//MYBS salvo que quieran probar alguna potencial mejora.


/**
 *    offsets.h
 *    Copyright (C) 2011  Francisco Claude F.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <algorithm>
using namespace std;

#define MYBS 0

inline uint exp_search(uint *offsets, uint ini, uint fin, uint val) {
  if(offsets[ini+1] > val) return ini;
  uint add = 1;
  uint inipadd = ini + add;
  while(inipadd<=fin) {
    if(offsets[inipadd] > val) {
#if MYBS > 1
      uint bs_ini = ini;
      uint bs_fin = inipadd;
      uint bs_med = (bs_ini+bs_fin)/2;
      while(bs_ini <= bs_fin) {
	if(offsets[bs_med] >= val) {
	  bs_fin = bs_med;
	} else {
	  if(bs_ini == bs_med) {
	    bs_fin = bs_ini-1;
	    break;
	  }
	  bs_ini = bs_med;
	}
	bs_med = (bs_ini+bs_fin)/2;
      }
      bs_fin++;
      return bs_fin;
#else
      uint * pos = upper_bound(offsets+ini,offsets+inipadd+1,val);
      pos--;
      return pos-offsets;
#endif
    }
    add *= 2;
    inipadd = ini + add;
  }
#if MYBS > 0
  uint bs_ini = ini;
  uint bs_fin = fin;
  uint bs_med = (bs_ini+bs_fin)/2;
  while(bs_ini <= bs_fin) {
    if(offsets[bs_med] >= val) {
      bs_fin = bs_med;
    } else {
      if(bs_ini == bs_med) {
	bs_fin = bs_ini-1;
	break;
      }
      bs_ini = bs_med;
    }
    bs_med = (bs_ini+bs_fin)/2;
      }
  bs_fin++;
  return bs_fin;
#else
  uint * pos = upper_bound(offsets+ini,offsets+fin+1,val);
  pos--;
  return pos-offsets;
#endif
}

inline uint * doc_offset_exp(uint *offsets, uint len_offs, ulong * positions, ulong len_posn) {
  uint * result = new uint[len_posn*2+1];
  result[0] = len_posn*2;
  sort(positions,positions+len_posn);
  uint ini = 0;
  for(uint i=1;i<=len_posn;i++) {
    uint pos = exp_search(offsets,ini,len_offs-1,(uint)positions[i-1]);
    result[2*i-1] = pos+1;
    result[2*i] = ((uint)positions[i-1])-offsets[pos];
    ini = pos;
  }
  return result;
}

inline uint * doc_offset_exp(uint *offsets, ulong * positions) {
  return doc_offset_exp(offsets+1,offsets[0],positions+1,positions[0]);
}

inline uint * doc_offset_exp_Fari(uint *offsets, uint ndocs, ulong *positions, ulong n) {
  return doc_offset_exp(offsets,ndocs,positions,n);
}













/****************************************************************************************************************/
/**  IMPLEMENTED BY FARI ****************************************************************************************/
/****************************************************************************************************************/


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
 

/** *****************************************************************************/
/** Translates absolute occs[] into docs[] positions. So union of occs in the same doc 
/** *****************************************************************************/

/* For each occ[i], finds its corresponding document "doc_i"
 * returns the offset inside doc_i, and the value doc_i.
 * The first noccs integers are positions within docs.
 * The next noccs integers are the doc-ids for such occurrencias.
 * Returns an array of size 2*noccs integers.
 */

uint * addDocIdsToOccs(ulong **candoccs, uint noccs, uint *docbeg, uint ndocs) {
  ulong *src=*candoccs;
  uint n=noccs;
  
  ulong * positions=*candoccs; ulong len_posn=n;
   sort(positions,positions+len_posn);
  
  uint *result = (uint*) malloc(sizeof(uint)*n*2);
  //if (!result) {printf("\n OUT OF MEMORY IN MALLOC: addDocIdsToOccs");exit(-1);} //out of memory.

  uint i=0;  uint ri=0;
  uint currdoc=0;
  
  while (i<n) { //adds one entry (ri) to result per iteration.
	ffsearch3_exp( (uint)src[i], docbeg,&currdoc,ndocs);
	result[i]  = (uint)src[i] - docbeg[currdoc]; //offset inside the current doc.
	result[i+n]= currdoc;                 //document.
	i++;
	
	while ((i<n) && ((uint)src[i]< docbeg[currdoc+1])){ //skip loop (jump over occs in the same doc).
	  result[i]  = (uint)src[i] - docbeg[currdoc]; //offset inside the current doc.
	  result[i+n]= currdoc;                 //document.	  
	  i++;
	}	
	currdoc++;
  }    
  free(src);
  
  return result;
}



/****************************************************************************************************************/
/**  IMPLEMENTED BY FARI ****************************************************************************************/
/****************************************************************************************************************/





/* sort_alg.c
 * Copyright (C) 2011, Antonio Fariña, all rights reserved.
 * 
 * sort_alg.c: Some comparing funtions for qsort().
 *    
 * Implementation of an representation of compressed postings lists. It provides
 *   Culpeper & Moffat skiper structure over postings lists compressed with vbyte.
 * 
 * Contact info:
 * Antonio Fariña, University of A Coruña
 * http://vios.dc.fi.udc.es/
 * antonio.fari[@]gmail.com  
 * 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "sort_alg.h"


/** sorting a uint-vector ascending, (O(n^2)) but valid as n<< **/
int simplesortAsc(tsort *V, uint n) {
	register int i,j;
	register uint min,tmp;
	for (i=0; i< n-1;i++) {
		min =i;
		for (j=i+1;j<n;j++) {
			if ( V[min].key > V[j].key ) min = j;
		}
		tmp =V[i].key;  V[i].key=V[min].key;     V[min].key=tmp;	//swap	
		tmp =V[i].value;V[i].value=V[min].value; V[min].value=tmp;	//swap	
	}
}
	
/** sorting a uint-vector descending, (O(n^2)) but valid as n<< **/
int simplesortDesc(tsort *V, uint n) {
	register int i,j;
	register uint max,tmp;
	for (i=0; i< n-1;i++) {
		max =i;
		for (j=i+1;j<n;j++) {
			if ( V[max].key < V[j].key ) max = j;
		}
		tmp =V[i].key;  V[i].key=V[max].key;     V[max].key=tmp;	//swap	
		tmp =V[i].value;V[i].value=V[max].value; V[max].value=tmp;	//swap	
	}
}


	
///** sorting a uint-vector descending, (O(n^2)) but valid as n<< **/
int simplesortUint(uint *V, uint n) {
	register int i,j;
	register uint max,tmp;
	for (i=0; i< n-1;i++) {
		max =i;
		for (j=i+1;j<n;j++) {
			if ( V[max] < V[j] ) max = j;
		}
		tmp =V[i];V[i]=V[max]; V[max]=tmp;	//swap	
	}
}

//int checkSimpleSort() {
//	
//	{
//	char text [100];
//	uint V[100], i;
//	uint len=100;;
//		while (len !=0) {
//			printf("\nIntro number of integers integer: ");
//			fgets(text, 100, stdin);
//			len = atoi(text);
//			if (!len) break;
//			i=0;
//			while(i<len) {	
//					printf("Intro integer [%d]: ",i);
//					fgets(text, 100, stdin);
//					V[i]=atoi(text);
//					i++;
//			}
//			printf("\nWill sort vector: ");
//			for (i=0;i<len;i++) printf("[%d]",V[i]);
//			simplesort(V,len);	
//			printf("\nSorted vector: ");	
//			for (i=0;i<len;i++) printf("[%d]",V[i]);	
//			printf("\n");				
//		}		
//	}
//		
//}


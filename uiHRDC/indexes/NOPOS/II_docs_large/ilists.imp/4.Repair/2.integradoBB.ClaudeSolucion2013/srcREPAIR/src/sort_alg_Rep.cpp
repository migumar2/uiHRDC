#include "sort_alg_Rep.h"


/** sorting a uint-vector ascending, (O(n^2)) but valid as n<< **/
int simplesortAsc_Rep(tsort_Rep *V, uint n) {
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
int simplesortDesc_Rep(tsort_Rep *V, uint n) {
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
int simplesortUint_Rep(uint *V, uint n) {
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


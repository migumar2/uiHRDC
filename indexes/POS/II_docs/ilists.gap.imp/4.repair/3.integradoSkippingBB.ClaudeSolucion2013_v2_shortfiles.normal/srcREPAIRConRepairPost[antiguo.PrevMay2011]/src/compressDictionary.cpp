
#include <iostream>
#include <errno.h>
#include <cassert>
#include "basic.h"
#include "bitrankw32int.h"

using namespace std;

uint *symbols_pair;
uint *symbols_new_bit; 
uint  symbols_new_bit_len;
uint *symbols;
uint n,m,nbits;

BitRankW32Int *BRR; //bitmap to indicate the structure of symbols_new
BitRankW32Int *BR;
uint *symbols_new; // table to decompress de new codes
uint  symbols_new_len; // len of symbols_new
uint min_value;
uint max_value;
uint max_assigned;


void  new_value(uint *symbols_pair,uint *symbols_new_value,uint *k1,uint *j,uint pos);

uint pos;

void unroll(uint s, int i) {
  if(s>max_value) {
    unroll(symbols_pair[2*(s-max_value-1)],i);
    unroll(symbols_pair[1+2*(s-max_value-1)],i);
    return;
  }
  pos++;
}

void fillBR(uint * bits) {
  pos = 0;
  for(uint i=0;i<m;i++) {
    bitset(bits,pos);
    unroll(symbols[i],i);   /* increases pos */
  }
}

void fillBR2() {
	uint bits_BR_len = (n+2);
	uint * bits_BR = (uint*)malloc(sizeof(uint)*((bits_BR_len+WW32-1)/WW32));
	for(uint i=0; i<(WW32-1+bits_BR_len)/WW32;i++)
		bits_BR[i]=0;

	fillBR(bits_BR);
  bitset(bits_BR,pos); /** ¿la última posición, para marcar el final ??? :: sin comprobar */
	BR = new BitRankW32Int(bits_BR, bits_BR_len , true, 20); 
}

void compress_pair_table() {
  uint aux,j,i;
  /* Compress table of pairs */
  fillBR2();
  
 		 //** max_assigned-max_value == número de reglas que se crearon = num nuevos símbolos . 
  uint *symbols_pair_tmp = (uint*) malloc(sizeof(uint)*(max_assigned-max_value));
  for (i =0 ; i< (max_assigned-max_value) ; i++) 
    symbols_pair_tmp[i]=0;

  for (i =0 ; i< (max_assigned-max_value) ; i++) {
    aux=symbols_pair[2*i];
    if (aux > max_value) {
      symbols_pair_tmp[aux-max_value-1]++;  //**establece un "1"
    }
    aux=symbols_pair[2*i+1];
    if (aux > max_value) {
      symbols_pair_tmp[aux-max_value-1]++;  //**establece un "1"
    }
  }
  
  j=0;
  for (i =0 ; i< (max_assigned-max_value); i++) {
    if (symbols_pair_tmp[i] != 0) 
      j++;                                /** cuenta los que no son terminales ??? */
  }
 
  symbols_new_len = 2*(max_assigned-max_value)-j;
  symbols_new = (uint*) malloc(sizeof(uint)*(symbols_new_len));
  symbols_new_bit = (uint*) malloc(sizeof(uint)*((symbols_new_len+(max_assigned-max_value))/32+1));
  uint *symbols_new_value = (uint*) malloc(sizeof(uint)*(max_assigned-max_value));
  assert(symbols_new_value!=NULL);
 
  for (i =0 ; i<((symbols_new_len+(max_assigned-max_value))/32+1);i++)
    symbols_new_bit[i]=0;
  for (i =0 ; i<symbols_new_len;i++)
    symbols_new[i]=0;
  for (i =0 ; i<(max_assigned-max_value);i++)
    symbols_new_value[i]=0;
  j=1;
  uint k1=0;
  for (i =0 ; i< (max_assigned-max_value) ; i++) {
    if (symbols_pair_tmp[i] == 0) {
      symbols_new_value[i]=j; bitset(symbols_new_bit,j-1); j++;
      new_value(symbols_pair,symbols_new_value,&k1,&j,i);
    }
  }
  symbols_new_bit_len = j;
  uint *symbols_new_bit_aux = new uint [(symbols_new_bit_len/32+1)];
  assert(symbols_new_bit_aux!=NULL);

  for (i =0 ; i<symbols_new_bit_len/32+1;i++) {
    symbols_new_bit_aux[i]= symbols_new_bit[i];
  }
  free(symbols_new_bit);
  symbols_new_bit=symbols_new_bit_aux;

  /* Solo para verificar */
  //for (i =0 ; i<(max_assigned-max_value);i++)
    //if (symbols_new_value[i]==0) printf("s[%lu]=%lu, frec=%lu\n",i,symbols_new_value[i], symbols_pair_tmp[i]);
  
  free(symbols_pair_tmp);
  uint cuentame=0,cuentame2=0,distancia=0;
  double maximo=0;
  nbits=bits(symbols_new_bit_len+max_value+1);
  uint *symbols_aux = (uint *) malloc(sizeof(uint)*((m/32*nbits + m%32*nbits/32)+1));
  assert(symbols_aux!=NULL);
  
  for (i =0 ; i< m ; i++) {
    aux = symbols[i];
    if (aux > max_value) {
      SetField(symbols_aux,nbits,i,symbols_new_value[aux-max_value-1]+max_value);
      //symbols[i] = symbols_new_value[aux-max_value-1]+max_value;
      /*if(i>=18412900)
        printf("i=%u aux=%u\n",i,aux); fflush(stdout);*/
      distancia+=BR->select(i+2)-BR->select(i+1);
      cuentame2++;
    } else {
      SetField(symbols_aux,nbits,i,aux);
      maximo+=distancia*distancia;
      distancia=0;maximo++;
      cuentame++;
    }
  }
  free(symbols);
  symbols=symbols_aux;

  /* Compact symbols_new */
  /*
  symbols_aux = (uint *) malloc(sizeof(uint)*((symbols_new_len*nbits)/32+1));
  for (i =0 ; i< symbols_new_len ; i++) {
    SetField(symbols_aux,nbits,i,symbols_new[i]);
  }
  free(symbols_new);
  symbols_new=symbols_aux;
  */
            
  free(symbols_new_value); 
  free(symbols_pair); 
  BRR=new BitRankW32Int(symbols_new_bit, symbols_new_bit_len , true, 20); 

  printf("Sin Comprimir=%u, comprimido=%u\n",cuentame,cuentame2); 
  printf("Costo prom descompr=%f\n",maximo/(float)n);
  printf("Original=%u\n",n); 
  printf("Simpolos finales=%u (%.6f%%)\n",m,m/(float)n*100);
  printf("Conjunto de pares=%u (%.6f%%) hubo %u reemplazos\n",2*(max_assigned-max_value),(2*(max_assigned-max_value))/(float)n*100,(max_assigned-max_value));
  aux=symbols_new_bit_len/32+1;
  printf("Conjunto de pares2=%u+%u (%.6f%%) ahorro  %u\n",symbols_new_len,aux,(symbols_new_len+aux)/(float)n*100,2*(max_assigned-max_value)-symbols_new_len-aux);
  printf("Estructura para rank para descomprimir=%u (%.6f%%) tira de bits+5%% extra \n",((n)/32)+1+(((n)/32)+1)*5/100,(((n)/32)+1+(((n)/32)+1)*5/100)/(float)n*100);
  printf("Sumas simbolos+pares=%u (%.6f%%)\n",m+2*(max_assigned-max_value),(m+2*(max_assigned-max_value))/(float)n*100);
  printf("Sumas simbolos+pares2=%u (%.6f%%)\n",m+symbols_new_len+aux,(m+symbols_new_len+aux)/(float)n*100);
}


void  new_value(uint *symbols_pair,uint *symbols_new_value,uint *k1,uint *j,uint pos) {
  uint izq,der;
  izq=symbols_pair[2*pos];
  der=symbols_pair[2*pos+1];

  if (izq>max_value) {
    izq=izq-max_value-1;
    if (symbols_new_value[izq] == 0) {
      symbols_new_value[izq]=*j; bitset(symbols_new_bit,*j-1); (*j)++;
      new_value(symbols_pair,symbols_new_value,k1,j,izq);
    } else {
      symbols_new[*k1]=symbols_new_value[izq]+max_value; (*j)++; (*k1)++;
    }
  } else {
    symbols_new[*k1]=izq;(*j)++;(*k1)++;
  }

  if (der>max_value) {
    der=der-max_value-1;
    if (symbols_new_value[der] == 0) {
      symbols_new_value[der]=*j; bitset(symbols_new_bit,*j-1); (*j)++;
      new_value(symbols_pair,symbols_new_value,k1,j,der);
    } else {
      symbols_new[*k1]=symbols_new_value[der]+max_value; (*j)++; (*k1)++;
    }
  } else {
    symbols_new[*k1]=der;(*j)++;(*k1)++;
  }

}

int main(int argc, char ** argv) {
	if(argc<2) {
		printf("Usage: %s <GRAPH>\n",argv[0]);
		return 0;
	}
	
	//** cargar de fichero .rp....
	
	char * grFile = new char[strlen(argv[1])+50];
	sprintf(grFile,"%s.rp",argv[1]);
	
	char * dictFile = new char[strlen(argv[1])+50];
	sprintf(dictFile,"%s.dict",argv[1]);
	
	FILE * grfp = fopen64(grFile,"r");
	fread(&min_value,sizeof(uint),1,grfp);
	fread(&max_value,sizeof(uint),1,grfp);
	fread(&max_assigned,sizeof(uint),1,grfp);
	fread(&m,sizeof(uint),1,grfp);
	fread(&n,sizeof(uint),1,grfp);
	uint nodes;
  fread(&nodes,sizeof(uint),1,grfp);
	printf("m=%u\n",++m);
	symbols = new uint[m];
  assert(symbols!=NULL);
	printf("read=%u\n",fread(symbols,sizeof(uint),m,grfp));
	fclose(grfp);

  printf("nodes=%u\n",nodes);
  printf("max_value=%u\nmax_assigned=%u\n",max_value,max_assigned);
  max_value-=nodes;
  max_assigned-=nodes;
  printf("max_value=%u\nmax_assigned=%u\n",max_value,max_assigned);


  //** cargar de fichero .dict...

	symbols_pair = (uint*)malloc(sizeof(uint)*2*(max_assigned-max_value));
  assert(symbols_pair!=NULL);
	FILE * dictfp = fopen64(dictFile,"r");
	for(uint i=0;i<(max_assigned-max_value);i++) {
		uint nv, p1, p2;
		if(fread(&nv,sizeof(uint),1,dictfp)==0) {
			printf("Error leyendo nvalue\n");
			return -1;
		}
		if(fread(&p1,sizeof(uint),1,dictfp)==0) {
			printf("Error leyendo p1\n");
			return -1;
		}
		if(fread(&p2,sizeof(uint),1,dictfp)==0) {
			printf("Error leyendo p2\n");
			return -1;
		}
    nv-=nodes; p1-=nodes; p2-=nodes;                //**  resta "nodes" a todas la reglas (al igual que se hizo a los datos en el fichero .rp antes de guardarlo)
		if(p1>=max_assigned || p2>=max_assigned || nv>max_assigned) {
			printf("Error, se ha leido lo siguiente:\n");
			printf("p1 = %u\np2 = %u\nnv = %u\nmax_value = %u\n",p1,p2,nv,max_value);
			return -1;
		}
		//if(i==1) printf("p1 = %u\np2 = %u\nnv = %u\nmax_value = %u\n",p1,p2,nv,max_value);
		if(nv!=(uint)(i+max_value+1)) printf("Something is wrong\n");
		symbols_pair[2*i]=p1;
		symbols_pair[2*i+1]=p2;
		//if(i%10000==0) printf("i=%u\n",i);
	}

	compress_pair_table();  //** !!!  Crea symbols_new, symbols_new_len 

	char * cdictFile = new char[strlen(argv[1])+50];
	sprintf(cdictFile,"%s.cdict",argv[1]);
	
	FILE * cdictfp = fopen64(cdictFile,"w");
	fwrite(&symbols_new_len,sizeof(uint),1,cdictfp);
	fwrite(symbols_new,sizeof(uint),symbols_new_len,cdictfp);
	printf("save BRR = %u\n",BRR->save(cdictfp));
	fclose(cdictfp);

	char * cpsiFile = new char[strlen(argv[1])+50];
	sprintf(cpsiFile,"%s.crp",argv[1]);

	nbits=bits(symbols_new_bit_len+max_value+1);
	FILE * cpsifp = fopen64(cpsiFile,"w");
	fwrite(&min_value,sizeof(uint),1,cpsifp);
	fwrite(&max_value,sizeof(uint),1,cpsifp);
	fwrite(&max_assigned,sizeof(uint),1,cpsifp);
	fwrite(&nbits, sizeof(uint),1,cpsifp);
	fwrite(&m,sizeof(uint),1,cpsifp);
	fwrite(&n,sizeof(uint),1,cpsifp);
  uint wr = 0;
  while((wr+=fwrite(symbols+wr,sizeof(uint),1,cpsifp))<(m/32*nbits+m%32*nbits/32+1));
  //printf("Writing: %u (m=%u nbits=%u) => %u\n",(m/32*nbits+m%32*nbits/32+1),m,nbits,wr);
	/*for(uint i=0;i<=m;i++) {
		fwrite(&symbols[i],sizeof(uint),1,cpsifp);
	}*/
	/*while((sum=fwrite(symbols+wr,sizeof(uint),1,cpsifp))&& wr<=m) {
		if(ferror(cpsifp)) {
			perror("fwrite()");
		}
		wr+=sum;
		if(wr%10000000==0)
			printf("write = %u\n",wr);
	}*/
	/*printf("written : %u\n",wr);*/
	printf("save BR = %u\n",BR->save(cpsifp));
	fclose(cpsifp);

	return 0;
}

#include <limits.h>
#include "icsa.h"

#define KEY(p)          (V[*(p)+(h)])
#define SWAP(p, q)      (tmp=*(p), *(p)=*(q), *(q)=tmp)
#define MED3(a, b, c)   (KEY(a)<KEY(b) ?                        \
        (KEY(b)<KEY(c) ? (b) : KEY(a)<KEY(c) ? (c) : (a))       \
        : (KEY(b)>KEY(c) ? (b) : KEY(a)>KEY(c) ? (c) : (a)))
	
static int *I,                  /* group array, ultimately suffix array.*/
   *V,                          /* inverse array, ultimately inverse of I.*/
   r,                           /* number of symbols aggregated by transform.*/
   h;                           /* length of already-sorted prefixes.*/
	
/* Subroutine for select_sort_split and sort_split. Sets group numbers for a
   group whose lowest position in I is pl and highest position is pm.*/

static void update_group(int *pl, int *pm)
{
   int g;

   g=pm-I;                      /* group number.*/
   V[*pl]=g;                    /* update group number of first position.*/
   if (pl==pm)
      *pl=-1;                   /* one element, sorted group.*/
   else
      do                        /* more than one element, unsorted group.*/
         V[*++pl]=g;            /* update group numbers.*/
      while (pl<pm);
}

/* Quadratic sorting method to use for small subarrays. To be able to update
   group numbers consistently, a variant of selection sorting is used.*/

static void select_sort_split(int *p, int n) {
   int *pa, *pb, *pi, *pn;
   int f, v, tmp;

   pa=p;                        /* pa is start of group being picked out.*/
   pn=p+n-1;                    /* pn is last position of subarray.*/
   while (pa<pn) {
      for (pi=pb=pa+1, f=KEY(pa); pi<=pn; ++pi)
         if ((v=KEY(pi))<f) {
            f=v;                /* f is smallest key found.*/
            SWAP(pi, pa);       /* place smallest element at beginning.*/
            pb=pa+1;            /* pb is position for elements equal to f.*/
         } else if (v==f) {     /* if equal to smallest key.*/
            SWAP(pi, pb);       /* place next to other smallest elements.*/
            ++pb;
         }
      update_group(pa, pb-1);   /* update group values for new group.*/
      pa=pb;                    /* continue sorting rest of the subarray.*/
   }
   if (pa==pn) {                /* check if last part is single element.*/
      V[*pa]=pa-I;
      *pa=-1;                   /* sorted group.*/
   }
}

/* Subroutine for sort_split, algorithm by Bentley & McIlroy.*/

static int choose_pivot(int *p, int n) {
   int *pl, *pm, *pn;
   int s;
   
   pm=p+(n>>1);                 /* small arrays, middle element.*/
   if (n>7) {
      pl=p;
      pn=p+n-1;
      if (n>40) {               /* big arrays, pseudomedian of 9.*/
         s=n>>3;
         pl=MED3(pl, pl+s, pl+s+s);
         pm=MED3(pm-s, pm, pm+s);
         pn=MED3(pn-s-s, pn-s, pn);
      }
      pm=MED3(pl, pm, pn);      /* midsize arrays, median of 3.*/
   }
   return KEY(pm);
}

/* Sorting routine called for each unsorted group. Sorts the array of integers
   (suffix numbers) of length n starting at p. The algorithm is a ternary-split
   quicksort taken from Bentley & McIlroy, "Engineering a Sort Function",
   Software -- Practice and Experience 23(11), 1249-1265 (November 1993). This
   function is based on Program 7.*/

static void sort_split(int *p, int n)
{
   int *pa, *pb, *pc, *pd, *pl, *pm, *pn;
   int f, v, s, t, tmp;

   if (n<7) {                   /* multi-selection sort smallest arrays.*/
      select_sort_split(p, n);
      return;
   }

   v=choose_pivot(p, n);
   pa=pb=p;
   pc=pd=p+n-1;
   while (1) {                  /* split-end partition.*/
      while (pb<=pc && (f=KEY(pb))<=v) {
         if (f==v) {
            SWAP(pa, pb);
            ++pa;
         }
         ++pb;
      }
      while (pc>=pb && (f=KEY(pc))>=v) {
         if (f==v) {
            SWAP(pc, pd);
            --pd;
         }
         --pc;
      }
      if (pb>pc)
         break;
      SWAP(pb, pc);
      ++pb;
      --pc;
   }
   pn=p+n;
   if ((s=pa-p)>(t=pb-pa))
      s=t;
   for (pl=p, pm=pb-s; s; --s, ++pl, ++pm)
      SWAP(pl, pm);
   if ((s=pd-pc)>(t=pn-pd-1))
      s=t;
   for (pl=pb, pm=pn-s; s; --s, ++pl, ++pm)
      SWAP(pl, pm);

   s=pb-pa;
   t=pd-pc;
   if (s>0)
      sort_split(p, s);
   update_group(p+s, p+n-t-1);
   if (t>0)
      sort_split(p+n-t, t);
}

/* Bucketsort for first iteration.

   Input: x[0...n-1] holds integers in the range 1...k-1, all of which appear
   at least once. x[n] is 0. (This is the corresponding output of transform.) k
   must be at most n+1. p is array of size n+1 whose contents are disregarded.

   Output: x is V and p is I after the initial sorting stage of the refined
   suffix sorting algorithm.*/
      
static void bucketsort(int *x, int *p, int n, int k)
{
   int *pi, i, c, d, g;

   for (pi=p; pi<p+k; ++pi)
      *pi=-1;                   /* mark linked lists empty.*/
   for (i=0; i<=n; ++i) {
      x[i]=p[c=x[i]];           /* insert in linked list.*/
      p[c]=i;
   }
   for (pi=p+k-1, i=n; pi>=p; --pi) {
      d=x[c=*pi];               /* c is position, d is next in list.*/
      x[c]=g=i;                 /* last position equals group number.*/
      if (d>=0) {               /* if more than one element in group.*/
         p[i--]=c;              /* p is permutation for the sorted x.*/
         do {
            d=x[c=d];           /* next in linked list.*/
            x[c]=g;             /* group number in x.*/
            p[i--]=c;           /* permutation in p.*/
         } while (d>=0);
      } else
         p[i--]=-1;             /* one element, sorted group.*/
   }
}

/* Transforms the alphabet of x by attempting to aggregate several symbols into
   one, while preserving the suffix order of x. The alphabet may also be
   compacted, so that x on output comprises all integers of the new alphabet
   with no skipped numbers.

   Input: x is an array of size n+1 whose first n elements are positive
   integers in the range l...k-1. p is array of size n+1, used for temporary
   storage. q controls aggregation and compaction by defining the maximum value
   for any symbol during transformation: q must be at least k-l; if q<=n,
   compaction is guaranteed; if k-l>n, compaction is never done; if q is
   INT_MAX, the maximum number of symbols are aggregated into one.
   
   Output: Returns an integer j in the range 1...q representing the size of the
   new alphabet. If j<=n+1, the alphabet is compacted. The global variable r is
   set to the number of old symbols grouped into one. Only x[n] is 0.*/

static int transform(int *x, int *p, int n, int k, int l, int q)
{
   int b, c, d, e, i, j, m, s;
   int *pi, *pj;
   
   for (s=0, i=k-l; i; i>>=1)
      ++s;                      /* s is number of bits in old symbol.*/
   e=INT_MAX>>s;                /* e is for overflow checking.*/
   for (b=d=r=0; r<n && d<=e && (c=d<<s|(k-l))<=q; ++r) {
      b=b<<s|(x[r]-l+1);        /* b is start of x in chunk alphabet.*/
      d=c;                      /* d is max symbol in chunk alphabet.*/
   }
   m=(1<<(r-1)*s)-1;            /* m masks off top old symbol from chunk.*/
   x[n]=l-1;                    /* emulate zero terminator.*/
   if (d<=n) {                  /* if bucketing possible, compact alphabet.*/
      for (pi=p; pi<=p+d; ++pi)
         *pi=0;                 /* zero transformation table.*/
      for (pi=x+r, c=b; pi<=x+n; ++pi) {
         p[c]=1;                /* mark used chunk symbol.*/
         c=(c&m)<<s|(*pi-l+1);  /* shift in next old symbol in chunk.*/
      }
      for (i=1; i<r; ++i) {     /* handle last r-1 positions.*/
         p[c]=1;                /* mark used chunk symbol.*/
         c=(c&m)<<s;            /* shift in next old symbol in chunk.*/
      }
      for (pi=p, j=1; pi<=p+d; ++pi)
         if (*pi)
            *pi=j++;            /* j is new alphabet size.*/
      for (pi=x, pj=x+r, c=b; pj<=x+n; ++pi, ++pj) {
         *pi=p[c];              /* transform to new alphabet.*/
         c=(c&m)<<s|(*pj-l+1);  /* shift in next old symbol in chunk.*/
      }
      while (pi<x+n) {          /* handle last r-1 positions.*/
         *pi++=p[c];            /* transform to new alphabet.*/
         c=(c&m)<<s;            /* shift right-end zero in chunk.*/
      }
   } else {                     /* bucketing not possible, don't compact.*/
      for (pi=x, pj=x+r, c=b; pj<=x+n; ++pi, ++pj) {
         *pi=c;                 /* transform to new alphabet.*/
         c=(c&m)<<s|(*pj-l+1);  /* shift in next old symbol in chunk.*/
      }
      while (pi<x+n) {          /* handle last r-1 positions.*/
         *pi++=c;               /* transform to new alphabet.*/
         c=(c&m)<<s;            /* shift right-end zero in chunk.*/
      }
      j=d+1;                    /* new alphabet size.*/
   }
   x[n]=0;                      /* end-of-string symbol is zero.*/
   return j;                    /* return new alphabet size.*/
}

/* Makes suffix array p of x. x becomes inverse of p. p and x are both of size
   n+1. Contents of x[0...n-1] are integers in the range l...k-1. Original
   contents of x[n] is disregarded, the n-th symbol being regarded as
   end-of-string smaller than all other symbols.*/

void suffixsort(int *x, int *p, int n, int k, int l)
{
   int *pi, *pk;
   int i, j, s, sl;

   V=x;                         /* set global values.*/
   I=p;
   
   if (n>=k-l) {                /* if bucketing possible,*/
      j=transform(V, I, n, k, l, n);
      bucketsort(V, I, n, j);   /* bucketsort on first r positions.*/
   } else {
      transform(V, I, n, k, l, INT_MAX);
      for (i=0; i<=n; ++i)
         I[i]=i;                /* initialize I with suffix numbers.*/
      h=0;
      sort_split(I, n+1);       /* quicksort on first r positions.*/
   }
   h=r;                         /* number of symbols aggregated by transform.*/
   
   while (*I>=-n) {
      pi=I;                     /* pi is first position of group.*/
      sl=0;                     /* sl is negated length of sorted groups.*/
      do {
         if ((s=*pi)<0) {
            pi-=s;              /* skip over sorted group.*/
            sl+=s;              /* add negated length to sl.*/
         } else {
            if (sl) {
               *(pi+sl)=sl;     /* combine sorted groups before pi.*/
               sl=0;
            }
            pk=I+V[s]+1;        /* pk-1 is last position of unsorted group.*/
            sort_split(pi, pk-pi);
            pi=pk;              /* next group.*/
         }
      } while (pi<=I+n);
      if (sl)                   /* if the array ends with a sorted group.*/
         *(pi+sl)=sl;           /* combine sorted groups at end of I.*/
      h=2*h;                    /* double sorted-depth.*/
   }

   for (i=0; i<=n; ++i)         /* reconstruct suffix array from inverse.*/
      I[V[i]]=i;
}


//ticsa *createIntegerCSA(uint **intVector, uint textSize, char *build_options) {
int buildIntIndex (uint *intVector, uint n, char *build_options, void **index ){
	uint textSize=n;
	ticsa *myicsa;
	myicsa = (ticsa *) malloc (sizeof (ticsa));
	uint *Psi, *SAI, *C, vocSize;
	register uint i, j;
	uint nsHUFF;

	parametersCSA(myicsa, build_options);
	
	nsHUFF=myicsa->tempNSHUFF;
	
	// Almacenamos o valor dalguns parametros
	
	myicsa->suffixArraySize = textSize;
	myicsa->D = (uint*) malloc (sizeof(uint) * ((textSize+31)/32));	
	
	myicsa->samplesASize = (textSize + myicsa->T_A - 1) / myicsa->T_A + 1; //--> última pos siempre sampleada!
//	myicsa->samplesASize = (textSize + myicsa->T_A - 1) / myicsa->T_A ;//+ 1;
	myicsa->samplesA = (uint *)malloc(sizeof(uint) * myicsa->samplesASize);
	myicsa->samplesA[myicsa->samplesASize-1] = 0000;
	myicsa->BA = (uint*) malloc (sizeof(uint) * ((textSize+31)/32));
	myicsa->samplesAInvSize = (textSize + myicsa->T_AInv - 1) / myicsa->T_AInv;
	myicsa->samplesAInv = (uint *)malloc(sizeof(int) * myicsa->samplesAInvSize);

	// CONSTRUIMOS A FUNCION C
	vocSize = 0;
	for(i=0;i<textSize;i++) if(intVector[i]>vocSize) vocSize = intVector[i];
	C = (uint *) malloc(sizeof(uint) * (vocSize + 1));	// Para contar o 0 terminador
	for(i=0;i<vocSize;i++) C[i] = 0;
	for(i=0;i<textSize;i++) C[intVector[i]]++;
	for (i=0,j=0;i<=vocSize;i++) {
		j = j + C[i];
		C[i] = j;
	}
	for(i=vocSize;i>0;i--) C[i] = C[i-1];
	C[0] = 0;

	// Reservamos espacio para os array de sufijos
	Psi = (uint *) malloc (sizeof(uint) * (textSize+1));
	printf("\n\t *BUILDING THE SUFFIX ARRAY over %d integers... (with larsson-sadakane)", textSize);fflush(stdout);
	
/* Makes suffix array p of x. x becomes inverse of p. p and x are both of size
   n+1. Contents of x[0...n-1] are integers in the range l...k-1. Original
   contents of x[n] is disregarded, the n-th symbol being regarded as
   end-of-string smaller than all other symbols.*/
   //void suffixsort(int *x, int *p, int n, int k, int l)
   	
	//suffixsort((int *)intVector, (int *)Psi, n, vocSize+1, 1);
	suffixsort((int *)intVector, (int *)Psi, n-1, vocSize+1, 1);
	//*Psi = *Psi++; 	// Con esto temos o array de sufixos desde a primeira posición (sen o terminador)
	printf("\n\t ...... ended.");



	// CONSTRUIMOS A INVERSA DO ARRAY DE SUFIXOS
	//SAI = (uint *) malloc (sizeof(uint) * (textSize + 1));	// +1 para repetir na ultima posición. Evitamos un if
	//for(i=0;i<textSize;i++) SAI[Psi[i]] = i;
	//SAI[textSize] = SAI[0];
	
	//SAI = intVector;
	
	SAI = (uint *) malloc (sizeof(uint) * (textSize + 1));	// +1 para repetir na ultima posición. Evitamos un if
	for(i=0;i<textSize;i++) SAI[i] = intVector[i];
	SAI[textSize] = SAI[0];	

	//SAI[textSize] = SAI[0];

	// ALMACENAMOS AS MOSTRAS DO ARRAY DE SUFIXOS
	for(i=0;i<((textSize+31)/32);i++) myicsa->BA[i] = 0;
	for(i=0; i<textSize; i+=myicsa->T_A) bitset(myicsa->BA, SAI[i]);
	bitset(myicsa->BA, SAI[textSize-1]);			// A ultima posicion sempre muestreada
	//printf("TextSize = %d\n", textSize);
	myicsa->bBA = createBitmap(myicsa->BA, textSize);
	for(i=0,j=0; i<textSize; i++) if(bitget(myicsa->BA, i)) myicsa->samplesA[j++] = Psi[i];
	
	// ALMACENAMOS AS MOSTRAS DA INVERSA DO ARRAY DE SUFIXOS
	for(i=0,j=0;i<textSize;i+=myicsa->T_AInv) myicsa->samplesAInv[j++] = SAI[i];
	
	// CONSTRUIMOS E COMPRIMIMOS PSI
	printf("\n\t Creating compressed Psi...");
	for(i=0;i<textSize;i++) Psi[i] = SAI[Psi[i]+1];
	
	//FILE *ff = fopen("psi.log","w");
	//for (i=0;i<textSize;i++) fprintf(ff,"%d::%u",i,Psi[i]);
	//fclose(ff);
	
	free(SAI);
	
	
	#ifdef PSI_HUFFMANRLE	
	myicsa->hcPsi = huffmanCompressPsi(Psi,textSize,myicsa->T_Psi,nsHUFF);
	#endif				
	#ifdef PSI_GONZALO	
	myicsa->gcPsi = gonzaloCompressPsi(Psi,textSize,myicsa->T_Psi,nsHUFF);
	#endif			
	#ifdef PSI_DELTACODES
	myicsa->dcPsi = deltaCompressPsi(Psi,textSize,myicsa->T_Psi);		
	#endif
  	
  	free(Psi);	
		
	// Contruimos D
	for(i=0;i<((textSize+31)/32);i++) myicsa->D[i] = 0;	
	for(i=0;i<=vocSize;i++) bitset(myicsa->D, C[i]);
	myicsa->bD = createBitmap(myicsa->D,textSize);
	free(C);	

	// VARIABLE GLOBAL QUE ALMACENA O ESTADO DOS DISPLAYS (IMPORTANTE PARA DISPLAY SECUENCIAL)
	// Almacena a última posición do array de sufixos que mostramos con display ou displayNext
	// Se nos piden un displayNext, aplicamos PSI sobre esta posición e obtemos a seguinte,
	// coa que podemos obter o símbolo pedido, e actualizamos displayState
	myicsa->displayCSAState = 0;
	myicsa->displayCSAPrevPosition = -2;  //needed by DisplayCSA(position)
	
	//aintVector = intVector;
	// Liberamos o espacio non necesario

	*index = myicsa;
	//return (myicsa);
	return 0;
}


//Returns number of elements in the indexed sequence of integers
int sourceLenIntIndex(void *index, uint *numInts){
	ticsa *myicsa = (ticsa *) index;
	*numInts= myicsa->suffixArraySize;
	return 0; //no error;
}

int saveIntIndex(void *index, char *pathname) {
//void storeStructsCSA(ticsa *myicsa, char *basename) {
  
 	ticsa *myicsa = (ticsa *) index; 
 	char *basename=pathname;

	char *filename;
	int file;
	
	// Reservamos espacio para o nome do ficheiro
	filename = (char *)malloc(sizeof(char)*MAX_FILENAME_LENGTH);
		
	// Ficheiro co n�mero de elementos indexados (enteiros do texto orixinal)
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, NUMBER_OF_ELEMENTS_FILE_EXT);
	unlink(filename);
	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	write(file, &(myicsa->suffixArraySize), sizeof(int));
	close(file);		

	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, PSI_COMPRESSED_FILE_EXT);	

	#ifdef PSI_HUFFMANRLE
		storeHuffmanCompressedPsi(&(myicsa->hcPsi), filename);	
	#endif	
	#ifdef PSI_GONZALO
		storeGonzaloCompressedPsi(&(myicsa->gcPsi), filename);	
	#endif
	#ifdef PSI_DELTACODES
		storeDeltaCompressedPsi(&(myicsa->dcPsi), filename);
	#endif
	
	// Ficheiro co vector de bits D
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, D_FILE_EXT);  
	unlink(filename);
	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	write(file, myicsa->D, sizeof(int)*((myicsa->suffixArraySize+31)/32));
	close(file);

	// Directorio de rank para D
	// Almacenamos o n�mero de superbloques seguido dos superbloques
	// E logo o n�mero de bloques seguido dos bloques
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, D_RANK_DIRECTORY_FILE_EXT);
	saveBitmap(filename,myicsa->bD);
	
	// Ficheiro coas mostras de A
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, SAMPLES_A_FILE_EXT); 
	unlink(filename);
	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	write(file, myicsa->samplesA, sizeof(int) * (myicsa->samplesASize));
	close(file);

	// Ficheiro co vector BA (marca as posicions de A muestreadas)
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, BA_FILE_EXT);  
	unlink(filename);
	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	write(file, myicsa->BA, sizeof(int)*((myicsa->suffixArraySize+31)/32));
	close(file);

	// Directorio de rank para BA
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, BA_RANK_DIRECTORY_FILE_EXT);
	saveBitmap(filename, myicsa->bBA);
	
	// Ficheiro coas mostras de A inversa
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, SAMPLES_A_INV_FILE_EXT);
	unlink(filename);
	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	write(file, myicsa->samplesAInv, sizeof(int) * (myicsa->samplesAInvSize));
	close(file);	

	// Ficheiro co periodo de muestreo de A e A inversa
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, SAMPLING_PERIOD_A_FILE_EXT);
	unlink(filename);
	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	write(file, &(myicsa->T_A), sizeof(int));
	write(file, &(myicsa->T_AInv), sizeof(int));
	
	write(file, &(myicsa->psiSearchFactorJump), sizeof(uint));

	close(file);
	free(filename);	
	return 0; //no error.
}

//Returns the size (in bytes) of the index over the sequence of integers.
//uint CSA_size(ticsa *myicsa) {
int sizeIntIndex(void *index, uint *numBytes) {
	ticsa *myicsa = (ticsa *) index;
	uint size = 0;
	size +=(sizeof (ticsa) * 1);
	size += sizeof(uint)*((myicsa->suffixArraySize+31)/32) ;  //D vector
	size += myicsa->bD->mem_usage;
	size += sizeof(uint) * myicsa->samplesASize ;  // samples A
	size += sizeof(uint) * myicsa->samplesAInvSize ;  // samples A^{-1}
	size += sizeof(uint)*((myicsa->suffixArraySize+31)/32) ;  //BA vector
	size += myicsa->bBA->mem_usage;
	#ifdef PSI_HUFFMANRLE
		size +=myicsa->hcPsi.totalMem;	 	
	#endif	
	#ifdef PSI_GONZALO
		size +=myicsa->gcPsi.totalMem;	 	
	#endif
	#ifdef PSI_DELTACODES
		size +=myicsa->dcPsi.totalMem;
	#endif	
	*numBytes = size;
	return 0; //no error.
}


//ticsa *loadCSA(char *basename) {
int loadIntIndex(char *pathname, void **index){

	char *basename=pathname;
	char *filename;
	int file;
	uint length;
	char c;
	char *word;
	struct stat f_stat;	
	uint suffixArraySize;

	ticsa *myicsa;
	myicsa = (ticsa *) malloc (sizeof (ticsa) * 1);
	
	
	// VARIABLE GLOBAL QUE ALMACENA O ESTADO DOS DISPLAYS (IMPORTANTE PARA DISPLAY SECUENCIAL)
	// Almacena a �ltima posici�n do array de sufixos que mostramos con display ou displayNext
	// Se nos piden un displayNext, aplicamos PSI sobre esta posici�n e obtemos a seguinte,
	// coa que podemos obter o s�mbolo pedido, e actualizamos displayState
	myicsa->displayCSAState = 0;
	myicsa->displayCSAPrevPosition = -2;  //needed by DisplayCSA(position)
	
	// Reservamos espacio para o nome do ficheiro
	filename = (char *)malloc(sizeof(char)*MAX_FILENAME_LENGTH);

	// LEEMOS OS DATOS DO FICHEIRO QUE ALMACENA O NUMERO DE ELEMENTOS INDEXADOS
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, NUMBER_OF_ELEMENTS_FILE_EXT);
	if( (file = open(filename, O_RDONLY)) < 0) { 
		printf("Cannot read file %s\n", filename);exit(0);
	}	
	read(file, &suffixArraySize, sizeof(uint));		
	myicsa->suffixArraySize = suffixArraySize;
	printf("Number of indexed elements (suffix array size) = %d\n", suffixArraySize);
	
	// LEEMOS OS DATOS DO FICHEIRO QUE ALMACENA PSI COMPRIMIDA	
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, PSI_COMPRESSED_FILE_EXT);		
	#ifdef PSI_HUFFMANRLE
		myicsa->hcPsi = loadHuffmanCompressedPsi(filename);	 	
	#endif	
	#ifdef PSI_GONZALO
		myicsa->gcPsi = loadGonzaloCompressedPsi(filename);	 	
	#endif
	#ifdef PSI_DELTACODES
		myicsa->dcPsi = loadDeltaCompressedPsi(filename);
	#endif	 
	
	// LEEMOS OS DATOS DO FICHEIRO QUE ALMACENA D
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, D_FILE_EXT);
	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot read file %s\n", filename); exit(0);
	}	
	myicsa->D = (uint *) malloc (sizeof(uint)*((suffixArraySize+31)/32));
	read(file, myicsa->D, sizeof(uint)*((suffixArraySize+31)/32));
	printf("Bit vector D loaded (%d bits)\n", suffixArraySize);

	// LEEMOS OS DATOS DO FICHEIRO QUE ALMACENA O DIRECTORIO DE RANK1 PARA D
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, D_RANK_DIRECTORY_FILE_EXT);				
	myicsa->bD = loadBitmap(filename,myicsa->D,suffixArraySize);
	{	uint ns, nb;		
		ns = myicsa->bD->sSize;
		nb = myicsa->bD->bSize;
		myicsa->bD->data = myicsa->D;
		printf("Rank1 Directory for D loaded (%d superblocks, %d blocks)\n", ns, nb);	
	}
	
	// LEEMOS OS DATOS DO FICHEIRO QUE ALMACENA SAMPLES A
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, SAMPLES_A_FILE_EXT);
	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot read file %s\n", filename); exit(0);
	}
	if( fstat(file, &f_stat) < 0) {
		printf("Cannot read information from file %s\n", filename);	exit(0);
	}	
	myicsa->samplesASize = (f_stat.st_size)/sizeof(uint);
	myicsa->samplesA = (uint *)malloc(sizeof(uint) * myicsa->samplesASize);
	read(file, myicsa->samplesA, sizeof(uint) * myicsa->samplesASize);		
	printf("Suffix array samples loaded (%d samples)\n", myicsa->samplesASize);	
	
	// LEEMOS OS DATOS DO FICHEIRO QUE ALMACENA BA
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, BA_FILE_EXT);
	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot read file %s\n", filename); exit(0);
	}	
	myicsa->BA = (uint *) malloc (sizeof(uint)*((suffixArraySize+31)/32));
	read(file, myicsa->BA, sizeof(uint)*((suffixArraySize+31)/32));
	printf("Bit vector BA loaded (%d bits)\n", suffixArraySize);

	// LEEMOS OS DATOS DO FICHEIRO QUE ALMACENA O DIRECTORIO DE RANK1 PARA BA
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, BA_RANK_DIRECTORY_FILE_EXT);				
	myicsa->bBA = loadBitmap(filename,myicsa->BA,suffixArraySize);
	{	uint ns, nb;		
		ns = myicsa->bBA->sSize;
		nb = myicsa->bBA->bSize;
		myicsa->bBA->data = myicsa->BA;
		printf("Rank1 Directory for BA loaded (%d superblocks, %d blocks)\n", ns, nb);	
	}

	// LEEMOS OS DATOS DO FICHEIRO QUE ALMACENA SAMPLES A INVERSA
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, SAMPLES_A_INV_FILE_EXT);
	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot read file %s\n", filename); exit(0);
	}
	if( fstat(file, &f_stat) < 0) {
		printf("Cannot read information from file %s\n", filename);	exit(0);
	}	
	myicsa->samplesAInvSize = (f_stat.st_size)/(sizeof(uint));
	myicsa->samplesAInv = (uint *)malloc(sizeof(uint) * myicsa->samplesAInvSize);
	read(file, myicsa->samplesAInv, sizeof(uint) * myicsa->samplesAInvSize);		
	printf("Suffix array inverse samples loaded (%d samples)\n", myicsa->samplesAInvSize);
	
	// LEEMOS OS DATOS DO FICHEIRO QUE ALMACENA O PERIODO DE MUESTREO DO ARRAY DE SUFIXOS E DA INVERSA
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, SAMPLING_PERIOD_A_FILE_EXT);
	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot read file %s\n", filename); exit(0);
	}	
	read(file, &(myicsa->T_A), sizeof(uint));
	read(file, &(myicsa->T_AInv), sizeof(uint));	
	printf("Sampling A Period T = %d, Sampling A inv Period TInv = %d\n", myicsa->T_A, myicsa->T_AInv);	
	
	read(file, &(myicsa->psiSearchFactorJump), sizeof(uint));
	printf("Psi Bin Search Factor-Jump is = %d\n", myicsa->psiSearchFactorJump);	
	
	close(file);
	free(filename);

	//return myicsa;
	*index = myicsa;
	return 0;
}
	

//uint destroyStructsCSA(ticsa *myicsa) {	
int freeIntIndex(void *index) {	
	ticsa *myicsa = (ticsa *) index;
		// Liberamos o espacio reservado
		
	if (!myicsa) return 0;
	
	size_t total=0, totaltmp=0;
	
	uint nbytes;sizeIntIndex(index, &nbytes);
		
	total +=(sizeof (ticsa) * 1);;
	
	#ifdef PSI_HUFFMANRLE
		total+= totaltmp = myicsa->hcPsi.totalMem;
		destroyHuffmanCompressedPsi(&(myicsa->hcPsi));
	#endif
	#ifdef PSI_GONZALO
		total+= totaltmp = myicsa->gcPsi.totalMem;
		destroyGonzaloCompressedPsi(&(myicsa->gcPsi));
	#endif
	#ifdef PSI_DELTACODES
		total+= totaltmp = myicsa->dcPsi.totalMem;
		destroyDeltaCodesCompressedPsi(&(myicsa->dcPsi));	
	#endif
	printf("\n\t[destroying  SA: compressed PSI structure] ...Freed %zu bytes... RAM", totaltmp);
	
	free(myicsa->D);			total+= totaltmp =  (sizeof(uint)*((myicsa->suffixArraySize+31)/32)); 
							printf("\n\t[destroying  SA: D vector] ...Freed %zu bytes... RAM",totaltmp);
	free(myicsa->samplesA);		total+= totaltmp = (sizeof(uint) * myicsa->samplesASize); 
							printf("\n\t[destroying  Samples A: A   ] ...Freed %zu bytes... RAM",totaltmp);
	free(myicsa->samplesAInv);	total+= totaltmp =  (sizeof(uint) * myicsa->samplesAInvSize); 
							printf("\n\t[destroying  Samples AInv: A   ] ...Freed %zubytes... RAM",totaltmp);							
						printf("\n\t[destroying  rank bit D   ] ...Freed %zu bytes... RAM", (size_t)myicsa->bD->mem_usage);
	free(myicsa->BA);			total+= totaltmp =  (sizeof(uint)*((myicsa->suffixArraySize+31)/32)); 
							printf("\n\t[destroying  SA: BA vector] ...Freed %zu bytes... RAM",totaltmp);
								
								total += myicsa->bD->mem_usage;
	destroyBitmap(myicsa->bD);			
								total += myicsa->bBA->mem_usage;
	destroyBitmap(myicsa->bBA);
								
	printf("\n\t**** [the whole iCSA ocuppied ... %zu bytes... RAM",total);
	printf("\n\t**** iCSA size = %zu bytes ", (size_t) nbytes);
	printf("\n");
	
	free(myicsa);
	
	return 0; //no error.
}

	// Shows detailed summary info of the self-index (memory usage of each structure)
int printInfoIntIndex(void *index, const char tab[]) {
	ticsa *myicsa = (ticsa *) index;
	if (!myicsa) return 0;
	
	uint structure, totalpsi, totalD, totalBD, totalSA,totalSAinv, totalBA,totalBBA;
	
	structure=sizeof(ticsa);
	
	#ifdef PSI_HUFFMANRLE
		totalpsi = myicsa->hcPsi.totalMem;
	#endif
	#ifdef PSI_GONZALO
		totalpsi = myicsa->gcPsi.totalMem;
	#endif
	#ifdef PSI_DELTACODES
		totalpsi = myicsa->dcPsi.totalMem;
	#endif

	totalD   = (sizeof(uint)*((myicsa->suffixArraySize+31)/32));
	totalBD  = myicsa->bD->mem_usage;
	totalSA  = (sizeof(uint) * myicsa->samplesASize); 
	totalSAinv = (sizeof(uint) * myicsa->samplesAInvSize); 
	totalBA  = (sizeof(uint)*((myicsa->suffixArraySize+31)/32));
	totalBBA = myicsa->bBA->mem_usage;
	
	uint nbytes; sizeIntIndex(index, &nbytes); //whole self-index
	
	printf("\n ===================================================:");		
	printf("\n%sSummary Self-index on integers (icsa) layer:",tab);		
	printf("\n%s   icsa structure = %d bytes",tab, structure);
	printf("\n%s   psi         = %8d bytes",tab, totalpsi);
	printf("\n%s   D (bitmap)  = %8d bytes",tab, totalD);
	printf("\n%s   rank for D  = %8d bytes",tab, totalBD);
	printf("\n%s   SA(sampled) = %8d bytes",tab, totalSA);
	printf("\n%s   SAinv(samp) = %8d bytes",tab, totalSAinv);
	printf("\n%s   BA (bitmap) = %8d bytes",tab, totalBA);
	printf("\n%s   rank for BA = %8d bytes",tab, totalBBA);
	printf("\n%sTotal = ** %9d bytes (in RAM) ** ",tab, nbytes);
	printf("\n");
	
	return 0; //no error.
}


// OPERACIONS DO CSA

// BUSCA BINARIA SOBRE MOSTRAS + 2 BUSCAS EXPONENCIAIS + 2 BUSCAS BINARIAS
// 1º Busca binaria sobre o array de sufixos, elexindo como pivote un múltiplo de bin_search_psi_skip_interval (que orixinalmente foi pensado para igualar co valor de Psi).
// 2º Esta busca pode deterse por:
//	a) O pivote repítese entre dúas iteracións -> As ocorrencias están entre o pivote e a seguinte mostra (pivote + bin_search_psi_skip_interval) -> facemos dúas buscas binarias
//	b) O pivote é unha ocorrencia do patrón -> Faise unha busca exponencial sobre mostras hacia a esquerda e outra hacia a dereita, ata atopar a unha mostra á esquerda e outra
//	á dereita do intervalo de ocorrencias. Entre cada unha destas e a inmediatamente anterior da busca exponencial, faise unha busca binaria para atopar os extremos do intervalo.

int countIntIndex(void *index, uint *pattern, uint length, ulong *numocc, ulong *left, ulong *right){	
	//unsigned int countCSA(ticsa *myicsa, uint *pattern, uint patternSize, uint *left, uint *right) {

	uint patternSize = length;
	ticsa *myicsa = (ticsa *) index;
	
	register unsigned long l, r, i;
	register long comp, p, previousP;
	//register unsigned int l, r, i;
	//register int comp, p, previousP;
	register uint bin_search_psi_skip_interval = myicsa->psiSearchFactorJump;
	
	//fprintf(stderr,"\n psiSearchFactor = %d",myicsa->psiSearchFactorJump);
	
	l = 0; 
	r = (myicsa->suffixArraySize+bin_search_psi_skip_interval-2)/bin_search_psi_skip_interval*bin_search_psi_skip_interval;
	p = ((l+r)/2)/bin_search_psi_skip_interval * bin_search_psi_skip_interval;
	previousP = 0;
	
	// BUSCA BINARIA SOBRE MOSTRAS
	while( (p != previousP) && (comp = SadCSACompare(myicsa, pattern, patternSize, p)) ) {
		if(comp > 0) l = p;
		else r = p;
		previousP = p;
		p = ((l+r)/2)/bin_search_psi_skip_interval*bin_search_psi_skip_interval;
	}

	if(p==previousP) {
	
		// BUSCA BINARIA ENTRE O PIVOTE E A SEGUINTE MOSTRA
		l = previousP; 
		r = previousP+bin_search_psi_skip_interval;
		if(r > myicsa->suffixArraySize) r = myicsa->suffixArraySize - 1;
		while(l < r) {
			p = (l+r)/2; 
			if(SadCSACompare(myicsa, pattern, patternSize, p) <= 0) r = p;
			else l = p+1;
		}

		if(SadCSACompare(myicsa, pattern, patternSize, r)) {
			*left = l;
			*right = r;
			//return 0;
			*numocc = 0;
			return 0; //no error.
		}
		*left = r;
	
		l = previousP; 
		r = previousP+bin_search_psi_skip_interval;
		if(r > myicsa->suffixArraySize) r = myicsa->suffixArraySize - 1;
		while(l < r) {
			p = (l+r+1)/2;
			if(SadCSACompare(myicsa, pattern, patternSize, p) >= 0) l = p;
			else r = p-1;	
		}
		*right = l;
		
	} else {
		
		previousP = p;	// En previousP poñemos o p atopado na busca sobre as mostras
		
		// BUSCA EXPONENCIAL HACIA ATRÁS
		i = 1;
		p -= bin_search_psi_skip_interval;
		while(p>0 && !SadCSACompare(myicsa, pattern, patternSize, p)) {
			i<<=1;
			p = previousP-(i*bin_search_psi_skip_interval);
		}
		// Busca binaria entre as duas ultimas mostras da exponencial
		if(previousP > i*bin_search_psi_skip_interval) l = previousP-(i*bin_search_psi_skip_interval);
		else l=0;
		i>>=1;		
		r = previousP-(i*bin_search_psi_skip_interval);
		while(l < r) {
			p = (l+r)/2; 
			if(SadCSACompare(myicsa, pattern, patternSize, p) <= 0) r = p;
			else l = p+1;
		}
		*left = r;
		
		// BUSCA EXPONENCIAL HACIA ADIANTE
		i = 1;
		p = previousP+bin_search_psi_skip_interval;
		while(p<myicsa->suffixArraySize && !SadCSACompare(myicsa, pattern, patternSize, p)) {
			i<<=1;
			p = previousP+(i*bin_search_psi_skip_interval);		
		}
		// Busca binaria entre as duas ultimas mostras da exponencial
		if(p < myicsa->suffixArraySize) r = previousP+(i*bin_search_psi_skip_interval);
		else r = myicsa->suffixArraySize-1;
		i>>=1;		
		l = previousP+(i*bin_search_psi_skip_interval);
		while(l < r) {
			p = (l+r+1)/2;
			if(SadCSACompare(myicsa, pattern, patternSize, p) >= 0) l = p;
			else r = p-1;	
		}
		*right = l;	
	}

	//return *right-*left+1;
	*numocc = (uint) *right-*left+1;
	return 0; //no error		
}

// Version inicial de busca binaria
unsigned int countCSABin(ticsa *myicsa, uint *pattern, uint patternSize, uint *left, uint *right) {
	register ulong l, r, p;

	l = 0; 
	r = myicsa->suffixArraySize-1;

	while(l < r) {
		p = (l+r)/2; 
		if(SadCSACompare(myicsa, pattern, patternSize, p) <= 0) r = p;
		else l = p+1;
	}

	// SE SON DISTINTOS O PATRON NON APARECE NO TEXTO E DEVOLVEMOS 0
	if(SadCSACompare(myicsa, pattern, patternSize, r)) {
		*left = l;
		*right = r;
		return 0;
	}
	
	// Almacenamos o limite esquerdo
	*left = r;

	// SE SON IGUALES (O PATRON APARECE NO TEXTO), BUSCAMOS AGORA O LIMITE DEREITO, QUE ALMACENAREMOS EN right
	// NOTA: INICIAMOS A BUSQUEDA A PARTIR DO ESQUERDO...
	l = r; 
	r = myicsa->suffixArraySize-1;
	
	while(l < r) {
		p = (l+r+1)/2;
		if(SadCSACompare(myicsa, pattern, patternSize, p) >= 0) l = p;
		else r = p-1;	
	}
	
	// Gardamos o limite dereito
	*right = l;
	
	return (uint) *right-*left+1;	
}

int locateIntIndex(void *index, uint *pattern, uint length, ulong **occ, ulong *numocc) {
	//unsigned int *locateCSA(ticsa *myicsa, uint *pattern, uint patternSize, uint *occ) {
	
	ticsa *myicsa = (ticsa *) index;
	uint patternSize = length;
	ulong *positions;
	ulong occurrences;
	register ulong left, right;
	
		//occurrences = countCSA(myicsa, pattern, patternSize, &left, &right);
	int err;
	err = countIntIndex((void *) myicsa, pattern, patternSize, &occurrences, &left, &right);
	*numocc = occurrences;

	if (occurrences) {
		register ulong idx = 0;
		positions = (ulong*) malloc(sizeof(ulong) * occurrences);
		while(left<=right) positions[idx++] = A(myicsa,left++);	
		
		*occ = positions;
		return 0;	
		//return positions;
	}
	
	(*occ)=NULL;
	return 0; //no error, but no occurrences.
	
	//return NULL;
}

// Devolve o enteiro do texto que ocupa a posicion dada,
// e fixa o estado para poder seguir obtendo os seguintes enteiros con displayNext();

int displayIntIndex(void *index, ulong position, uint *value){
	//uint displayCSA(ticsa *myicsa, uint position) {
	ticsa *myicsa = (ticsa *) index;
	if (position == (myicsa->displayCSAPrevPosition +1)) {
		myicsa->displayCSAPrevPosition = position;
		//return displayCSANext(myicsa);
		*value = displayCSANext(myicsa);
	}
	else {
		myicsa->displayCSAPrevPosition = position;
		//return displayCSAFirst(myicsa, position);
		*value = displayCSAFirst(myicsa, position);
	}
	return 0; //no error
}


/**********************************************************************/

// Devolve o enteiro do texto que ocupa a posicion dada, e fixa o estado 
// para poder seguir obtendo os seguintes enteiros con displayNext();
uint displayCSAFirst(ticsa *myicsa, uint position) {

	register uint positionAux, index;
	register uint T_AInv = myicsa->T_AInv;
	
	positionAux = myicsa->samplesAInv[position / T_AInv];
	for(index = 0; index < position%T_AInv; index++) {
		#ifdef PSI_HUFFMANRLE
			positionAux=getHuffmanPsiValue(&(myicsa->hcPsi),positionAux);
		#endif			 
		#ifdef PSI_GONZALO
			positionAux=getGonzaloPsiValue(&(myicsa->gcPsi),positionAux);
		#endif
		#ifdef PSI_DELTACODES
			positionAux=getDeltaPsiValue(&(myicsa->dcPsi),positionAux);
		#endif		
	}
	
	// Fijamos a variable global para a chamada a displayNext
	myicsa->displayCSAState = positionAux;
	
	//	return rank1(D, Dir, positionAux) - 1;
	return rank(myicsa->bD, positionAux) - 1;
}


// Devolve o seguinte enteiro do texto (a partir do estado)
unsigned int displayCSANext(ticsa *myicsa) {	
	#ifdef PSI_HUFFMANRLE
		myicsa->displayCSAState=getHuffmanPsiValue(&(myicsa->hcPsi),myicsa->displayCSAState);
	#endif		 
	#ifdef PSI_GONZALO
		myicsa->displayCSAState = getGonzaloPsiValue(&(myicsa->gcPsi),myicsa->displayCSAState);
	#endif
	#ifdef PSI_DELTACODES
		myicsa->displayCSAState = getDeltaPsiValue(&(myicsa->dcPsi),myicsa->displayCSAState);
	#endif	
	return rank(myicsa->bD, myicsa->displayCSAState) - 1;	
}


// Mostra as estructuras creadas
void showStructsCSA(ticsa *myicsa) {

	unsigned int index;

	// ESTRUCTURAS PARA CSA
	printf("Basic CSA structures:\n\n");
	
	// VALORES DA FUNCI�N PSI (decodificando)
	printf("\tPSI: (Sampling period = %d)\n", myicsa->T_Psi);
	for(index=0; index < myicsa->suffixArraySize; index++)	 
		#ifdef PSI_HUFFMANRLE
			printf("\tPsi[%d] = %d\n", index, getHuffmanPsiValue(&(myicsa->hcPsi),index));
		#endif
		#ifdef PSI_GONZALO
			printf("\tPsi[%d] = %d\n", index, getGonzaloPsiValue(&(myicsa->gcPsi),index));
		#endif
		#ifdef PSI_DELTACODES
			printf("\tPsi[%d] = %d\n", index, getDeltaPsiValue(&(myicsa->dcPsi),index));		
		#endif				
	printf("\n");
	
	// VECTOR D DE SADAKANE CO DIRECTORIO DE RANK ASOCIADO
	printf("\tD = ");
	showBitVector(myicsa->D, myicsa->suffixArraySize);
	printf("\n\nSuperbloques de D:\n");
	{	uint ns;
		uint nb;
		ns = myicsa->bD->sSize;
		nb= myicsa->bD->bSize;
		for(index=0; index<ns; index++) {
			//printf("\tDs[%d] = %d\n", index, Dir.Ds[index]);
			printf("\tDs[%d] = %d\n", index, myicsa->bD->sdata[index]);
		}
		printf("\nBloques de D:\n");
		
		for(index=0; index<nb; index++) {
			//printf("\tDb[%d] = %d\n", index, Dir.Db[index]);
			printf("\tDb[%d] = %d\n", index, myicsa->bD->bdata[index]);		
		}	
		printf("\n\n");
	}
	// ESTRUCTURAS PARA ACCEDER O ARRAY DE SUFIXOS E A SUA INVERSA
	printf("Suffix Array Sampling Structures: (Sampling period = %d)\n", myicsa->T_A);
	printf("\tSuffix Array Samples:\n");
	for(index=0; index < myicsa->samplesASize; index++) 
		printf("\tSamplesA[%d] = %d\n", index, myicsa->samplesA[index]);
	printf("\n");	
	printf("\tInverse Suffix Array Samples:\n");
	for(index=0; index < myicsa->samplesASize; index++) 
		printf("\tSamplesAInv[%d] = %d\n", index, myicsa->samplesAInv[index]);
	printf("\n");
			
}


// Comparacion de Sadakane entre un patron (pattern) y el sufijo en la posicion p del array de sufijos
// IMPORTANTE EVITAR ULTIMA CHAMADA A PSI
int SadCSACompare(ticsa *myicsa, uint *pattern, uint patternSize, uint p) {

	register unsigned int j, i, currentInteger, diff;
	
	i = p;
	j = 0;
	
	while(1) {
		currentInteger = rank(myicsa->bD, i) - 1;
		diff = pattern[j++] - currentInteger;
		if(diff) return diff;
		if(j == patternSize) return 0;
		else 
			#ifdef PSI_HUFFMANRLE
				i=getHuffmanPsiValue(&(myicsa->hcPsi),i);
			#endif
			#ifdef PSI_GONZALO
				i=getGonzaloPsiValue(&(myicsa->gcPsi),i);
			#endif		
			#ifdef PSI_DELTACODES
				i=getDeltaPsiValue(&(myicsa->dcPsi),i);
			#endif
	}
	
}


// Acceso a array de sufixos A
inline uint A(ticsa *myicsa, uint position) {

	register uint timesPsi, sampleValue;
	register uint T_A = myicsa->T_A;
	
	uint proba = position;
	
	timesPsi = 0;
	while(!bitget(myicsa->BA, position)) {
	
		#ifdef PSI_HUFFMANRLE
		 	position=getHuffmanPsiValue(&(myicsa->hcPsi),position);
		#endif		 
		#ifdef PSI_GONZALO
			position=getGonzaloPsiValue(&(myicsa->gcPsi),position);
		#endif
		#ifdef PSI_DELTACODES
			position=getDeltaPsiValue(&(myicsa->dcPsi),position);
		#endif
		timesPsi++;
		
	}
	sampleValue = myicsa->samplesA[rank(myicsa->bBA, position)-1];
	
	return sampleValue - timesPsi;

}


// Acceso 'a inversa do array de sufixos
inline uint inverseA(ticsa *myicsa, uint offset) {

	register uint index, inverseValue;
	register uint T_AInv = myicsa->T_AInv;
	
	inverseValue = myicsa->samplesAInv[offset/T_AInv];
	for(index=0; index<(offset%T_AInv); index++) 
		#ifdef PSI_HUFFMANRLE
			inverseValue=getHuffmanPsiValue(&(myicsa->hcPsi),inverseValue);
		#endif		
		#ifdef PSI_GONZALO
		   inverseValue = getGonzaloPsiValue(&(myicsa->gcPsi),inverseValue);
		#endif	
		#ifdef PSI_DELTACODES
			inverseValue = getDeltaPsiValue(&(myicsa->dcPsi),inverseValue);
		#endif
	return inverseValue;
	
}

// Initializes the parameters of the index.
uint parametersCSA(ticsa *myicsa, char *build_options){ 
	char delimiters[] = " =;";
	int j,num_parameters;
	char ** parameters;
	int ssA,ssAinv,ssPsi,nsHuff,psiSearchFactor;
	
	ssA    = DEFAULT_A_SAMPLING_PERIOD;
	ssAinv = DEFAULT_A_INV_SAMPLING_PERIOD;
	ssPsi  = DEFAULT_PSI_SAMPLING_PERIOD;
	nsHuff = DEFAULT_nsHUFF;
	psiSearchFactor = DEFAULT_PSI_BINARY_SEARCH_FACTOR;
	
	if (build_options != NULL) {
	parse_parameters(build_options,&num_parameters, &parameters, delimiters);
	for (j=0; j<num_parameters;j++) {
	  
		if ((strcmp(parameters[j], "sA") == 0 ) && (j < num_parameters-1) ) {
			ssA=atoi(parameters[j+1]);			
		} 
		if ((strcmp(parameters[j], "sAinv") == 0 ) && (j < num_parameters-1) ) {
			ssAinv=atoi(parameters[j+1]);			
		} 	
		if ((strcmp(parameters[j], "sPsi") == 0 ) && (j < num_parameters-1) ) {
			ssPsi=atoi(parameters[j+1]);			
		} 	
		if ((strcmp(parameters[j], "nsHuff") == 0 ) && (j < num_parameters-1) ) {
			nsHuff=atoi(parameters[j+1]);
			nsHuff *=1024;			
		} 
		if ((strcmp(parameters[j], "psiSF") == 0 ) && (j < num_parameters-1) ) {
			psiSearchFactor=atoi(parameters[j+1]);
		} 			
		j++;
	}
	free_parameters(num_parameters, &parameters);
	}		

	myicsa->T_A = ssA;
	myicsa->T_AInv = ssAinv;
	myicsa->T_Psi = ssPsi;
	myicsa->tempNSHUFF = nsHuff;
	myicsa->psiSearchFactorJump = psiSearchFactor * ssPsi;	

	printf("\n\t parameters for iCSA: sampleA=%d, sampleAinv=%d, samplePsi=%d",ssA,ssAinv,ssPsi);
	printf("\n\t              : nsHuff=%d, psiSearchFactor = %d --> jump = %d", nsHuff,psiSearchFactor, myicsa->psiSearchFactorJump);
}

/* RANDOM PATTERNS CHOOSER -------------
   Given 
   	A file cointaining a given number of words (and /or separators) with their lenght and frequency ...
   	A value #maxIters
   	A value #patternsPerIter1, #patternsPerIter2, ..., #patternsPerIterN
   
   Chooses randomly #iters times #patternsPerIter random patterns, ensuring that no repeated patterns
   are generated in each iteration.
   
   Outputs...
   	N files containing #patternsPerIter_i * #iters  patterns.
   		The patterns in positions (0 < i <#patterns) + j*#patterns, j=0,1,2 ... #maxIters can not be repeated.
-----

Copyright (C) 2005 Antonio Fariña.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Author's contact: Antonio Fariña, Dept. of Computer Science, University of
A Coruña. Campus de Elviña s/n. Spain  fari@udc.es
*/


#define byte unsigned char
#define AND &&
#define OR &&

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdlib.h>

#include <stdarg.h>  // vscanf
#include <sys/stat.h> //stat.

#include "options.h"

#define MAXSIZEPALABRA 1

struct Vocab {
        unsigned char   *palabra;   // palabra del vocabulario
        unsigned int 	numBytesPalabra;
        unsigned int 	frecuencia;
} ;

typedef struct Vocab t_Vocab;

t_Vocab *lista;
unsigned int  numPalabras;


/*************************************************************************/
/* loads the vocabulary into memory
*/
void CargaVocabulario(char *ficheiro){

    FILE *ffrec;  //para sacar la lista de frecuencias ...
    unsigned int i;
    unsigned int numBytesPalabra;
    unsigned int frecuencia;
    unsigned char palabra[1024];

   	unsigned int itmp;

	if ((ffrec = fopen(ficheiro,"r")) == NULL)
		{
		  fprintf(stderr,
			  "Error en la apertura del fichero %s\n",
			  ficheiro);
		  exit(0);
		}

	//	fread(&numPalabras,4,1,ffrec);
	//fprintf(stderr,"ENTRADA EN CARGAVOCABULARIO\n");
	int errr= fscanf(ffrec,"%d\n",&numPalabras);
	//	fprintf(stderr,"\nLeyendo NumPalabras[%d]\n",numPalabras);


	//FORMAT OF THE FILE ...
	// [POS] [SIZE] [OCCUR] [WORD]
	// %7ld   %9Ld   %9Ld   %s\n"

	lista= (t_Vocab *) malloc(numPalabras * sizeof(t_Vocab));

	for (i=0;i<numPalabras;i++) {
		int err=fscanf(ffrec,"%7d ",&itmp);
		err=fscanf(ffrec,"%9d "  ,&numBytesPalabra);
		err=fscanf(ffrec,"%9d",&frecuencia);

		lista[i].palabra = (unsigned char *) malloc (numBytesPalabra +1);
		err=fscanf(ffrec,"%s\n", lista[i].palabra);

		lista[i].frecuencia = frecuencia;
		lista[i].numBytesPalabra = numBytesPalabra;

		//		fprintf(stderr,"NumbytesPalabra %d\n ",lista[i].numBytesPalabra);
		//		fprintf(stderr," Frecuencia %d\n ",lista[i].frecuencia);
		//		fprintf(stderr," PALABRA [%s]\n ",lista[i].palabra);

	}

	fprintf(stderr,"\nVector de palabras cargado: [%ld] palabras...\n", (long) numPalabras);
}



/*************************************************************************/

unsigned int_rand(int n)
	{

	return ((int)  ((1.0*rand()/ (RAND_MAX + 1.0)) * n));
}

/*------------------------------------------------------------------
 Writes the integer indicating the number of patterns of the sample file.
------------------------------------------------------------------ */
void outputNumPatterns(FILE *outFile, uint num) {
	fprintf(outFile,"#numberOfFrases::%9d, #NumberOfWordsPerPhrase:: %9d\n",num, 1);
}
/*------------------------------------------------------------------
 Filters the words in the vocabulary and writes them to the
 output file.
------------------------------------------------------------------ */
void outputWord(FILE *outFile, unsigned char* phrase, uint len) {
	static counter =0;
	fprintf(outFile,"%9d %9d %s\n",counter,len,phrase);
	fprintf(stderr, "%9d %9d %s\n",counter,len,phrase);
	counter++;
}


/*
  Creates a file containing iterat * numPatterns patterns.
  Patterns from       0 <= i < numPatterns are not repeated 
  				numPatterns	<= i < 2*numPatterns		...are not repeated 
*/
void generateFile(int iterat, int numPatterns, char *preffix ) {
		unsigned int *chosenPos;

		unsigned int i,z;
		unsigned int pos;
		unsigned int iterActual =1;
				
		FILE *fout;
		char outFile[100];
		char tmp[10];

		chosenPos = (unsigned int *) malloc(1*4*numPatterns);

		outFile[0]='\0';
		strcat(outFile,"./tmp/");
		strcat(outFile,preffix);
		strcat(outFile,"_");
		sprintf(tmp,"Iter%ld_",(long)iterat);
		strcat(outFile,tmp);
		sprintf(tmp,"Pats%ld", (long)numPatterns);
		strcat(outFile,tmp);
		
		remove(outFile);
		
		if ((fout = fopen(outFile,"w")) == NULL){
		  fprintf(stderr,
			  "Error en la apertura del fichero %s\n",
			  "frequencias,txt");
		  exit(0);
		}		

		outputNumPatterns(fout, iterat);

		// MAIN LOOP OF CHOOSNING RND PATTERNS******************
		
		fprintf(stderr, "\n Generating %d file with %dx%d random patterns: ",1,numPatterns,iterat);
		for (;iterActual <= iterat; iterActual++) {
				
				//generates the random patterns.
				for (i=0;i<numPatterns;i++) {
					pos=int_rand(numPalabras);
					chosenPos[i]=pos;  //the random positions inside "lista"

					//avoids repeated random patterns							
					for (z=0;z<i;z++) {
							if (pos == chosenPos[z]) {   //a new iteration "i" will search for other word
								i--;
							}
					}
 				}
	

				//writes the random patterns to the output file.				
				for (i=0; i< numPatterns; i++) {
						outputWord(fout, lista[chosenPos[i]].palabra,lista[chosenPos[i]].numBytesPalabra) ;

					   /*#ifdef OPTION_ONLY_WORDS
							fprintf(fout,"%s\n",lista[chosenPos[i]].palabra);
						#endif
						
						#ifdef OPTION_FREQ_AND_WORDS
							fprintf(fout,"%9ld %9ld %s\n",lista[chosenPos[i]].frecuencia,lista[chosenPos[i]].numBytesPalabra , lista[chosenPos[i]].palabra);
						#endif
						*/
				}
		}
				
		fclose(fout);	 
		free(chosenPos);
		//MAIN LOOP OF CHOOSNING RND PATTERNS ENDS ***********************	
}

/***************************************************************/

int main (int argc, char *argv[]) {

	unsigned int z;
   unsigned char nombreFichTiempos[100];

	int iterations = 1;
	unsigned int numFiles =1;
	
	unsigned int i;

	/* Read parameters */
	if (argc < 5) {
		printf("\nUsage: %s <AllPatternsFile> <outPrefix> <MaxIters> <numPatterns1> [<numPatterns2>] [<numPatterns3>] ... \n\n", *argv);
		exit(0);
	}

	
	{/* PREPARES the tmp directory and deletes previous tmp files */	 
	struct stat buf;	
	if (stat ("tmp", &buf )) 
		{fprintf(stderr,"\n...Creating directory ./tmp");
		 int err= system("mkdir tmp");
		}
	}


	srand( time(NULL) );

	iterations = atoi(argv[3]);
	numFiles = argc-4;  //starting from argv[3] to argv[3+numPatterns -1]

	fprintf(stderr, "fichero con patrones: [%s],  maxIters: [%d]\n", argv[1], iterations);
	fprintf(stderr, " numFiles [%d]\n", numFiles);

	CargaVocabulario(argv[1]);
	
	//Each file contains #iterations with #numPatts patterns each.
	for (i=0;i<numFiles;i++){
		int numPatts;
		numPatts = atoi(argv[4+i]);
		fprintf(stderr,"\n NUMPATTS %ld",(long)numPatts);
		generateFile(iterations, numPatts ,argv[2]);
	}


	//frees memory
	fprintf(stderr,"\n FREEING MEMORY ...\n");
	for (i=0; i<numPalabras; i++) {
		free(lista[i].palabra);
	}
	free(lista);	

}


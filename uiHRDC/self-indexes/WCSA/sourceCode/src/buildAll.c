#include "buildFacade.h"

/**------------------------------------------------------------------ 
  *  MAIN PROGRAM.
  *------------------------------------------------------------------ */

	int main(int argc, char* argv[])
	{
				
		char *infile, *outbasename, *stopwordsfile;	// Name of in/out files
		byte *inputBuffer;
		ulong finsize;
	
	 	int f_in;
		void *Index;

		
		printf("\n*Presentation level for CSA (simple WCSA)");
		printf("\n*CopyRight (c) 2007 [LBD & G.N.]\n\n");
	
		// Reads input parameters from command line.
		if(argc < 3) {
			printf("Use: %s <in file> <out basename> [build_options]\n", argv[0]);
			exit(0);
		}
	
		// Reads params (input file, output basename, and stopwords file)
		infile = argv[1];
		outbasename = argv[2];
		stopwordsfile = argv[3];
		
		finsize= fileSize(infile);
		
		if (! finsize) {
			printf( "\nFILE EMPTY OR FILE NOT FOUND %s !!\nSkipping processement ...\n",infile);
			exit(0);
		}	
	
		// Opening the input text file.
		if( (f_in = open(infile, O_RDONLY)) < 0) {
			printf("Cannot read file %s\n", infile);
			exit(0);
		}	
		inputBuffer = (byte *) malloc(finsize *sizeof(byte));// +1);
		read (f_in,inputBuffer,finsize);	
		close (f_in);	
		
		
	{
		//printf("\n parametros <<%s>>\n\n",stopwordsfile);	
		//	build_WCSA (inputBuffer, finsize, stopwordsfile, NULL,outbasename);								
		build_index (inputBuffer, finsize, stopwordsfile, &Index);  /** building the index */


		/** saving the index to disk*/
		
		save_index (Index, outbasename);		
		fprintf(stderr,"Index saved !! ");

		/** tells the mem used by the index */
		ulong indexsize;		
		index_size(Index, &indexsize);
		fprintf(stderr,"Index occupied %lu bytes, 2 extra mallocs = %lu",indexsize, (ulong) 2* sizeof(uint));


		/** recovering the source text from the index */
			{
				double start, end;
				start = getTime2();
				ulong size;
				get_length(Index, &size);

				fprintf(stderr, "\nRecovering source file ");	fflush(stderr);
				char ext1[10]=".source";
				recoverSourceText1((twcsa*) Index, outbasename,ext1, size);
				end = getTime2();	
				fprintf(stderr, " time: %.3f secs\n", end-start );	

				start=end;
				char ext2[10]=".source2";
				fprintf(stderr, "\nRecovering source file ");	fflush(stderr);
				recoverSourceText2((twcsa*) Index, outbasename,ext2,size);
				end = getTime2();	
				fprintf(stderr, " time: %.3f secs\n", end-start );	
				//fprintf(stderr, "\nRecovering source file time: %.3f secs\n", end-start );	
			}
		
		// DISPLAYING THE OCCURRENCES OF A TEXT PATTERN (word/phrase).
			{unsigned char textPattern[MAX_TEXT_PATTERN_SIZE];
			 int error = 0;
			ulong numocc,numc, length, i, *snippet_len, tot_numcharext = 0, numpatt;
			uchar *pattern, *snippet_text;
				 
				 pattern = textPattern;
			 printf("\nSEARCH TEST for DISPLAY (pizzachili interface)\n");
				while(1) {	
					printf("Intro string: ");
					fgets((char*)textPattern, MAX_TEXT_PATTERN_SIZE, stdin);
					if (!strcmp((char*)textPattern,"\n") ) break;
					 textPattern[strlen((char*)textPattern)-1] = '\0';
		
					length = strlen( (char*)textPattern);
					numc=50;
         
//					error =	display (Index, textPattern, length, numc, &numocc, 
//							    	 &snippet_text, &snippet_len);
					error =	displayWords (Index, textPattern, length, numc, &numocc, 
							    	 &snippet_text, &snippet_len,1);
					
					if (error){ fprintf(stderr, "%s\n", "Hubo un error durante display");exit(0);}
		
						fprintf(stderr,"\n acabou display");fflush(stderr);			
					{//show the results
						ulong j, len = length + 2*numc;
					    char blank = '\0';
						fprintf(stderr,"\n length = %lu",(ulong) length);
						fprintf(stderr,"\n pattern = %s", pattern);fflush(stderr);
						fprintf(stderr,"\n numocc = %lu", (ulong)numocc);fflush(stderr);
						fprintf(stderr,"\n snippet len = %lu",(ulong) len);fflush(stderr);
						fprintf(stderr,"\n =========");fflush(stderr);		
						for (i = 0; i < numocc; i++){
							fprintf(stderr,"\n[%2lu][len=%3lu]<<", (ulong)i+1,(ulong)snippet_len[i]);fflush(stderr);
							fwrite(snippet_text+len*i,sizeof(uchar),snippet_len[i],stderr);fflush(stderr);
							fprintf(stderr,">>");fflush(stderr);
						}
					}
					numpatt--;
					
					for(i=0; i<numocc; i++) {
						tot_numcharext += snippet_len[i];
					}
						
					if (numocc) {
						free (snippet_len);
						free (snippet_text);
					}
					
					printf("Ocurrences = %lu\n", numocc);
					if (!strcmp((char*)textPattern,"\n") ) break;
				}
			}
	


			// SEARCHING FOR A TEXT PATTERN (word/phrase).
			{unsigned char textPattern[MAX_TEXT_PATTERN_SIZE];
			 int occ;
			 int len;
			 uint *occs;
			 int i;
			 printf("\nSEARCH TEST for LOCATE\n");
				while(1) {	
					printf("Intro string: ");
					fgets((char*)textPattern, MAX_TEXT_PATTERN_SIZE, stdin);
					len = strlen((char*)textPattern);
					if (!strcmp((char*)textPattern,"\n") ) break;
					textPattern[len-1] = '\0';
					len --;
					
					//occs = locateTextOcurrences(wcsa,textPattern,&occ);
					// locate(Index, textPattern, len, (ulong **)&occs, (ulong *)&occ);
					  locateWord(Index, textPattern, len, (ulong **)&occs, (ulong *)&occ, 0);
					
					printf("\n*** %s occurs %d times: In the source text in positions:\n\t",textPattern,occ);
		/* 			for (i=0;i<occ;i++) 
						printf("[%u]",occs[i]);
					fflush(stderr);	 */
					if (occ >0) free(occs);		
					
					if (!strcmp((char*)textPattern,"\n") ) break;
				}
			}	
	



		/** freeing the index */		 			
		free_index(Index);	 			
	 		 
	}
}





























//{	
//	bG = createBitmap (B,len);
//	bE = createBitmapEdu (B,len);
//	//fprintf(stderr,"\n RANK DE GONZALO DA %u, de EDU DA %u\n",rank(bG,33),rank1Edu(bE,33));
//	//fprintf(stderr,"\n SELECT1(2) DE GONZALO DA %u, de EDU DA %u\n",bselect(bG,4),bselect(bE,4));
//	
//	showBitVector(bitvector,34);	
//}



/*	

	//USING A HASH TABLE
	//	{
	//	char a[20]="beginnings";char b[20]="HOSTIAS";
	//	char *w;	
	//	int i;
	//	w=a;
	//	i = inHashTable(stopwordshash,w, strlen(w), &addrInTH );
	//	if (!i) insertElement (stopwordshash, w, strlen(w), &addrInTH);
	//	else stopwordshash->hash[addrInTH].freq++;
	//	//fprintf(stderr,"\n i = %ld, addrInTh = %ld ",i,addrInTH);
	//	//fprintf(stderr,"\n word in hash[%ld]= %s , freq = %ld, posinvoc =%ld",addrInTH, stopwordshash->hash[addrInTH].word, stopwordshash->hash[addrInTH].freq, stopwordshash->hash[addrInTH].posInVoc);	
	//  }



	/// ENCODING THE separators  ...
{	
	freeHuff(gapsHuffman);
	uint i;
	uint *bitvector;
	uint bitvectorSize;
	uint ptr;
	bitmap bG,bE;
	uint len;
	len = 1000; //number of bits
	bitvector = (uint *) malloc ((len/32 +1)* sizeof(uint));
	
	byte texto[100] = "####@?*";
	uint freqs[256];
	
	//fprintf(stderr,"\n este es el texto a codificar: %s",texto);
	for (i=0;i<256;i++) freqs[i]=0;
	for (i=0;i<strlen(texto);i++) freqs[texto[i]]++;
	gapsHuffman = createHuff (freqs,255,UNSORTED);
		
	ptr=0;
	for (i=0;i<strlen(texto);i++) {
		//fprintf(stderr,"\n ENCODING seprators !!\n");
		//fprintf(stderr,"%d. \n",ptr=encodeHuff(gapsHuffman, texto[i],bitvector,ptr) );		
	}	

	prepareToDecode(&(gapsHuffman));
	bitvectorSize = ptr;
	showBitVector(bitvector,bitvectorSize);	
	uint pos;
	//fprintf(stderr,"\n DECODING !!\n");
	ptr=0;
	while (ptr < bitvectorSize) {
		ptr=decodeHuff (gapsHuffman, &pos, bitvector, ptr);
		//fprintf(stderr,"\n DECODING pos is %ld!!\n",pos);
		//fprintf(stderr,"%c. \n",pos);
	}
	exit(0);	
}															
	
/// ENCODING THE CANONICAL WORDS ...
{	uint i;
	uint *bitvector;
	uint bitvectorSize;
	uint ptr;
	bitmap bG,bE;
	uint len;
	len = 1000; //number of bits
	bitvector = (uint *) malloc ((len/32 +1)* sizeof(uint));
	
	
	ptr=0;
	//fprintf(stderr,"\n ENCODING VARIANTS !!\n");
	//fprintf(stderr,"%d. \n",ptr=encodeHuff(posInHT[0].huffman, 0,bitvector,ptr) );
	//fprintf(stderr,"%d. \n",ptr=encodeHuff(posInHT[0].huffman, 1,bitvector,ptr) );
	//fprintf(stderr,"%d. \n",ptr=encodeHuff(posInHT[0].huffman, 2,bitvector,ptr) );
	//fprintf(stderr,"%d. \n",ptr=encodeHuff(posInHT[0].huffman, 3,bitvector,ptr) );
	//fprintf(stderr,"%d. \n",ptr=encodeHuff(posInHT[0].huffman, 4,bitvector,ptr) );
	//fprintf(stderr,"%d. \n",ptr=encodeHuff(posInHT[0].huffman, 4,bitvector,ptr) );
			
	//	FILE *f;
	//	f = fopen("huff","w");
	//	saveHuff(posInHT[0].huffman,f);
	//	fclose(f);
	//	
	//	f = fopen("huff","r");
	//	posInHT[0].huffman = loadHuff (f,0);
	//	fclose(f);
	//	
	
	prepareToDecode(&(posInHT[0].huffman));

	FILE *f;
	f = fopen("huff","w");
	saveHuffAfterDecode(posInHT[0].huffman,f);
	fclose(f);
	
	f = fopen("huff","r");
	//posInHT[0].huffman = loadHuffAfterDecode(f,0);
	loadHuffAfterDecode2 (&(posInHT[0].huffman),f,0);
	fclose(f);
	


	bitvectorSize = ptr;
	showBitVector(bitvector,bitvectorSize);	
	uint pos;
	//fprintf(stderr,"\n DECODING !!\n");
	ptr=0;
	while (ptr < bitvectorSize) {
		ptr=decodeHuff (posInHT[0].huffman, &pos, bitvector, ptr);
		//fprintf(stderr,"\n DECODING pos is %ld!!\n",pos);
		//fprintf(stderr,"%s. \n",posInHT[0].variants[pos]);
	}
	exit(0);
}	
	
	
			
{	uint i;
	uint *bitvector;
	bitmap bG,bE;
	uint len;
	len = 101; //number of bits
	bitvector = (uint *) malloc ((len/32 +1)* sizeof(uint));
	//bitvector[0]=0;
	//bitvector[1]=0;
	bitzero (bitvector,0,101-1);
	for (i=0; i<len;i++) setBit (bitvector,len,i,0);
	 


	bitset(bitvector,1); 
	bitset(bitvector,10);
	bitset(bitvector,12);
	//activateBit(bitvector,1); 
	//activateBit(bitvector,10); 
	
	bG = createBitmap (bitvector,len);
	bE = createBitmapEdu (bitvector,len);
	//fprintf(stderr,"\n RANK DE GONZALO DA %u, de EDU DA %u\n",rank(bG,33),rank1Edu(bE,33));
	//fprintf(stderr,"\n SELECT1(2) DE GONZALO DA %u, de EDU DA %u\n",bselect(bG,4),bselect(bE,4));
	
	showBitVector(bitvector,34);
}	
*/		
	

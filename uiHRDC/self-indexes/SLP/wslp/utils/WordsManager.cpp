/* WordsManager.cpp
 * Copyright (C) 2011, Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * Extracted from the implementation of Antonio Fariña
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
 *
 * Contacting the author:
 *   Miguel A Martinez-Prieto:  migumar2@infor.uva.es
 */

#include "WordsManager.h"

WordsManager::WordsManager()
{
	// Initializes the arrays used to detect if a char is valid or not.
	ValString::StartValid();
	// Initializes the arrays used translated a char into lowercase.
	ValString::StartToLow();

	avglen = 0;
}

void 
WordsManager::parse(uchar *text, uint length, char *build_options, int **iseq, uint *nwords, uint *docboundaries, uint ndocs)
{
	t_hash hash;   		// the hash table to store both variants and canonical words.				      	
	tposInHT *posInHT;	// structure for canonicals and variants+huffmans 

	zeroNode=0;
	seSize=0;

	uint totallenWords=0; 	//The numberOfBytes that occupy canonical words (their ascii version) in memory
	ulong bytesFile,bytesFileReal;
	long sizeNValue;

	/* used during first pass */		
	ulong addrInTH;
 	
	uchar* inputBuffer = text;	
	bytesFileReal= bytesFile = length;
	sourceTextSize=length;

	//Estimation (Using Heap's law) of the number of different "meaningful" words.
	if(bytesFile<5000000) bytesFile = 5000000;
	sizeNValue = (unsigned long) floor(3.9* pow(bytesFile,0.60) );
	uint *doc_beg_id = (uint *) malloc (sizeof(uint)*(ndocs+1));
	
	// **********************************************************************************
	// STARTING THE FIRST PASS. 
	// **********************************************************************************

	posInHT = (tposInHT *) malloc(sizeof(tposInHT) * sizeNValue); 
	hash = HashWM::initialize_hash (sizeNValue); 

	//-----------------------------------------------------------------	
	//1st pass (processing the file)
	{ 
		uchar *pbeg,*pend,*wordstart,*aWord;
		register ulong size;
		register uint i;

		uchar *T = inputBuffer;
		uint n = bytesFileReal;

		uint j;
		
		for (j=0;j<ndocs;j++) 
		{
			pbeg = T + docboundaries[j];
			pend = T + docboundaries[j+1];

			while (pbeg <pend) 
			{  			
				//parsing either a word or separator.			
				size=0;
				wordstart = pbeg;
				if (_Valid[*pbeg])
				{   //alphanumerical data
					while ( (size<MAX_SIZE_OF_WORD) && (pbeg<pend)&& ( _Valid[*pbeg] )) {size++;pbeg++;}
			   	}		    
				else 
				{
					if (*pbeg != ' ') 
					{ //a separator comes starting in ' ' comes, so it is a new word					
						while ( (size<MAX_SIZE_OF_GAP) && (pbeg<pend) &&  (!_Valid[*pbeg] )) {size++;pbeg++;}
					}
					else 
					{  //a  SPACE comes, so we have to test if next character is alphanumerical or not
						pbeg++;
						if (pbeg >= pend) {size++;}  // a unique BLANK at the end of the file.
						else 
						{
							if (_Valid [*pbeg] ) 
							{
								wordstart = pbeg;   //So skipping 1 blank character
								while ( (size<MAX_SIZE_OF_WORD) && (pbeg<pend)&& ( _Valid[*pbeg] )) {size++;pbeg++;}
							}
							else 
							{   // a "separator word" ...
								size++; //the prev BLANK...
								while ( (size<MAX_SIZE_OF_GAP) && (pbeg<pend) &&  (!_Valid[*pbeg] )) {size++;pbeg++;}
							}//else {  // a "separator word"
						}//else ... not a unique BLANK AT THE END.
					}//else ... starting by a BLANK... 
				}

				//The parsed word/separator is  is "wordstart", and its length is "size"...
				aWord=wordstart;				

				//Processement done for each word word			
				i = HashWM::inHashTable(hash,aWord, size, &addrInTH );				
				if (!i)
				{
					HashWM::insertElement (hash,aWord, size, &addrInTH);
					posInHT[zeroNode].slot=addrInTH;
					posInHT[zeroNode].word=hash->hash[addrInTH].word;
					hash->hash[addrInTH].posInVoc = zeroNode;
					zeroNode++;
					totallenWords += size +1;			// +1 due to the '\0' char...		
				}							
				seSize ++;		
			}
		}
	}//1st pass ends

	avglen = (uint)(1+ceil(totallenWords/zeroNode));

	// **********************************************************************************
	// END OF 1ST PASS
	// **********************************************************************************

	// Sorting the words alphanumerically (over posInHT)
	{	
		register unsigned long i,j;	
		//sorting canonical words ...
		qsort(posInHT, zeroNode, sizeof(tposInHT), qSortWordsCompareAlpha);
		
		//setting in hash the new positions of the  words in the hash table
		for (i=0;i<zeroNode;i++) 
		{	
			hash->hash[posInHT[i].slot].posInVoc = i;	
		}
	}	

	// INITIALIZING structures for the 2nd pass ......................................
	{
		*iseq  = new int[seSize+1];
	}

	// **********************************************************************************
	//  STARTING THE SECOND PASS.
	// **********************************************************************************/

	//2nd pass (processing the file)
	{ 
		uchar *pbeg,*pend,*wordstart,*aWord;
		register ulong size;
		register uint i;
		register uint countValidWords = 0;

		uchar *T = inputBuffer;
		uint n = bytesFileReal;
		uint j;
	
		for (j=0;j<ndocs;j++) 
		{
			pbeg = T + docboundaries[j];
			pend = T + docboundaries[j+1];
			doc_beg_id[j]=countValidWords;
				
			while (pbeg < pend) 
			{  			
				//parsing either a word or separator.			
				size=0;
				wordstart = pbeg;
				if (_Valid[*pbeg]) 
				{   //alphanumerical data
					while ( (size<MAX_SIZE_OF_WORD) && (pbeg<pend)&& ( _Valid[*pbeg] )) {size++;pbeg++;}
				}		    
				else 
				{
					if (*pbeg != ' ') 
					{ //a separator comes starting in ' ' comes, so it is a new word					
						while ( (size<MAX_SIZE_OF_GAP) && (pbeg<pend) &&  (!_Valid[*pbeg] )) {size++;pbeg++;}
					}
					else 
					{  //a  SPACE comes, so we have to test if next character is alphanumerical or not
						pbeg++;
						if (pbeg >= pend) {size++;}  // a unique BLANK at the end of the file.
						else 
						{
							if (_Valid [*pbeg] ) 
							{
								wordstart = pbeg;   //So skipping 1 blank character
								while ( (size<MAX_SIZE_OF_WORD) && (pbeg<pend)&& ( _Valid[*pbeg] )) {size++;pbeg++;}
							}
							else 
							{   // a "separator word" ...
								size++; //the prev BLANK...
								while ( (size<MAX_SIZE_OF_GAP) && (pbeg<pend) &&  (!_Valid[*pbeg] )) {size++;pbeg++;}
							}//else {  // a "separator word"
						}//else ... not a unique BLANK AT THE END.
					}//else ... starting by a BLANK... 
				}

				//The parsed word/separator is  is "wordstart", and its length is "size"...
				aWord=wordstart;					

				//Processement done for each word word			
				i = HashWM::inHashTable(hash,aWord, size, &addrInTH );	

				(*iseq)[countValidWords]=hash->hash[addrInTH].posInVoc+1;  // !!!!
				countValidWords++;		
			}
		}
		doc_beg_id[j]=countValidWords;

		ofstream wbounds((char*)((string(build_options)+".docs").c_str()));
		saveValue(wbounds, doc_beg_id, j);
		wbounds.close();
		
		(*iseq)[countValidWords] = 0;		
		
		*nwords = countValidWords;
	}//2nd pass ends

	
	// **********************************************************************************
	// END OF 2ND PASS
	// **********************************************************************************

	//freeing the source text (it is no longer needed).
	delete [] inputBuffer;

	// Creating the words of the vocabulary...
	{  
		/** copying the words into WSA. */
		uint *tmpOffsets = (uint *) malloc (sizeof(uint) * (zeroNode  +1) );  //1 extra uint (to point to the virtual "zeroNode+1" ^th word.
		uint tmpOffset =0;
		 
		uchar *zoneMem,*src;
		uint i;
								 
		//Moving data from posInHT to WSA structure
 		wordsData.wordsZoneMem.size = totallenWords - zeroNode; //without '\0' bytes (end-tag).
 		wordsData.wordsZoneMem.zone = (uchar *) malloc ( wordsData.wordsZoneMem.size * sizeof(uchar) );  
 		zoneMem = wordsData.wordsZoneMem.zone;

		for(i = 0; i < zeroNode; i++) 
		{
			src = posInHT[i].word;					//copying the canonical word
			tmpOffsets[i]=tmpOffset;  				//offset in zoneMem
			while (*src) {*zoneMem++ = *src++; tmpOffset++;}	//moving data until '\0'			
		}
		tmpOffsets[zeroNode]=tmpOffset; //setting pointer to the "virtual" word {zeroNode+1}^{th}
		
		//kbit encoding of the offsets
		uint elemSize = bits(tmpOffset);
		wordsData.elemSize = elemSize;
		wordsData.words = (uint *) malloc (((((zeroNode +1)*elemSize)+W-1) /W) * sizeof(uint));  //with 1 extra slot !.
		wordsData.words[((((zeroNode +1)*elemSize)+W-1) /W)-1]=0000;

		tmpOffset=0;
		for (i=0; i<=zeroNode; i++) 
		{  //setting "zeroNode+1" offsets
			bitwrite(wordsData.words, tmpOffset, elemSize, tmpOffsets[i]);
			tmpOffset+=elemSize; 
		}
				

		//frees memory from hash table and posInHT structures.
		free(tmpOffsets);
		HashWM::destroy_hash(hash);
		free(posInHT);
	}
}
		
int 
WordsManager::save(ofstream  &output)
{
	saveValue(output, zeroNode);
	saveValue(output, wordsData.elemSize);
	saveValue(output, wordsData.wordsZoneMem.size);
	saveValue(output, avglen);
	saveValue<uchar>(output, wordsData.wordsZoneMem.zone, wordsData.wordsZoneMem.size);
	saveValue(output, wordsData.words, ((((zeroNode +1)*wordsData.elemSize)+W-1) /W));
}
			
WordsManager*
WordsManager::load(ifstream & input)
{
	WordsManager *wm = new WordsManager();

	wm->zeroNode = loadValue<uint>(input);
	wm->wordsData.elemSize = loadValue<uint>(input);
	wm->wordsData.wordsZoneMem.size = loadValue<uint>(input);
	wm->avglen = loadValue<uint>(input);

	wm->wordsData.wordsZoneMem.zone = (uchar *) malloc(wm->wordsData.wordsZoneMem.size * sizeof(uchar));
	for (uint i=0; i<wm->wordsData.wordsZoneMem.size; i++) wm->wordsData.wordsZoneMem.zone[i] = loadValue<uchar>(input);


	uint s = ((((wm->zeroNode+1)*(wm->wordsData.elemSize))+W-1)/W);
	wm->wordsData.words = (uint *)malloc (s*sizeof(uint));	
	wm->wordsData.words[s-1] =0000;
	for (uint i=0; i<s; i++) wm->wordsData.words[i] = loadValue<uint>(input);

	return wm;
}

uint 
WordsManager::getID(uchar *word)
{

}

uint 
WordsManager::getWord(uint id, uchar **word)
{
	uint ith = id-1;
	uint tmplen = bitread(wordsData.words, (ith+1)*wordsData.elemSize, wordsData.elemSize);
	uint offtmp = bitread(wordsData.words, (ith )*wordsData.elemSize, wordsData.elemSize);
	tmplen-=offtmp;

	*word = new uchar[tmplen+1];
	strncpy((char*)*word, (char*)(wordsData.wordsZoneMem.zone + offtmp), tmplen);
	(*word)[tmplen] = '\0';

	return tmplen;
}

uint 
WordsManager::addWord(uint id, uchar *stream, bool *isword)
{
	uint ith = id-1;
	uint tmplen = bitread(wordsData.words, (ith+1)*wordsData.elemSize, wordsData.elemSize);
	uint offtmp = bitread(wordsData.words, (ith )*wordsData.elemSize, wordsData.elemSize);
	tmplen-=offtmp;

	char c;
	strncpy(&c, (char*)(wordsData.wordsZoneMem.zone + offtmp), 1);	
	uint space = 0;

	if (_Valid[c])
	{
		if (*isword)
		{
			stream[0] = ' ';
			stream++;
			space++;
		}

		*isword = true;
	}
	else
	{
		*isword = false;
	}

	strncpy((char*)stream, (char*)(wordsData.wordsZoneMem.zone + offtmp), tmplen);

	return tmplen+space;
}

void 
WordsManager::parsePattern(uchar *textPattern, uint patLen, uint **integerPattern, uint *sizeIntegers)
{
	uint reserved = 2*(1+ceil(patLen/avglen));
	*integerPattern = (uint*)malloc(reserved*sizeof(uint));

	uchar *pbeg,*pend,*wordstart,*aWord;
	register unsigned int size;
	uint index =0; 
		 	
	pbeg = textPattern; 
	pend = pbeg + patLen;
							
	while (pbeg <pend)
	{  
		//parsing either a word or separator.
		size=0;
		wordstart = pbeg;

		if (_Valid[*pbeg]) 
		{   //alphanumerical data
			while ( (size<MAX_SIZE_OF_WORD) && (pbeg<pend)&& ( _Valid[*pbeg] )) {size++;pbeg++;}
		}		    
		else 
		{
			if (*pbeg != ' ') 
			{ //a separator comes starting in ' ' comes, so it is a new word					
				while ( (size<MAX_SIZE_OF_GAP) && (pbeg<pend) &&  (!_Valid[*pbeg] )) {size++;pbeg++;}
			}
			else 
			{  //a  SPACE comes, so we have to test if next character is alphanumerical or not
				pbeg++;
				if (pbeg >= pend) {size++;}  // a unique BLANK at the end of the file.
				else 
				{
					if (_Valid [*pbeg] ) 
					{
						wordstart = pbeg;   //So skipping 1 blank character
						while ( (size<MAX_SIZE_OF_WORD) && (pbeg<pend)&& ( _Valid[*pbeg] )) {size++;pbeg++;}
					}
					else 
					{   // a "separator word" ...
						size++; //the prev BLANK...
						while ( (size<MAX_SIZE_OF_GAP) && (pbeg<pend) &&  (!_Valid[*pbeg] )) {size++;pbeg++;}
					}//else {  // a "separator word"
				}//else ... not a unique BLANK AT THE END.
			}//else ... starting by a BLANK... 
		}
				  		
		//The parsed word is "aWord", and its length is "size"...
		aWord=wordstart;	
		
		// Binary search on the canonical words (wordsData)
		{
			uint len, tmplen;
			uchar *wsaWord;
			register uint min,max,p;	
			min = 0;
			max = (zeroNode) - 1;

			while(min < max) 
			{
				p = (min+max)/2;
				
				{
					//preparing for strcompL
					len    = bitread (wordsData.words, (p+1)* wordsData.elemSize , wordsData.elemSize);
					tmplen = bitread (wordsData.words, (p )* wordsData.elemSize  , wordsData.elemSize);
					len -=tmplen;
					wsaWord= (uchar *) wordsData.wordsZoneMem.zone + tmplen;
				}
				
				if(HashWM::strcompL(aWord, wsaWord, size, len) > 0) min = p+1;
				else max = p;					
			}
			
			{//preparing for strcompL
				len    = bitread (wordsData.words, (min+1)* wordsData.elemSize, wordsData.elemSize);
				tmplen = bitread (wordsData.words, ( min )* wordsData.elemSize, wordsData.elemSize);
				len -=tmplen;
				wsaWord= (uchar *) wordsData.wordsZoneMem.zone + tmplen;
			}

			if(!HashWM::strcompL(aWord, wsaWord, size, len)) 
			{
				if (index == reserved)
				{
					reserved*=2;
					*integerPattern = (uint*)realloc(*integerPattern, reserved*sizeof(uint));
				}

				(*integerPattern)[index++] = min +1 ;  //<--
			}
			else {*sizeIntegers = 0; return;}  // a valid word that does not appear in the source text.
			
		}														
	}// end while
	*sizeIntegers = index;	
}

int 
WordsManager::size()
{
	uint size = ((((zeroNode+1)* (wordsData.elemSize))+W-1) /W) * (sizeof(uint));
	size += wordsData.wordsZoneMem.size * sizeof(uchar);
	size += sizeof(WordsManager);

	return size;
}

uint 
WordsManager::getAvgLen()
{
	return avglen;
}

int 
WordsManager::qSortWordsCompareAlpha(const void *arg1, const void *arg2)
{
	tposInHT *a1 = (tposInHT *) arg1;
	tposInHT *a2 = (tposInHT *) arg2;
	return strcmp((char*)a1->word, (char *)a2->word);	
}

void 
WordsManager::bitwrite (register uint *e, register uint p, register uint len, register uint s)
{ 
	e += p/W; p %= W;

	if (len == W)
	{ 
		*e |= (*e & ((1<<p)-1)) | (s << p);
		if (!p) return;
		e++;
		*e = (*e & ~((1<<p)-1)) | (s >> (W-p));
	}
     	else
	{ 
		if (p+len <= W)
		{ 
			*e = (*e & ~(((1<<len)-1)<<p)) | (s << p);
			return;
		}

		*e = (*e & ((1<<p)-1)) | (s << p);
		e++; len -= W-p;
		*e = (*e & ~((1<<len)-1)) | (s >> (W-p));
	}
}

uint 
WordsManager::bitread (uint *e, uint p, uint len)

   { uint answ;
     e += p/W; p %= W;
     answ = *e >> p;
     if (len == W)
	  { if (p) answ |= (*(e+1)) << (W-p);
	  }
     else { if (p+len > W) answ |= (*(e+1)) << (W-p);
            answ &= (1<<len)-1;
	  }

     return answ;
   }

WordsManager::~WordsManager()
{
	free(wordsData.words);
	free(wordsData.wordsZoneMem.zone);
}	


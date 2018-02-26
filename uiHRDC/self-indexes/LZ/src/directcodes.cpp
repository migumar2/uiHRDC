/* Wavelet Tree over Dense Code. -- 
A word index using wavelet tree strategy over compressed text.

Programmed by Susana Ladra.

Author's contact: Susana Ladra, Databases Lab, University of
A Coruna. Campus de Elvina s/n. Spain  sladra@udc.es

Copyright (C) 2007  Nieves R. Brisaboa, Antonio Farina and Susana Ladra
Author's contact: susanaladra@gmail.com

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
*/
  


/*-----------------------------------------------------------------------
 Hash.c: Definition of a HashTable that uses linear hash
 ------------------------------------------------------------------------*/
#include <cstdlib>

#include "directcodes.h"

#define BASE 16
#define BASE_BITS 4

namespace lz77index{
namespace basics{

const unsigned short TABLESUM[256] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};


const unsigned int TABLEBASE[7] = {0,16,272,4368,69904,1118480,17895696};
/*-----------------------------------------------------------------

 ---------------------------------------------------------------- */

FTRep* createFT(unsigned int *list,unsigned int listLength){
	FTRep * rep = (FTRep *) malloc(sizeof(struct sFTRep));
	unsigned int levelSizeAux[7]={0,0,0,0,0,0,0};
	unsigned int cont[7]={0,0,0,0,0,0,0};	
	rep->listLength = listLength;
	register unsigned int i;
	int j, k;
	unsigned int value, newvalue;
	unsigned int bits_BS_len = 0;
	
	//Reservando espacio para los niveles

	for (i=0;i<listLength;i++){
		value = list[i];
		for(j=0;j<7;j++)
			if(value>=TABLEBASE[j])
				levelSizeAux[j]++;
	}
	//
	j=0;
	while((j<7)&&(levelSizeAux[j]!=0)){
		//fprintf(stderr,"levelSize[%d]=%d\n",j,levelSizeAux[j]);
		if(levelSizeAux[j]%2)
			levelSizeAux[j]++;
		 j++;
		}
	rep->nLevels = j;
	//fprintf(stderr,"Number of Levels: %d\n",rep->nLevels);
	rep->levelsIndex = (unsigned int *) malloc(sizeof(unsigned int)*(rep->nLevels+1));
	bits_BS_len =0;
	rep->levelsIndex[0]=0;
	for(j=0;j<rep->nLevels;j++){
		rep->levelsIndex[j+1]=rep->levelsIndex[j] + levelSizeAux[j];
		cont[j]=rep->levelsIndex[j];
		//fprintf(stderr,"nivel %d: de %d a %d\n",j,rep->levelsIndex[j],rep->levelsIndex[j+1]);
	}
	rep->levels = (unsigned char *) malloc(sizeof(unsigned char)*rep->levelsIndex[rep->nLevels]/2);
	bits_BS_len = rep->levelsIndex[rep->nLevels-1]+1; 
	//Se pone el ultimo a 0 para ahorrarnos comparaciones despues en la descompresion
	unsigned int * bits_BS = (unsigned int *) malloc(sizeof(unsigned int)*((bits_BS_len+W-1)/W));
	for(i=0; i<(W-1+bits_BS_len)/W;i++)
		bits_BS[i]=0;

	
	//cuando cont sea impar se pone en la parte superior del byte (<<BASE_BITS)
	//Resto de casos
	for(i=0;i<listLength;i++){
		value = list[i];
		j=rep->nLevels-1;
	//	fprintf(stderr,"value = %d, j= %d\n",value,j);
		while(j>=0){
			if(value >= TABLEBASE[j]){
				newvalue = value - TABLEBASE[j];
		//		fprintf(stderr,"newvalue = %d j= %d\n",newvalue,j);
				for(k=0;k<j;k++){
					//fprintf(stderr,"cont[%d]=%d, limite: %d\n",k,cont[k],rep->levelsIndex[k+1]);
					if(cont[k]%2){
						//fprintf(stderr,"Valor anterior: %d, valor nuevo: %d, valor que queda: %d\n",rep->levels[cont[k]/2],(byte)newvalue%BASE,rep->levels[cont[k]/2]+(((byte)newvalue%BASE)<<BASE_BITS));
						rep->levels[cont[k]/2]=rep->levels[cont[k]/2]+(((unsigned char)newvalue%BASE)<<BASE_BITS);
					}
					else
						rep->levels[cont[k]/2]=(unsigned char)newvalue%BASE;	
					cont[k]++;
					//fprintf(stderr,"rep->levels[%d][%d]=%d\n",k,cont[k]-1,rep->levels[k][cont[k]-1]);
					newvalue = newvalue/BASE;
				}
				
				if(cont[j]%2)
						rep->levels[cont[j]/2]=rep->levels[cont[j]/2]+(((unsigned char)newvalue%BASE)<<BASE_BITS);
				else
						rep->levels[cont[j]/2]=(unsigned char)newvalue%BASE;	
					cont[k]++;
				if(j<rep->nLevels-1){
					bitset(bits_BS,cont[j]-1);
				//fprintf(stderr,"rep->levels[%d][%d]=%d\n\n",j,cont[j]-1,rep->levels[j][cont[j]-1]);
				}
				break;
							}
			j--;
		}

	}
	//Para simular ultimo array:
	bitset(bits_BS,bits_BS_len-1);
	//fprintf(stderr,"se esta creando un bitmap de %d longitud, con %d en la ultima posicion\n",bits_BS_len, bitget(bits_BS,bits_BS_len-1));
	rep->bits_bitmap = bits_BS;
	//rep->bS = createBitmap(bits_BS, bits_BS_len); 
	rep->bS = createBitRankW32Int(bits_BS, bits_BS_len , 1, 20); 	
	//Contadores
	

	
	rep->rankLevels = (unsigned int *) malloc(sizeof(unsigned int)*rep->nLevels);
	for(j=0;j<rep->nLevels;j++)
 			rep->rankLevels[j]= rank(rep->bS, rep->levelsIndex[j]-1);

	return rep;
}


/*-----------------------------------------------------------------

 ---------------------------------------------------------------- */

unsigned int accessFT(FTRep * listRep,unsigned int param){
 unsigned int mult=0;
 //register unsigned int i;
 register unsigned int j;
 //unsigned int * rankLevel = listRep->rankLevels;
 //unsigned char * list;
 //unsigned int n ,
 unsigned int partialSum=0;
 //unsigned int sumAux=0;
 unsigned int ini = param-1;
 //bitRankW32Int * bS = listRep->bS;
 //unsigned int * bsData = listRep->bS->data;
 unsigned int nLevels=listRep->nLevels;
 //unsigned int levelIndex;
 unsigned char * level;
 unsigned char readByte;
 unsigned int cont,pos, rankini;
	//fprintf(stderr,"Queriendo leer la posicion: %d\n",ini);
		partialSum=0;
		j=0;
		level=listRep->levels + (listRep->levelsIndex[j]>>1);
		cont=ini+(listRep->levelsIndex[j]&0x1);
		pos=listRep->levelsIndex[j]+ini;
			
			mult=0;
		readByte = ((*(level+(cont>>1)))>>(BASE_BITS*(cont&0x1)))&0xF;
		//fprintf(stderr,"contenido del vector: %d readByte: %d con cont[%d]: %d\n",*(level[j]+(cont[j]>>1)),readByte,j,cont[j]);
		//fprintf(stderr,"readByte: %d... %d\n",readByte,(*(level[j]+(cont[j]>>1))));
		
		//fprintf(stderr,"pos[%d]= %d\n",j,pos[j]);
        if(nLevels == 1){
            return (readByte<<mult) + TABLEBASE[j];
        }
		while((!bitget(listRep->bS->data,pos))){
		//fprintf(stderr,"pos[%d]= %d\n",j,pos[j]);
			rankini = rank(listRep->bS, listRep->levelsIndex[j]+ini-1) - listRep->rankLevels[j];
			ini = ini-rankini;

			//fprintf(stderr,"readByte: %d\n",readByte);
			//partialSum = partialSum+ readByte*pot256[j];
			partialSum = partialSum+ (readByte<<mult);
			j++;
			mult+=BASE_BITS;
			//fprintf(stderr,"pos[%d] = %d lsit-> levels %d cont[%d] = %d \n",j-1,pos[j-1]-1,listRep->levels, j,cont[j]);
		
			level=listRep->levels + (listRep->levelsIndex[j]>>1);
			cont=ini+(listRep->levelsIndex[j]&0x1);
			pos=listRep->levelsIndex[j]+ini;
		
			readByte = ((*(level+(cont>>1)))>>(BASE_BITS*(cont&0x1)))&0xF;
		//fprintf(stderr,"contenido del vector: %d readByte: %d con cont[%d]: %d\n",*(level[j]+(cont[j]>>1)),readByte,j,cont[j]);
		
			//fprintf(stderr,"readByte: %d... %d\n",readByte,(*(level[j]+(cont[j]>>1))));
			if(j==nLevels-1){
				break;
			}
	
					
		}
		//fprintf(stderr,"readByte: %d\n",readByte);
		//partialSum = partialSum + readByte*pot256[j] + TABLEBASE[j];
		partialSum = partialSum + (readByte<<mult) + TABLEBASE[j];
		//fprintf(stderr,"sum = %d, partialSum = %d, param = %u \n",sum,partialSum,param);
		//list[i]=sum
	//fprintf(stderr,"devolviendo %d\n",partialSum);
	return partialSum;

}

/*-----------------------------------------------------------------

 ---------------------------------------------------------------- */
void destroyFT(FTRep * rep){
	free(rep->levelsIndex);
	free(rep->rankLevels);
	free(rep->levels);
	//destroyBitmap(rep->bS);
	destroyBitRankW32Int(rep->bS);
	//free(rep->bits_bitmap);
	free(rep);
}

/*-----------------------------------------------------------------

 ---------------------------------------------------------------- */
void saveFT(FTRep * rep, FILE* flist){
	//int i;
	fwrite(&(rep->listLength),sizeof(unsigned int),1,flist);
	fwrite(&(rep->nLevels),sizeof(unsigned char),1,flist);
	fwrite(rep->levelsIndex,sizeof(unsigned int),rep->nLevels+1,flist);
	fwrite(rep->rankLevels,sizeof(unsigned int),rep->nLevels,flist);
	//for(i=0;i<rep->nLevels;i++)
		//fprintf(stderr,"i:%d ranklevel: %d\n",i,rep->rankLevels[i]);
	fwrite(rep->levels,sizeof(unsigned char),rep->levelsIndex[rep->nLevels]/2,flist);
	//fwrite(rep->bS->data,sizeof(unsigned int),((rep->bS->n + W -1)/W) ,flist);	
	//fwrite(rep->bits_bitmap,sizeof(unsigned int),((rep->levelsIndex[rep->nLevels-1]+1 + W -1)/W),flist);
	save(rep->bS,flist);
	
//	char *filename2 = (char *) malloc(sizeof(char)*(strlen(filename)+4));
//  strcpy(filename2,filename);
//  strcat(filename2,".bm");
//  saveBitmap(filename2,rep->bS);

	
//	fprintf(stderr,"bitmap: %d, sumas: %d\n",sizeof(unsigned int) * rep->bS->sSize + sizeof(byte) * rep->bS->bSize,sizeof(unsigned int)*((rep->stoppersSize*rep->sumBits + W -1)/W));
//	unsigned int espSinEstr, espEstrDisco, espEstrMemoria, espEstr, espTotal;
//	espSinEstr =sizeof(unsigned int)*(rep->nLevels+1) + sizeof(byte)*rep->levelsIndex[rep->nLevels]/2+sizeof(unsigned int) *(rep->bS->n/W+1);
//	espEstrDisco = sizeof(byte) + sizeof(unsigned int)*7 + sizeof(unsigned int)*((rep->stoppersSize*rep->sumBits + W -1)/W)+sizeof(unsigned int) * rep->bS->sSize + sizeof(byte) * rep->bS->bSize;
//	espEstrMemoria = sizeof(unsigned int *)*2 + sizeof(byte *) +  sizeof(bitmap)-sizeof(unsigned int)*5;
//	espEstr = espEstrDisco + espEstrMemoria;
//	espTotal = espEstr + espSinEstr;
	//fprintf(stderr,"\n---\nSin estructuras ocupa: %d\n",espSinEstr);
	//fprintf(stderr,"Las estructuras adicionales ocupan: %d\n",espEstr);
	//fprintf(stderr,"Espacio total: %d, espacio del fichero: %d\n", espEstrDisco+espSinEstr,fileSize(filename)+fileSize(filename2));
	//fprintf(stderr,"Porcentaje de estructuras: %f sobre el total\n---\n", ((float)espEstr)*100/(4*rep->listLength));

	
}
FTRep* loadFT(FILE* flist){
	//int i;
	FTRep * rep = (FTRep *) malloc(sizeof(struct sFTRep));
	fread(&(rep->listLength),sizeof(unsigned int),1,flist);
	fread(&(rep->nLevels),sizeof(unsigned char),1,flist);
	rep->levelsIndex = (unsigned int *) malloc(sizeof(unsigned int)*(rep->nLevels+1));
	fread(rep->levelsIndex,sizeof(unsigned int),rep->nLevels+1,flist);
	rep->rankLevels = (unsigned int *) malloc(sizeof(unsigned int)*(rep->nLevels));
	fread(rep->rankLevels,sizeof(unsigned int),rep->nLevels,flist);
	//for(i=0;i<rep->nLevels;i++)
		//fprintf(stderr,"i:%d ranklevel: %d\n",i,rep->rankLevels[i]);
	rep->levels = (unsigned char *) malloc(sizeof(unsigned char)*rep->levelsIndex[rep->nLevels]/2);	
	fread(rep->levels,sizeof(unsigned char),rep->levelsIndex[rep->nLevels]/2,flist);
		
	//rep->bits_bitmap = (unsigned int *) malloc(sizeof(unsigned int)*((rep->levelsIndex[rep->nLevels-1]+1 + W -1)/W));
	//fread(rep->bits_bitmap,sizeof(unsigned int),((rep->levelsIndex[rep->nLevels-1]+1 + W -1)/W),flist);

	rep->bS = (bitRankW32Int *) malloc(sizeof(struct sbitRankW32Int));
	load(rep->bS,flist);	
	
	//fprintf(stderr,"levels:%d\n",rep->nLevels);
//	fprintf(stderr,"blockSize: %d. stoppersSize: %d, sumBits:%d\n",rep->blockSize,rep->stoppersSize,rep->sumBits);
//	char *filename2 = (char *) malloc(sizeof(char)*(strlen(filename)+4));
//  strcpy(filename2,filename);
//  strcat(filename2,".bm");
//  rep->bS = loadBitmap(filename2,rep->bits_bitmap,rep->levelsIndex[rep->nLevels-1]+1);


	
//	unsigned int espSinEstr, espEstrDisco, espEstrMemoria, espEstr, espTotal;
//	espSinEstr =sizeof(unsigned int)*(rep->nLevels+1) + sizeof(byte)*rep->levelsIndex[rep->nLevels]/2+sizeof(unsigned int) *(rep->bS->n/W+1);
//	espEstrDisco = sizeof(byte) + sizeof(unsigned int)*7 + sizeof(unsigned int)*((rep->stoppersSize*rep->sumBits + W -1)/W)+sizeof(unsigned int) * rep->bS->sSize + sizeof(byte) * rep->bS->bSize;
//	espEstrMemoria = sizeof(unsigned int *)*2 + sizeof(byte *) +  sizeof(bitmap)-sizeof(unsigned int)*5;
//	espEstr = espEstrDisco + espEstrMemoria;
//	espTotal = espEstr + espSinEstr;
	//fprintf(stderr,"\n---\nSin estructuras ocupa: %d\n",espSinEstr);
	//fprintf(stderr,"Las estructuras adicionales ocupan: %d\n",espEstr);
	//fprintf(stderr,"Espacio total: %d, espacio del fichero: %d\n", espEstrDisco+espSinEstr,fileSize(filename)+fileSize(filename2));
	//fprintf(stderr,"Porcentaje de estructuras: %f sobre el total\n---\n", ((float)espEstr)*100/(4*rep->listLength));


	return rep;
}

unsigned int sizeFT(FTRep* rep){
    return sizeof(FTRep) + sizeof(unsigned int)*(rep->nLevels+1) + sizeof(unsigned char)*rep->levelsIndex[rep->nLevels]/2 + sizeof(unsigned int)*rep->nLevels + sizeof(unsigned int)*((rep->levelsIndex[rep->nLevels-1]+1+W-1)/W) + spaceRequirementInBits(rep->bS)/8; 
}

}
}

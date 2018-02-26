
// implements canonical Huffman 

#include "huff.h"

typedef struct
   { uint freq;
     uint symb;
     union
       { int prev;
	 int depth;
       } h;
     int ch1,ch2;
   } Ttree;
  
static void sort (Ttree *tree, int lo, int up)

   { int i, j;
     Ttree temp;
     while (up>lo) 
        { i = lo;
	  j = up;
	  temp = tree[lo]; 
	  while (i<j) 
	     { while (tree[j].freq > temp.freq) j--;
	       tree[i] = tree[j]; 
	       while (i<j && tree[i].freq <= temp.freq) i++;
	       tree[j] = tree[i];
	     }
	  tree[i] = temp;
	  if (i-lo < up-i) { sort(tree,lo,i-1); lo = i+1; }
	  else { sort(tree,i+1,up); up = i-1; }
	}
   }

static void setdepths (Ttree *tree, uint node, int depth)

   { if (tree[node].ch1 == -1) // leaf
	{ tree[node].h.depth = depth; 
	  return;
	}
     setdepths (tree,tree[node].ch1,depth+1);
     setdepths (tree,tree[node].ch2,depth+1);
   }


THuff createHuff (uint *freq, uint lim, uint sorted)

   { THuff H;
     int i,j,d;
     Ttree *tree;
     int ptr,last,fre;
         // remove zero frequencies
     H.max = lim;
     H.total = 0;
     
     int onlyOneSymbol = (lim ==0);

	//     tree = malloc((2*(lim+1)-1)*sizeof(Ttree));
		/*** BEG FARI... para solucionar caso en que s�lo hubiese un �nico s�mbolo a codificar ****/
		 if (onlyOneSymbol) {
		 	tree = (Ttree *)malloc(3*sizeof(Ttree));  //root, the valid node (pos 0) and the "zeroNode" (pos 1)
		 	H.max =  lim+1;
		}
		 else 
		 	tree =  (Ttree *)malloc((2*(lim+1)-1)*sizeof(Ttree));	 	
	/*** END FARI... para solucionar caso en que s�lo hubiese un �nico s�mbolo a codificar ****/	
     
     //fprintf(stderr,"\n **** CALLED CREATE_HUFF WITH  lim = %ld, freq[0]=%ld",lim,freq[0]);     
     j = 0;
     for (i=0;i<=lim;i++) 
	{ if (freq[i]>0) 
	     { tree[j].freq = freq[i];   
               tree[j].symb = i;   
	       j++;
	       //fprintf(stderr,"\n freq[%d] = [%d], para s�mbolo %d, now j=%ld",i,freq[i],tree[j-1].symb,j);
	     }
	}
	
	/*** BEG FARI... para solucionar caso en que s�lo hubiese un �nico s�mbolo a codificar ****/
		if (onlyOneSymbol){	
			tree[j].freq = 0;			
			tree[j++].symb = 1; //<--- !!!!!!!!! (incrementa j !!!)
			
			//H.total -=1 * 1;     //we will add 1*1 at the end of this function.

		}
		 //fprintf(stderr,"\n");
     	
	/*** END FARI... para solucionar caso en que s�lo hubiese un �nico s�mbolo a codificar ****/	
	
	H.lim = lim = j-1;		

     
     	// now run Huffman algorithm
     if (!sorted) sort (tree,0,lim);
     	
     for (i=0;i<=lim;i++) {
       //fprintf(stderr,"\n    XX freq[%d] = [%d], para symbolo %d",i,tree[i].freq,tree[i].symb);     	
       tree[i].h.prev = i+1;
	   tree[i].ch1 = tree[i].ch2 = -1;
	 }
     tree[lim].h.prev = -1;
    // last = next node to process, ptr = search point, fre = next free cell
	// leaves are in 0..lim in decreasing freq order
	// internal nodes are in lim+1.. 2*lim, created in incr. fre order
     last=0; ptr = 0; fre = lim+1;
     for (i=0;i<lim;i++)
	{ tree[fre].ch1 = last;
	  last = tree[last].h.prev;
	  tree[fre].ch2 = last;
	  tree[fre].freq = tree[tree[fre].ch1].freq+tree[tree[fre].ch2].freq;
	  while ((tree[ptr].h.prev != -1) &&
		 (tree[tree[ptr].h.prev].freq <= tree[fre].freq))
	      ptr = tree[ptr].h.prev;
	  tree[fre].h.prev = tree[ptr].h.prev;
	  tree[ptr].h.prev = fre;
	  last = tree[last].h.prev;
	  fre++;
	}
        // now assign depths recursively
     setdepths (tree,2*lim,0);
     H.s.spos = (uint *)malloc ((H.max+1)*sizeof(uint));
     for (i=0;i<=H.max;i++) H.s.spos[i] = ~0;
     H.num = (uint *)malloc ((lim+1)*sizeof(uint)); // max possible depth
     d=0;
     for (i=lim;i>=0;i--)
	{ H.s.spos[tree[i].symb] = i;
	  while (tree[i].h.depth > d) 
	      { H.num[d] = i+1; d++; }
	}
     H.num[d] = 0;
     H.depth = d;
     for (d=H.depth;d>0;d--) H.num[d] = H.num[d-1] - H.num[d];
     H.num[0] = (lim == 0);
     H.num = (uint *) realloc(H.num,(H.depth+1)*sizeof(uint));
     //H.total = 0;
     
     
     
     	
     if (onlyOneSymbol){	H.total += freq[tree[0].symb] * tree[0].h.depth;}
     else {
	     for (i=0;i<=lim;i++) {
	       H.total += ((size_t)freq[tree[i].symb])* tree[i].h.depth;
	       //fprintf(stderr,"\n         ****tota[%d] = %d x %d =%d",i,freq[tree[i].symb],tree[i].h.depth,H.total);
	    }
	}
     free (tree);

     //fprintf(stderr,"\n **** CALL ENDS CREATE_HUFF WITH  lim = %ld, freq[0]=%ld, Huffsize=%ld",lim,freq[0],H.total); 

     return H;
   }

size_t encodeHuff (THuff H, uint symb, uint *stream, size_t ptr)

   { uint pos;
     uint code;
     int d;
     pos = H.s.spos[symb];
     code = 0;
     d = H.depth; 
     while (pos >= H.num[d])
       { code = (code + H.num[d]) >> 1;
         pos -= H.num[d--];
       }
     code += pos;
     if (d > W) { bitzero(stream,ptr,d-W); ptr += d-W; d = W; }
     while (d--)
        { if ((code >> d) & 1) bitset(stream,ptr);
	  else bitclean(stream,ptr);
	  ptr++;
	}
     return ptr;
   } 

void printCodeHuff (THuff H, uint symb)

   { uint pos;
     uint code;
     int d;
     pos = H.s.spos[symb];
     code = 0;
     d = H.depth;
     
    // fprintf(stderr,"\n H.depth= %ld and pos is %ld\n",H.depth,pos); 
     while (pos >= H.num[d])
       { code = (code + H.num[d]) >> 1;
         pos -= H.num[d--];
       }
     code += pos;
     if (d > W) {fprintf(stderr,"code larger than W"); d=W;}
   
   	 //show the code.
     while (d--)
        { if ((code >> d) & 1) 
        	fprintf(stderr,"1");
	      else fprintf(stderr,"0");
		}
   } 



size_t decodeHuff (THuff *H, uint *symb, uint *stream, size_t ptr)

   { uint pos;
     int d;
     pos = 0;
     d = 0;
     while (pos < H->fst[d])
        { pos = (pos << 1) | bitget(stream,ptr);
          ptr++; d++;
        }
     *symb = H->s.symb[H->num[d]+pos-H->fst[d]];
     return ptr;
   }




/*   { uint pos;  // This "improved" code is actually slower!
     int d;
     uint wrd,off;
     stream += ptr/W;
     off = ptr & (W-1);
     wrd = *stream >> off;
     pos = 0;
     d = 0;
     while (pos < H.fst[d])
       { pos = (pos << 1) | (wrd & 1);
	 d++; wrd >>= 1; off++;
	 if (off == W) { wrd = *++stream; off = 0; }
       }
     *symb = H.s.symb[H.num[d]+pos-H.fst[d]];
     return ptr+d;
   } 
*/
void saveHuff (THuff H, FILE *f)

   { uint *symb = (uint *)malloc((H.lim+1)*sizeof(uint));
     int i;
     for (i=0;i<=H.max;i++) 
	 	if (H.s.spos[i] != ~0) symb[H.s.spos[i]] = i;
     fwrite (&H.max,sizeof(uint),1,f); 
     fwrite (&H.lim,sizeof(uint),1,f); 
     fwrite (&H.depth,sizeof(uint),1,f); 
     fwrite (symb,sizeof(uint),H.lim+1,f); 
     fwrite (H.num,sizeof(uint),H.depth+1,f); 
     free (symb);
   }

uint sizeHuff (THuff H)

   { return (4 +(H.lim+1)+2*(H.depth+1))*sizeof(uint);
   }

uint sizeHuffDisk (THuff H)

   { return ( sizeof(THuff) + ((H.lim+1)+(H.depth+1))*sizeof(uint) );
   }

void freeHuff (THuff H)

  { free (H.s.spos);  free (H.num); free (H.fst);
  }


THuff loadHuff (FILE *f, int enc)   //enc (0/1)-> do you only  want to perform encoding ??

   { THuff H;
     uint *symb;
     uint *num;
     int i,d,dold,dact;
     fread (&H.max,sizeof(uint),1,f); 
     fread (&H.lim,sizeof(uint),1,f); 
     fread (&H.depth,sizeof(uint),1,f); 
     symb = (uint *) malloc ((H.lim+1)*sizeof(uint));
     fread (symb,sizeof(uint),H.lim+1,f); 
     if (enc) 
          { H.s.spos = (uint *) malloc ((H.max+1)*sizeof(uint));
            for (i=0;i<=H.max;i++) H.s.spos[i] = ~0;
            for (i=0;i<=H.lim;i++) H.s.spos[symb[i]] = i;
            free (symb);
	  }
     else H.s.symb = symb;
     	
     H.num = (uint *) malloc ((H.depth+1)*sizeof(uint));
     fread (H.num,sizeof(uint),H.depth+1,f); 
     if (!enc) 
          { H.fst = (uint *) malloc ((H.depth+1)*sizeof(uint));
            H.fst[H.depth] = 0; dold = 0;
            for (d=H.depth-1;d>=0;d--)
	        { dact = H.num[d+1];
		  H.fst[d] = (H.fst[d+1]+dact) >> 1;
		  H.num[d+1] = dold;
		  dold += dact;
		}
	    H.num[0] = dold;
	  }
     return H;
   }     



  

/***************************************************************/     
void prepareToDecode(THuff *H)   
//***///**//  //by fari !!	

   { uint *symb = (uint *) malloc((H->lim+1)*sizeof(uint));
     uint *num;
     int i,d,dold,dact;

     for (i=0;i<=H->max;i++) 
	 	if (H->s.spos[i] != ~0) 
	 		symb[H->s.spos[i]] = i;
	 		
	 for (i=0;i<=H->lim;i++) 
	 	H->s.symb[i] = symb[i];
     	
     //H.num = malloc ((H.depth+1)*sizeof(uint));
     {
		H->fst = (uint *)malloc ((H->depth+1)*sizeof(uint));
		H->fst[H->depth] = 0; dold = 0;
		for (d=H->depth-1;d>=0;d--)
		{ 	dact = H->num[d+1];
			H->fst[d] = (H->fst[d+1]+dact) >> 1;
			H->num[d+1] = dold;
		  	dold += dact;
		}
	    H->num[0] = dold;
	 } 
     free (symb);	 
}    


////////////     
void saveHuffAfterDecode (THuff H, FILE *f)     
   { 
     fwrite (&H.max,sizeof(uint),1,f); 
     fwrite (&H.lim,sizeof(uint),1,f); 
     fwrite (&H.depth,sizeof(uint),1,f); 
     fwrite (H.s.symb,sizeof(uint),H.lim+1,f); 
     
     fwrite (H.fst,sizeof(uint),H.depth+1,f); 
     fwrite (H.num,sizeof(uint),H.depth+1,f); 
   }


THuff loadHuffAfterDecode (FILE *f, int enc)   //enc (0/1)-> do you only  want to perform encoding ??

   { THuff H;
//     int i,d,dold,dact;
//
//     fread (&H.max,sizeof(uint),1,f); 
//     fread (&H.lim,sizeof(uint),1,f); 
//     fread (&H.depth,sizeof(uint),1,f); 
//
//     H.s.symb = malloc ((H.lim+1)*sizeof(uint));
//     fread (H.s.symb,sizeof(uint),H.lim+1,f); 
// 
//     H.fst = malloc ((H.depth+1)*sizeof(uint));
//     fread (H.fst,sizeof(uint),H.depth+1,f);
//
//     H.num = malloc ((H.depth+1)*sizeof(uint));
//     fread (H.num,sizeof(uint),H.depth+1,f); 
//     
     return H;
   }   
	


void loadHuffAfterDecode2 (THuff *H, FILE *f, int enc)   //enc (0/1)-> do you only  want to perform encoding ??

   { 
     int i,d,dold,dact;

     fread (&H->max,sizeof(uint),1,f); 
     fread (&H->lim,sizeof(uint),1,f); 
     fread (&H->depth,sizeof(uint),1,f); 

     H->s.symb = (uint *) malloc ((H->lim+1)*sizeof(uint));
     fread (H->s.symb,sizeof(uint),H->lim+1,f); 
 
     H->fst = (uint *) malloc ((H->depth+1)*sizeof(uint));
     fread (H->fst,sizeof(uint),H->depth+1,f);

     H->num = (uint *) malloc ((H->depth+1)*sizeof(uint));
     fread (H->num,sizeof(uint),H->depth+1,f); 
   }   
	
     
     



// implements canonical Huffman 

#include "huffDec.h"


//THuff createHuff (uint *fst, uint *num, uint depth) {
//	THuff H;
//	H.fst = fst;
//	H.num = num;
//	H.depth = depth;
//	return H;	
//}


void printCodeHuffDec (THuffDec H, uint symb)

   { uint pos;
     uint code;
     int d;
     //pos = H.s.spos[symb];
     pos = symb;
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



//Decodes a code starting in position ptr from stream. Returns the ranking in the
//vector of symbols.
int decodeHuffDec (THuffDec *H, uint *symb, uint *stream, uint ptr)
   { uint pos;
     int d;
     pos = 0;
     d = 0;
     while (pos < H->fst[d])
        { pos = (pos << 1) | bitget(stream,ptr);
          ptr++; d++;
        }
     *symb = H->num[d]+pos-H->fst[d];
     return ptr;
   }



// //Decodes a code starting in position ptr from stream. Returns the ranking in the
// //vector of symbols.
// int decodeHuffDecVariantWord (uint *fst , uint *symb, uint *stream, uint ptr, uint depth)
//    { uint pos;
//      int d;
//      pos = 0;
//      d = 0;
//      while ((d< depth) && (pos < fst[d*2])  )
//         { pos = (pos << 1) | bitget(stream,ptr);
//           ptr++; d++;
//         }
// 	if (depth==d) 
// 		*symb = pos;
// 	else 
// 		*symb = 
// 			pos  - 
// 			fst[d*2] + 
// 			fst[d*2+1]; 
//      //(*symb) = (depth==d) ? pos : fst[d*2+1] + pos - fst[d*2]; 
// 
//      return ptr;
//    }



// the bytes used by HuffDecman struct
uint sizeHuffDec (THuffDec H)
   { return (1+ 2*(H.depth+1))*sizeof(uint);
   }


void freeHuffDec (THuffDec H)

  { free (H.fst); free (H.num);
  }


THuffDec loadHuffDecAfterDecode (FILE *f, int enc)   //enc (0/1)-> do you only  want to perform encoding ??

   { THuffDec H;
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
	
	
////THuffDec loadHuffDec (FILE *f, int enc)
////
////   { THuffDec H;
////     uint *symb;
////     uint *num;
////     int i,d,dold,dact;
////     fread (&H.max,sizeof(uint),1,f); 
////     fread (&H.lim,sizeof(uint),1,f); 
////     fread (&H.depth,sizeof(uint),1,f); 
////     symb = malloc ((H.lim+1)*sizeof(uint));
////     fread (symb,sizeof(uint),H.lim+1,f); 
////     if (enc) 
////          { H.s.spos = malloc ((H.max+1)*sizeof(uint));
////            for (i=0;i<=H.max;i++) H.s.spos[i] = ~0;
////            for (i=0;i<=H.lim;i++) H.s.spos[symb[i]] = i;
////            free (symb);
////	  }
////     else H.s.symb = symb;
////     H.num = malloc ((H.depth+1)*sizeof(uint));
////     fread (H.num,sizeof(uint),H.depth+1,f); 
////     if (!enc) 
////          { H.fst = malloc ((H.depth+1)*sizeof(uint));
////            H.fst[H.depth] = 0; dold = 0;
////            for (d=H.depth-1;d>=0;d--)
////	        { dact = H.num[d+1];
////		  H.fst[d] = (H.fst[d+1]+dact) >> 1;
////		  H.num[d+1] = dold;
////		  dold += dact;
////		}
////	    H.num[0] = dold;
////	  }
////     return H;
////   }	


void loadHuffDecAfterDecode2 (THuffDec *H, FILE *f, int enc)   //enc (0/1)-> do you only  want to perform encoding ??

   { 
     int i,d,dold,dact;

//     fread (&H->max,sizeof(uint),1,f); 
//     fread (&H->lim,sizeof(uint),1,f); 
     fread (&H->depth,sizeof(uint),1,f); 

//     H->s.symb = malloc ((H->lim+1)*sizeof(uint));
//     fread (H->s.symb,sizeof(uint),H->lim+1,f); 
 
     H->fst = (uint *) malloc ((H->depth+1)*sizeof(uint));
     fread (H->fst,sizeof(uint),H->depth+1,f);

     H->num = (uint *) malloc ((H->depth+1)*sizeof(uint));
     fread (H->num,sizeof(uint),H->depth+1,f); 
   }   
	
     
     
     


     


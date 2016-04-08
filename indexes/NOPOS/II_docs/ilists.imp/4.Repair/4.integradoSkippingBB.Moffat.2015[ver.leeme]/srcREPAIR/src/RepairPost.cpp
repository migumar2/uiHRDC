

#include "RepairPost.h"
				

	//**contructor that initializes object for querying
	RepairPost::RepairPost(char * file) {
		il = &ail;
		readConstants(file);
		readSampleK(file);  ///2015!
		load(file);
		readLenPostings(file);
		
		load_bitmap(file);  /** bitmap spire 2009 */	
		
		createAux(file);	///2015!		
	}	


/** Number of phrases that where generated during "repair" for the node v |  v \in [1..nodes] **/
uint RepairPost::lenList(uint v){
	uint x,y;
	v--;
	x = GetField(ptrs, plen, v); 	//symbols_new[rank_aux]);
	y = GetField(ptrs, plen, v+1);  //symbols_new[rank_aux]);
	return y-x;
}

/** return the number of bits used by each posting list (compressed) **/
uint RepairPost::lenListBits(uint v){

	if (shouldUseBitmap(v)) {
		return(il->bits_bitmaps);
	}

	uint x,y;
	v--;
	x = GetField(ptrs, plen, v); 	//symbols_new[rank_aux]);
	y = GetField(ptrs, plen, v+1);  //symbols_new[rank_aux]);
	uint len = y-x;
	uint numbitsList = len * bits_sn;
	return numbitsList;
}


//loads the structure containing the bitmap-type posting lists.
int RepairPost::load_bitmap(char *basename) {
	int file;
	char *filename;	
	filename = (char *)malloc(sizeof(char)*(strlen(basename)+20));
	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, "bitmappost");
	fprintf(stderr,"\nLoading  bitmap-type postings: %s\n", filename);
	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	fprintf(stdout,"\n file %s opened sucessfully",filename);
	
		
	/** Loading the posting lists using bitmaps.*/
	{	ssize_t rf;
		rf= read(file, &(il->lenBitmapThreshold), sizeof(uint));
	//	fprintf(stdout,"\n read: [%d]",il->lenBitmapThreshold);		exit(0);
		rf= read(file, &(il->bits_bitmaps), sizeof(uint));		
		rf= read(file, &(il->ints_bitmaps_aligned), sizeof(uint));	
		
		rf= read(file, &(il->numBitmaps), sizeof(uint));
		fprintf(stdout,"\n read: [%d][%d][%d][%d]",il->lenBitmapThreshold, il->	bits_bitmaps,il->ints_bitmaps_aligned , il->numBitmaps);	
		fflush(stderr);
		if (il->numBitmaps) {
			il->zoneBitmaps = (uint *) malloc (sizeof(uint) * ( il->numBitmaps * il->ints_bitmaps_aligned));
			il->zoneBitmaps[( il->numBitmaps * il->ints_bitmaps_aligned) -1] =0000; //so valgrind is shut up		
			rf= read(file, il->zoneBitmaps, ( il->numBitmaps * il->ints_bitmaps_aligned) *  sizeof(uint));
		}
		else il->zoneBitmaps=NULL;
	}

/*	
	{
		
//			uint * ptr2 = new uint[nodes+2];   //  longitud = nodes + 2 !!
//	for(uint i=0;i<nodes+1;i++) {
//		ptr2[i] = BR->rank(GetField(ptrs,plen,i))-1;    //** ptr2[i] = rank(BR,ptr[i]);  == nº símbolos "finales" hasta la posición del ptr[i] en la seq comprimida
//	} 
		//uint * occsBitmap = (uint*)malloc(sizeof(uint)*(((nodes+2)+WW32-1)/WW32));
		uint *occsBitmap = new uint [(((nodes+2)+WW32-1)/WW32)];
		for(uint i=0; i<(((nodes+2)+WW32-1)/WW32);i++)
			occsBitmap[i]=0;

		for(uint i=1; i<=nodes;i++) {
			if (shouldUseBitmap(i)) {
				bitset(occsBitmap,i +1 ); 
				// ** adds +1 to the actual position so that given Bitmap[0]=0, Bitmap[1]=1 (the first node),
				//  *  Rank(1) == 0 ==> 0 is the index in "zoneBitmaps" (as zoneBitmaps starts at position "0").
				// * *
			}
			
		}
		
		BR_ptr_bitmap = new BitRankW32Int(occsBitmap, nodes+2 , true, 20);	
//		cerr<< "\nRAnk en pos 1 =" <<BR_ptr_bitmap->rank(1);
//		cerr<< "\nRAnk en pos 2 =" <<BR_ptr_bitmap->rank(2);
//		cerr<< "\nRAnk en pos 3 =" <<BR_ptr_bitmap->rank(3);
//		cerr<< "\nRAnk en pos 100000 =" <<BR_ptr_bitmap->rank(100000);
//		

		
	}
*/	
	
	
	close(file);	
	free(filename);
	return 0;
 }

//** size needed by the posting represented as bitmaps **/
 uint RepairPost::bitmap_size(){
 	uint total = (sizeof(uint) * ( il->numBitmaps * il->ints_bitmaps_aligned));
 	return total;
}

 int RepairPost::free_bitmap () {
	if (il->zoneBitmaps)
		free(il->zoneBitmaps);
	return 0;
}


int RepairPost::shouldUseBitmap(uint id) {  /* 1 <= id <= il->nlists */
	if (lenPost[id] > il->lenBitmapThreshold)
		return 1;
	return 0;
}


	
void RepairPost::load(char * file) {
  int error=0;
  char * fname = new char[strlen(file)+50];

  sprintf(fname,"%s.crp",file);
  FILE * fp = fopen64(fname,"r");
  assert(fp!=NULL);
  
  
  size_t rf;
  rf= fread(&min_value,sizeof(uint),1,fp);
  rf= fread(&max_value,sizeof(uint),1,fp);
  rf= fread(&max_assigned,sizeof(uint),1,fp);
  rf= fread(&nbits, sizeof(uint), 1, fp);
  rf= fread(&m,sizeof(uint),1,fp);
  rf= fread(&n,sizeof(uint),1,fp);
  symbols = new uint[(m/32*nbits+m%32*nbits/32+1)];
  uint read = 0;
  while((read+=fread(symbols+read,sizeof(uint),1,fp))!=(m/32*nbits+m%32*nbits/32+1));
  BR = new BitRankW32Int(fp,&error);   //** se usará para calcular ptr, y después se desecha **
	
	cerr << "\n m vale = " << m << " n vale = " << n <<"\n" ;
  assert(error==0);
  fclose(fp);


  //** CARGAMOS EL DICCIONARIO COMPRIMIDO....
  sprintf(fname,"%s.cdict",file);
  fp = fopen64(fname,"r");
  rf= fread(&symbols_new_len,sizeof(uint),1,fp);
  symbols_new = new uint[symbols_new_len];
  rf= fread(symbols_new,sizeof(uint),symbols_new_len,fp);
  BRR = new BitRankW32Int(fp,&error);
  assert(error==0);
  fclose(fp);




  /**************************************/
  /** usar symbols_new y BBR para crear lo siguiente el vector RSS
           -- RSS = vector c_symb con skip-info, pero sin compactar.
               * para ello usar métodos : expand_build_Len(v)  o  expand_build_Gap(v)
    				- val = expand_build (v) //usa symbols_new en lugar de c_symb
    				- val = expand_build (v).
   */
   
   uint n = BRR->n-1;
    {   
	    RSS = new uint[n];  //después será compactado en c_symb.

    	uint skip_gap, skip_terminals;
		info = (uint *) malloc ((maxLenPosting+5) * sizeof(uint));		
    	uint pos_symbols_new=0;
    	
    	for (uint i=0; i<n;i++) {
    		if (! BRR->IsBitSet(i)){ //a terminal value
    			RSS[i] = symbols_new[pos_symbols_new++];    			
    		}
    		else {
				pos = 0;
				info[0] = 0;
				expand_build(i + max_value +1);     //** CALCULATES SKIP_INFO !!! for non-terminal "i + (maxvalue+1)"
				info[0]=pos;     		
				skip_gap = info[pos]; 
				skip_terminals = pos; //number of expanded terminals
				RSS[i] = skip_gap;
    		}
    	} 
    	
       	free(info);	
       	cerr<< "\n\n RSS[] array has been built !!!!\n";

	}      

	//** Compacting RSS --> creating c_symb_RS
	 symbols_new_len = n;
	 delete [] symbols_new;
	 symbols_new = RSS;

   /*************************************/    			

	uint max_sn = 0;
	for(uint i=0;i<symbols_new_len;i++)       //** calcula el máximo símbolo en symbols_new;
		max_sn = max(max_sn,symbols_new[i]);
	bits_sn = bits(max_sn);                   //** numero de bits necesarios para los valores en "symbols_new"

	csymb_n = new uint[symbols_new_len*bits_sn/WW32+1];   //** valores de symbols_new codificados con "bits_sn" bits.

	for(uint i=0;i<symbols_new_len;i++)
		SetField(csymb_n, bits_sn, i, symbols_new[i]);  //** copio en csymb_n los valores de symbols_new.
	delete [] symbols_new;
	
  	cerr<< "\n\n RSS[] array has been compacted with skip_info into c_symb !!!!\n";
	
	//** hasta aquí, respecto al Cdict... 
	//**   -csymb_n[]:  son los símbolos que había en symbols_new[], pero compactados.
	//**   -bits_sn: es el número de bits con que se representa cada entero en csymb_n
	//**   -symbols_new_len: es el número de symbolos en el vocabulario
	//**   -BBR es el bitmap que indica si hay un terminal (1) o un símbolo-interno (0);
	
  sprintf(fname,"%s.ptr",file);
  fp = fopen64(fname,"r");
  if(fp==NULL)
  {
    printf("Error abriendo el archivo %s\n",fname);
  }
  int r;
 
  rf= fread(&r,sizeof(int),1,fp);
  rf= fread(&nodes,sizeof(uint),1,fp);
  rf= fread(&edges,sizeof(uint),1,fp);
  //uint plen = bits(edges+1);
  //uint ptrs_len = (nodes+1)*plen/WW32 +1;
  //uint * ptrs = new uint[ptrs_len];
  plen = bits(edges+1);
  ptrs_len = (nodes+1)*plen/WW32 +1;  
  ptrs = new uint[ptrs_len];
  
  rf= fread(ptrs,sizeof(uint),ptrs_len,fp);
  fclose(fp);
  
	uint * ptr2 = new uint[nodes+2];   //** longitud = nodes + 2 !!
	for(uint i=0;i<nodes+1;i++) {
		ptr2[i] = BR->rank(GetField(ptrs,plen,i))-1;    //** ptr2[i] = rank(BR,ptr[i]);  == nº símbolos "finales" hasta la posición del ptr[i] en la seq comprimida
	}  
  	ptr2[nodes+1] = m;                                  //** ptr2[nodes+1]= m == nº símbolos "finales" en la seq comprimida "completa" ;)
	delete [] ptrs;
	delete BR;
	
	plen = bits (m+1);
	ptrs_len = (nodes +2) * plen/WW32 +1;
	ptrs = new uint[ptrs_len];
	for (uint i=0; i<=nodes+1;i++) {
		SetField(ptrs,plen,i,ptr2[i]);
	}
	delete [] ptr2;
 

  e=edges;
  shift=min_value+r;
  
  
  //**Creates the bitmap BMR as BRR without rank structures.
  BMR = new BitMapW32Int(BRR->data,n,true);
  BRR->owner = false;
  delete BRR;

	//info = (uint*)malloc(MAX_INFO*sizeof(uint));  //allocated as is needed.
  delete [] fname;  
}

RepairPost::~RepairPost() {
  delete [] symbols;
  delete [] csymb_n;
  delete BMR;
  delete [] ptrs;
  
  //delete BR_ptr_bitmap;

  //2015! sampling
  free(occs);
  delete []zoneOccs;


  delete [] lenPost;  /** len of each original posting-list */
  free_bitmap(); 
}


/** Implements expand method, using "symbols_new uint-array" */
/** Used during "load" of RepairPost structure               */
void RepairPost::expand_build(uint v) {
	if(v <= max_value) {
		pos++;                       //** pos dentro de "info".
		info[pos] = v+info[pos-1];   //** suma lo anterior (d-gaps)
		return;
	}
	//**  else // a non-terminal node
	uint aux1 = v-max_value-1;      //** no es un símbolo terminal. ==> 
	assert(BRR->IsBitSet(aux1));
	uint aux2=1;                    //** descomprimir hasta que aux2 ==0;   (mientras haya más 1's que 0's)
	uint rank_aux = aux1-BRR->rank(aux1)+1;
	
	while(aux2!=0) {
		if(BRR->IsBitSet(aux1))
			aux2++;
		else {
			aux2--;
			//expand_build(GetField(csymb_n, bits_sn, rank_aux)); //symbols_new[rank_aux]);
			expand_build(symbols_new[rank_aux]);			
			rank_aux++;
		}
		aux1++; 
	}
}


void RepairPost::readConstants(char *file) {
	
	{  //** reads some constants: maxLenPosting and sizeUncompressed from "convert program".
	  char * fname = new char[strlen(file)+50];
	
	  sprintf(fname,"%s.const",file);
	  FILE * fp = fopen64(fname,"r");
	  assert(fp!=NULL);
	  size_t rf;
	  rf= fread(&sizeUncompressed,sizeof(size_t),1,fp); //Len of uncompressed set of lists (including lenlist)
	  rf= fread(&maxLenPosting,sizeof(uint),1,fp);    //max len of any posting list. ==> needed for malloc on info[]
	  fclose(fp);
	  delete []fname;
	  cerr << "\n sizeUncompressed is " << sizeUncompressed << " and maxLenPosting is " << maxLenPosting;

	}
}



/********************* for compatibility with search_re and search_il *************/
void RepairPost::readSampleK(char *file) {
	
	{  //** reads some constants: maxLenPosting and sizeUncompressed from "convert program".
	  char * fname = new char[strlen(file)+50];
	
	  sprintf(fname,"%s.const.samplek",file);
	  FILE * fp = fopen64(fname,"r");
	  assert(fp!=NULL);
	  size_t rf;
	  //rf= fread(&sampleK,sizeof(uint),1,fp);    //max len of any posting list. ==> needed for malloc on info[]
  	  rf=fscanf(fp,"sampleK=%u",&sampleK);

	  fclose(fp);
	  delete []fname;
	  cerr << "\n sampleK (read-from-disk) is " << sampleK ;
	  fflush(stderr);

	}
}
/*
static void writeSampleK(char *file, uint sampleK) {
	
	{  //  reads some constants: maxLenPosting and sizeUncompressed from "convert program".
	  char * fname = new char[strlen(file)+50];
	
	  sprintf(fname,"%s.const.samplek",file);
	  unlink (fname);
	  FILE * fp = fopen64(fname,"w");
	  assert(fp!=NULL);
	  size_t rf;
	  rf= fwrite(&sampleK,sizeof(uint),1,fp);    //max len of any posting list. ==> needed for malloc on info[]
	  fclose(fp);
	  delete []fname;
	  cerr << "\n sampleK (writen-to-disk) is " << sampleK ;

	}
}
*/
/********************* for compatibility with search_re and search_il *************/

void RepairPost::readLenPostings(char *file) {
	
	{  //** reads some constants: maxLenPosting and sizeUncompressed from "convert program".
	  char * fname = new char[strlen(file)+50];
	
	  lenPost = new uint [nodes+1]; lenPost[0]=0;
	  sprintf(fname,"%s.lens",file);
	  FILE * fp = fopen64(fname,"r");
	  assert(fp!=NULL);
	  size_t rf = fread(&(lenPost[1]),sizeof(uint),nodes,fp); //Len of Posting-lists for id \in [1..nodes]
	  fclose(fp);
	  delete []fname;
	  cerr << "\n loaded lenPosts[] array";

	}
}




//**creates the sampling structure.
void RepairPost::createAux(char * file) {		  //DONE BY GONZALO !!! 
	
	//load_bitmap(file); 
	
	info = (uint *) malloc ((maxLenPosting+5) * sizeof(uint));
	occs = (uint *) malloc (sizeof(uint) * (nodes +1)); occs[0]=0;

	//** computing the number of AbsSamples to keep.
	uint counterAbsSamples = 0;
	uint countBitmaps =0;
	{
		uint v, n, len, sampleP;
		for (uint i=1;i<=nodes;i++) {
	
			if (!shouldUseBitmap(i)) {  /** spire **/
				v=i;
				len = lenList(v);
				SAMPLEPERIOD(sampleP,sampleK,len);
				//cout << "\n SAMPLE PERIOD = "<< sampleP << " "; fflush(stdout);
				n = (len +sampleP -1)/sampleP; //number of samples to keep.	
				counterAbsSamples += n ;
			}
			else {
				//printf("lenList[%d] = %d",i,lenList(i));
				countBitmaps++;
			}
		}
	}
	
	uint *zoneOccsTmp = new uint[counterAbsSamples];
	//** computes the values of the absSamples to keep.
	{
		uint maxAbsValue =0;
		uint i,currOcc=0;
		uint v,  n, len, sampleP;
		uint currBitmap=0;
		for (i=1; i<= nodes; i++) {	
			
			if (!shouldUseBitmap(i)) {  /** spire **/
				uint x,y;
				v=i-1;
				x = GetField(ptrs, plen, v);     //symbols_new[rank_aux]);
				y = GetField(ptrs, plen, v+1);   //symbols_new[rank_aux]);
				len = y-x;
				//len = lenList(v);

				occs[i]=currOcc;
				SAMPLEPERIOD(sampleP,sampleK,len);
				n = (len +sampleP -1)/sampleP; //number of samples to keep for current term.	

				pos = 0;      //** curr pos inside info[]
				info[0] = 0;
				

				uint k;
				for(k=x;k<y;){
					expand(GetField(symbols,nbits,k)); k++;
					info[0]=info[pos];  //**sum of previous dgaps.
					pos=0;
					if ((k==y)||!((k-x) % sampleP)) {
						zoneOccsTmp[currOcc++] = info[0];
						if (maxAbsValue < info[0]) maxAbsValue = info[0];
					}
					
				}
			}
			else {
				occs[i] = currBitmap++;
			}
		}		

		/* Compact zoneOccs <-- zoneOccsTmp */
		bits_zoneOccs = bits(maxAbsValue);
		zoneOccsSize = counterAbsSamples;
		zoneOccs = new uint [((zoneOccsSize * bits_zoneOccs)/32+1)];
		for (i =0 ; i< zoneOccsSize; i++) {
			SetField(zoneOccs,bits_zoneOccs,i,zoneOccsTmp[i]);
		}
		delete [] zoneOccsTmp;					
	}

	free(info);
	// zoneOccs = zoneOccsTmp; 
	cerr << "\n ACABO CREATE !!!" ;			
}
	


/** Calculates the size of array lenPost if it were encoded in a k-bit-fashion **/
uint RepairPost::sizetobitsLenPost() {
	 uint maxs, bits_s; uint total;
	
	// size of lenList
	maxs=0;
	for(uint i=1;i<=nodes;i++)       //** calcula el mayor valor
		maxs = max(maxs,lenPost[i]);
	bits_s = bits(maxs);                  
	total = ((nodes+1) * bits_s / WW32 +1)*sizeof(uint);   //** valores codificados con "bits_s" bits.
	return total;
}

uint RepairPost::sizetobitsOccs() {
	uint maxs, bits_s; uint total;
	
	maxs=0;
	maxs = occs[nodes];
	bits_s = bits(maxs);                   //** numero de bits necesarios para los valores en "symbols_new"
	total = ((nodes+1) * bits_s / WW32 +1)*sizeof(uint);   //** valores de symbols_new codificados con "bits_sn" bits.
	return total;
}

uint RepairPost::sizetobits() {
	 uint maxs, bits_s;
	uint total;
	
	// size of lenList
	maxs=0;
	for(uint i=1;i<=nodes;i++)       //** calcula el mayor valor
		maxs = max(maxs,lenPost[i]);
	bits_s = bits(maxs);                  
	total = ((nodes+1) * bits_s / WW32 +1)*sizeof(uint);   //** valores codificados con "bits_s" bits.

	maxs=0;
	maxs = occs[nodes];
	bits_s = bits(maxs);                   //** numero de bits necesarios para los valores en "symbols_new"
	total += ((nodes+1) * bits_s / WW32 +1)*sizeof(uint);   //** valores de symbols_new codificados con "bits_sn" bits.

	return total;
}

uint RepairPost::size() {
  uint sum = 0;

	sum += sizeof(uint) * ptrs_len;                     // ptrs  (in bits)	                    
  	sum += sizeof(uint)*(m/32*nbits+m%32*nbits/32+1);   // compressed sequence
  	sum += BMR->SpaceRequirementInBits()/8;             // Bitmap BMR
	sum += (symbols_new_len*bits_sn/WW32+1)*sizeof(uint);  // c_symb[]

	//sum += sizeof(uint) * (nodes +1);                   //**  lenPost array.
	//sum += sizetobits();
	sum += sizetobitsLenPost() + sizetobitsOccs();

	//** sampling!
	sum += 	((zoneOccsSize * bits_zoneOccs)/WW32+1) * 4;	  //** zoneoccs encoded with bits.


	cerr<< "\n ptrs_size ="<<ptrs_len*4 ;
	cerr<< "\n compressed seq="<<sizeof(uint)*(m/32*nbits+m%32*nbits/32+1);
	cerr<< "\n Bitmap Rb (ya quitadas estructs rank)=" << (BMR->SpaceRequirementInBits()/8) << " bytes";
		cerr<< "\n Rs (dic)=" << (symbols_new_len*bits_sn/WW32+1)*sizeof(uint);
	cerr<< "\n LenPosting[] array (lens of each posting OLD!!) = " << 4 * (nodes +1);
	cerr<< "\n LenPosting[] array (lens of each posting BITS) = " << sizetobitsLenPost();
	cerr<< "\n  occs[] array (coded with BITS) = " << sizetobitsOccs();	
	cerr<< "\n  LenPosting[] + occs array (lens of each posting BITS) = " << sizetobitsLenPost() + sizetobitsOccs();
	
	cerr<< "\n VOCAB SIZE = ptrs[] + lenList[] = " << sizetobits() + (sizeof(uint) * ptrs_len) << " bytes\n";
	cerr << "\n ZONEOCCS "<< ((zoneOccsSize * bits_zoneOccs)/WW32+1) * 4 ;

	uint totalBitmaps = bitmap_size();
	
	cerr<< "\n  @size Postings as bitmaps ="<<totalBitmaps << " bytes" ;
	cerr<< "\n  @Number of Bitmaps="<< il->numBitmaps << ", bitmapLenListThreshold="<<il->lenBitmapThreshold;
	cerr<< "\n  @Bytes per bitmap = " <<  il->ints_bitmaps_aligned * sizeof(uint)  << "\n";
	
	//cerr<< "\n  @Rank struct for bitVector+ bitVector " << BR_ptr_bitmap->SpaceRequirementInBits()/8 ; 	
	//sum +=  BR_ptr_bitmap->SpaceRequirementInBits()/8 ;
	
	sum+= totalBitmaps;	
	fflush (stderr);
  return sum;
}

 

/********************************************************************************/


uint * RepairPost::adj_il(int v, uint *noccs) {

	//a list encoded with a bitmap.
	if ( shouldUseBitmap(v) ) {
		//v--;v++;
		register uint len  = lenPost[v];
		uint *info = (uint *) malloc (sizeof(uint) * (len));	
		
		//@uint inipos= BR_ptr_bitmap->rank(v);
		
		uint inipos = occs[v] * il->ints_bitmaps_aligned * WW32; //pos in zoneBitmaps.
		//@inipos *= (il->ints_bitmaps_aligned * WW32); //pos in zoneBitmaps.
		
		uint endpos = inipos + il->bits_bitmaps;
		uint *zoneBitmaps = il->zoneBitmaps;
		uint i;
		
		len=0;		
		for (i=inipos; i<endpos; i++) {
			if (bitget(zoneBitmaps,i)) 
				info[len++] = i-inipos -GAP;
		}
		*noccs = len;
		return info;	
	}	
	
	
	info = (uint *) malloc ((maxLenPosting+5) * sizeof(uint));	
	v--;   //** v tendrá valor entre 1+ m, pero "symbols se indexa desde el "0" (symbols es la secuencia comprimida)

	uint i = GetField(ptrs, plen, v); //symbols_new[rank_aux]);
	uint j = GetField(ptrs, plen, v+1);   //symbols_new[rank_aux]);
	
	pos = 0;
	prevval = 0 - GAP;  // as extract must return values between 0.. n-1, but repair indexes 1..n
	//info[0] = 0;
	for(uint k=i;k<j;k++)
		expand_il(GetField(symbols,nbits,k));

	//info[0]=pos;  //** info[0] guarda el número de "elementos" que han sido expandidos = valor actual de pos.
	*noccs = pos;
  return info;  
}

void RepairPost::expand_il(uint v) {
	if(v <= max_value) {
		info[pos] = v+ prevval;   //** suma lo anterior (d-gaps)
		prevval = info[pos];
		pos++;                    //** pos dentro de "info".
		return;
	}
	//**  else // a non-terminal node
	uint aux1 = v-max_value-1;      //** no es un símbolo terminal. ==> 
	assert(BMR->IsBitSet(aux1));
	uint aux2=1;                    //** descomprimir hasta que aux2 ==0;   (mientras haya más 1's que 0's)
	while(aux2!=0) {
		if(BMR->IsBitSet(aux1)){
		//if(BMR->IsBitSet(rank_aux))
			aux2++;
		}
		else {
			aux2--;
			//while (BMR->IsBitSet(rank_aux)) rank_aux++;  ///!!!!!! No problem, pues el último valor en RSS siempre será un terminal.						
			expand_il(GetField(csymb_n, bits_sn, aux1)); //expand(RSS[rank_aux]); //symbols_new[rank_aux]);
		}
		aux1++;
	}
}


/*******************************************************************************/




uint * RepairPost::adj(int v) {

	//a list encoded with a bitmap.
	if ( shouldUseBitmap(v) ) {
		//v--; //**FARI CAMBIADO EL 2013/08/28  (debe estar descomentado para lenPost[v] y BR_ptr_bitmap->rank(v)
		register uint len  = lenPost[v];
		uint *info = (uint *) malloc (sizeof(uint) * (len+1));	
		
		//@uint inipos= BR_ptr_bitmap->rank(v);
		
		uint inipos = occs[v] * il->ints_bitmaps_aligned * WW32; //pos in zoneBitmaps.
		//@inipos *= (il->ints_bitmaps_aligned * WW32); //pos in zoneBitmaps.
		
		uint endpos = inipos + il->bits_bitmaps;
		uint *zoneBitmaps = il->zoneBitmaps;
		uint i;
		
		len=0;		
		for (i=inipos; i<endpos; i++) {
			if (bitget(zoneBitmaps,i)) 
				info[++len] = i-inipos;
		}
		info[0]=len;
		return info;	
	}	
	
	
	info = (uint *) malloc ((maxLenPosting+5) * sizeof(uint));	
	v--;   //** v tendrá valor entre 1+ m, pero "symbols se indexa desde el "0" (symbols es la secuencia comprimida)

	uint i = GetField(ptrs, plen, v); //symbols_new[rank_aux]);
	uint j = GetField(ptrs, plen, v+1);   //symbols_new[rank_aux]);
	
	pos = 0;
	info[0] = 0;
  for(uint k=i;k<j;k++)
		expand(GetField(symbols,nbits,k));
	info[0]=pos;  //** info[0] guarda el número de "elementos" que han sido expandidos = valor actual de pos.
  return info;  
}

void RepairPost::expand(uint v) {
	if(v <= max_value) {
		pos++;                       //** pos dentro de "info".
		info[pos] = v+info[pos-1];   //** suma lo anterior (d-gaps)
		return;
	}
	//**  else // a non-terminal node
	uint aux1 = v-max_value-1;      //** no es un símbolo terminal. ==> 
	assert(BMR->IsBitSet(aux1));
	uint aux2=1;                    //** descomprimir hasta que aux2 ==0;   (mientras haya más 1's que 0's)
	while(aux2!=0) {
		if(BMR->IsBitSet(aux1)){
		//if(BMR->IsBitSet(rank_aux))
			aux2++;
		}
		else {
			aux2--;
			//while (BMR->IsBitSet(rank_aux)) rank_aux++;  ///!!!!!! No problem, pues el último valor en RSS siempre será un terminal.						
			expand(GetField(csymb_n, bits_sn, aux1)); //expand(RSS[rank_aux]); //symbols_new[rank_aux]);
		}
		aux1++;
	}
}



	/* Fsearch, uncompressing the whole posting-list before seq. search  */
	   
inline int RepairPost::fsearchBitmap (uint id, uint value) {
	//fprintf(stderr,"\n Call to FsearchBitmap");

		/** searches for "value" inside list idList 
			 returns 0 if not found,
			        >0 if found
		 */
		
				//uint inipos= BR_ptr_bitmap->rank(v);
		
		//uint inipos = occs[v] * il->ints_bitmaps_aligned * WW32; //pos in zoneBitmaps.			
		//	register uint pos = occs[id] * il->ints_bitmaps_aligned * WW32 +value;
		
	//@register uint pos = BR_ptr_bitmap->rank(id);
	//@pos *= il->ints_bitmaps_aligned * WW32;
	uint pos = occs[id] * il->ints_bitmaps_aligned * WW32; //pos in zoneBitmaps.	
			
	pos += value;
	if (bitget((il->zoneBitmaps),pos)) return 1;	
	return 0;	
}

	/* Intersects "nids" bitmap-represented posting lists whose ids are given by ids[].
	    Returns *occs (the result), noccs (number of elements)
	    Memory for *occs is allocated, and must be freed by the caller 
	    note: nids >=2
	*/
//int RepairPost::mergeNBitmap (uint *ids, uint nids, uint *noccs, uint **occs);
int RepairPost::mergeNBitmap (uint *ids,  uint nids, uint *noccs, uint **occurs){
	//fprintf(stderr,"\n Call to mergeNBitmap");
	
	register uint nints = il->ints_bitmaps_aligned;
	register uint i,j; 

	uint *bitmapAND = (uint *) malloc (nints * sizeof(uint));  //uints for the resulting bitmap.
	bitmapAND[nints-1] = 0000;

	//intesects the first 2 bitmaps (bit-parallel)
	{	
		//@uint inipos0 = 	BR_ptr_bitmap->rank(ids[0]);
		//@uint inipos1 = 	BR_ptr_bitmap->rank(ids[1]);

		uint inipos0 = 	occs[ids[0]];
		uint inipos1 = 	occs[ids[1]];
		
		uint *bitmap1 = (uint *) il->zoneBitmaps + (inipos0 * (il->ints_bitmaps_aligned));
		uint *bitmap2 = (uint *) il->zoneBitmaps + (inipos1 * (il->ints_bitmaps_aligned));
		
		//uint *bitmap1 = (uint *) il->zoneBitmaps + (occs[ids[0]] * (il->ints_bitmaps_aligned));
		//uint *bitmap2 = (uint *) il->zoneBitmaps + (occs[ids[1]] * (il->ints_bitmaps_aligned));
		
		for (i=0;i< nints; i++) {
			bitmapAND[i] = bitmap1[i] & bitmap2[i];			
		}
	}
	
	//intersects the remainder (nids-2) bitmaps.
	{
		uint *bitmap;
		uint inipos;
		for (j=2;j<nids;j++) {
			//@inipos = BR_ptr_bitmap->rank(ids[j]);
			inipos = occs[ids[j]];
			
			bitmap = (uint *) il->zoneBitmaps + inipos * il->ints_bitmaps_aligned;
			//bitmap = (uint *) il->zoneBitmaps + occs[ids[j]] * il->ints_bitmaps_aligned;
			for (i=0;i< nints; i++) {
				bitmapAND[i] &= bitmap[i];
			}
		}
	}
	
	//generates the resulting list of integers.
	{
		register uint counter =0;
			//reserves memory for "at most" the smallest? list.
		uint *resultList = (uint *) malloc (sizeof(uint) * lenPost[ids[0]]); 
		
		j=il->bits_bitmaps;
		
		for (i=0;i<j;i++) {
			if (bitget(bitmapAND, i)) 
				resultList[counter++]=i -GAP;
		}
		
	/*
	 	if (!counter){      //intersect_2() and intersect_N() would free memory.
			free (resultList);
			resultList=NULL;
		}
	*/
		*occurs = resultList;
		*noccs= counter;
	}
		
	free(bitmapAND);
	return 0;
}

/*
int RepairPost::int2Skipping(uint id1, uint id2, uint *noccs, uint **occs){
	
	if (shouldUseBitmap(id1) && shouldUseBitmap(id2)) {
		uint nids = 2;
		uint ids[2];
		ids[0]=id1; ids[1]=id2;
		int err =  mergeNBitmap (&ids[0], nids, noccs, occs);
		return err;
	}
	
		
	register uint n;
	register uint first, second;  //small and longest posting.


	if (lenPost[id1] <= lenPost[id2]) 
		{ first =id1; second=id2;}
	else  
		{ first =id2; second=id1;}

	result = adj(first);
	
	n= result[0];     //number of elements in the first list
	matches =0;

	if (shouldUseBitmap(second)){	
		int found;
		register uint j;
		
		for (j=1; j<=n;j++) {
			found = fsearchBitmap(second, result[j]); //call to fsearch;
			//fprintf(stderr,"\n  Search for value %d -> found = %d",result[j], found);
			if (found) {
				result[matches++] = result[j]-GAP;;
			}
		}
		*occs  = result;
		*noccs = matches;
		return 0;
	}
	


	currVal =0;	  //curValue expanded in the 2nd list currently.
	fi=1;   //ini non-processed part of the 1st list
	fl=1;   //end non-processed part of the 1st list

	{
		uint v = second -1;
		uint ii = GetField(ptrs, plen, v); //symbols_new[rank_aux]);
		uint j  = GetField(ptrs, plen, v+1); //symbols_new[rank_aux]);
    	uint k, symb2nd, val2nd;

       	for (k=ii; k<j; k++){ //for all elements in the second list...
    		if (fi>n) break;						
    		symb2nd = GetField(symbols,nbits,k);  // the k-th element of the 2nd list
    
    		if (symb2nd <= max_value) { // a terminal value --> no expansion is needed.	
    			
				currVal += symb2nd;  //the actual value	
    			
			  	while ( (fi <= n) && (result[fi] < currVal) ) fi++;
				fl= fi;	
				if (fi>n) break;
			
				if (result[fi] == currVal) {
    				result[matches++] = currVal-GAP;;
    				fi++; fl=fi;
    			}
    		}
    		else { // it is a phrase
    			
    			 val2nd = currVal + GetField(csymb_n, bits_sn, (symb2nd-max_value-1)); //uses skipping-info.    			 
	    		//val2nd is the value after k-th phrase is processed (from the posting of the 2nd list).
	    		//currValue is the starting value before that k-th phrase.
	    		    			
    			if (val2nd > result[fi]) {  //processing is needed    		
					while ( (fl <= n) && (result[fl] <= val2nd) ) fl++;	
					fl--;    					
					
					expandAndSearchSkipping (symb2nd); //currValue, fi, fl, result, 
					fi=fl+1; 
					fl=fi;
				}
				else if (val2nd == result[fi]) {
					result[matches++] = val2nd-GAP;;					
    				//fi++; fl=fi;	
				}
									
				currVal = val2nd;
			}    			    			
    	}    		    		
	}	
	
	//if (matches == 0) {free(firstlist); firstlist = NULL;}	
	*occs  = result;
	*noccs = matches;					
		
	return 0;	
}	

*/



/*** searches for a searchedValue inside the posting list of term/node "v"    ***/
/*
void RepairPost::expandAndSearchSkipping22 (uint v) { //, uint *iniabsValue, uint searchedValue) {

	uint aux1 = v-max_value-1;      // ** no es un símbolo terminal. ==> 
	assert(BMR->IsBitSet(aux1));
	uint aux2=1;                    // ** descomprimir hasta que aux2 ==0;  
	while(aux2!=0) {                // ** (mientras haya más 1's que 0's)
		if (fi>fl) return;
			
		if(BMR->IsBitSet(aux1)){
			aux2++;
		}
		else {
			aux2--;	
			{
				uint symb_id = GetField(csymb_n, bits_sn, aux1);
			
				if (symb_id>max_value) {
    			 	uint val2nd = currVal + GetField(csymb_n, bits_sn, (symb_id-max_value-1)); 
    			 	                                                    //uses skipping-info.
					if (val2nd >= result[fi]) 									
						expandAndSearchSkipping(symb_id); //symbols_new[rank_aux]);	
				    currVal = val2nd;	
				}
				else { // a terminal-value...
//					expandAndSearchSkipping(symb_id); //symbols_new[rank_aux]);	

						currVal += symb_id;
						if (currVal != result[fi]) {
							if (currVal > result[fi]){
								while ((fi<=fl) && (currVal > result[fi])) fi++;
								if ((fi<=fl) && (currVal == result[fi])) {
									result[matches++]= currVal-GAP;;
	
									fi++; 									
								}
							}	
						}
						else {
							//  value was found::   needed for the first recursive loop
							result[matches++]= currVal-GAP;;
							fi++; 						
						}
				}// else-- a terminal value.
			}		
		}
		aux1++;
	}
}
*/

int RepairPost::int2Skipping(uint id1, uint id2, uint *noccs, uint **occs){
	
	if (shouldUseBitmap(id1) && shouldUseBitmap(id2)) {
		uint nids = 2;
		uint ids[2];
		ids[0]=id1; ids[1]=id2;
		int err =  mergeNBitmap (&ids[0], nids, noccs, occs);
		return err;
	}
	
		
	register uint n;
	register uint first, second;  //small and longest posting.


	if (lenPost[id1] <= lenPost[id2]) 
		{ first =id1; second=id2;}
	else  
		{ first =id2; second=id1;}

	result = adj_il(first,&n);
	
	//n= result[0];     //number of elements in the first list
	matches =0;

	if (shouldUseBitmap(second)){	
		int found;
		register uint j;
		
		for (j=0; j<n;j++) {
			found = fsearchBitmap(second, result[j]+GAP); //call to fsearch;
			//fprintf(stderr,"\n  Search for value %d -> found = %d",result[j], found);
			if (found) {
				result[matches++] = result[j];;//-GAP;;
			}
		}
		*occs  = result;
		*noccs = matches;
		return 0;
	}
	


	currVal =0;	  //curValue expanded in the 2nd list currently.
	fi=0;   //ini non-processed part of the 1st list
	fl=0;   //end non-processed part of the 1st list

	{
		uint v = second -1;
		uint ii = GetField(ptrs, plen, v); //symbols_new[rank_aux]);
		uint j  = GetField(ptrs, plen, v+1); //symbols_new[rank_aux]);
    	uint k, symb2nd, val2nd;

       	for (k=ii; k<j; k++){ //for all elements in the second list...
    		if (fi>=n) break;						
    		symb2nd = GetField(symbols,nbits,k);  // the k-th element of the 2nd list
    
    		if (symb2nd <= max_value) { // a terminal value --> no expansion is needed.	
    			
				currVal += symb2nd;  //the actual value	
    			
			  	while ( (fi < n) && ((result[fi]+GAP) < currVal) ) fi++;
				fl= fi;	
				if (fi>=n) break;
			
				if ((result[fi]+GAP) == currVal) {
    				result[matches++] = currVal-GAP;
    				fi++; fl=fi;
    			}
    		}
    		else { // it is a phrase
    			
    			 val2nd = currVal + GetField(csymb_n, bits_sn, (symb2nd-max_value-1)); //uses skipping-info.    			 
	    		//val2nd is the value after k-th phrase is processed (from the posting of the 2nd list).
	    		//currValue is the starting value before that k-th phrase.
	    		    			
    			if (val2nd > (result[fi]+GAP)) {  //processing is needed    		
					while ( (fl < n) && ((result[fl]+GAP) <= val2nd) ) fl++;	
					fl--;    					
					
					expandAndSearchSkipping (symb2nd); //currValue, fi, fl, result, 
					fi=fl+1; 
					fl=fi;
				}
				else if (val2nd == (result[fi]+GAP)) {
					result[matches++] = val2nd-GAP;				
    				//fi++; fl=fi;	
				}
									
				currVal = val2nd;
			}    			    			
    	}    		    		
	}	
	
	//if (matches == 0) {free(firstlist); firstlist = NULL;}	
	*occs  = result;
	*noccs = matches;					
		
	return 0;	
}	


int RepairPost::int2SkippingSeq(uint id1, uint id2, uint *noccs, uint **occcs){ 
	//printf("\n call 2 int2skippingSeq\n");
	
	if (shouldUseBitmap(id1) && shouldUseBitmap(id2)) {
		uint nids = 2;
		uint ids[2];
		ids[0]=id1; ids[1]=id2;
		int err =  mergeNBitmap (&ids[0], nids, noccs, occcs);
		return err;
	}
	
		
	register uint n;
	register uint first, second;  //small and longest posting.


	if (lenPost[id1] <= lenPost[id2]) 
		{ first =id1; second=id2;}
	else  
		{ first =id2; second=id1;}


	register uint len = lenList(second);
	register uint sampleP;
	SAMPLEPERIOD(sampleP,sampleK,len);	

	result = adj_il(first,&n);
	
	//n= result[0];     //number of elements in the first list
	matches =0;

	if (shouldUseBitmap(second)){	
		int found;
		register uint j;
		
		for (j=0; j<n;j++) {
			found = fsearchBitmap(second, result[j]+GAP); //call to fsearch;
			//fprintf(stderr,"\n  Search for value %d -> found = %d",result[j], found);
			if (found) {
				result[matches++] = result[j];;//-GAP;;
			}
		}
		*occcs  = result;
		*noccs = matches;
		return 0;
	}
	


	currVal =0;	  //curValue expanded in the 2nd list currently.
	fi=0;   //ini non-processed part of the 1st list
	fl=0;   //end non-processed part of the 1st list

	{
		uint v = second -1;
		uint ii = GetField(ptrs, plen, v); //symbols_new[rank_aux]);
		uint j  = GetField(ptrs, plen, v+1); //symbols_new[rank_aux]);
    	uint k, symb2nd, val2nd;

		uint poss,val;
		poss = occs[second]; //poss in zoneoccs	


       	for (k=ii; k<j; k++){ //for all elements in the second list...
    		if (fi>=n) break;	

			/** Searching in the samples */	
			
			/*seq search*/
    		uint nextSample = (k-ii)/sampleP;
			val = GetField(zoneOccs, bits_zoneOccs, poss + nextSample);  
			
		// 	uint prevkk = k;  // @TEST SAMPLING IS USED
			
			while (val < (result[fi]+GAP)) {
				currVal = val; k = ii+(++nextSample)*sampleP; //incremento k sólo si val < result[fi]
				if (k >= j) break;
				val = GetField(zoneOccs, bits_zoneOccs, poss + nextSample);	
			}
			
		//	if (k-prevkk)	cerr << "[s" << k-prevkk << "]";	// @TEST SAMPLING IS USED

			
			if (k >= j) break;

			/** after searching in the samples **/    							
    		symb2nd = GetField(symbols,nbits,k);  // the k-th element of the 2nd list
    
    		if (symb2nd <= max_value) { // a terminal value --> no expansion is needed.	
    			
				currVal += symb2nd;  //the actual value	
    			
			  	while ( (fi < n) && ((result[fi]+GAP) < currVal) ) fi++;
				fl= fi;	
				if (fi>=n) break;
			
				if ((result[fi]+GAP) == currVal) {
    				result[matches++] = currVal-GAP;
    				fi++; fl=fi;
    			}
    		}
    		else { // it is a phrase
    			
    			 val2nd = currVal + GetField(csymb_n, bits_sn, (symb2nd-max_value-1)); //uses skipping-info.    			 
	    		//val2nd is the value after k-th phrase is processed (from the posting of the 2nd list).
	    		//currValue is the starting value before that k-th phrase.
	    		    			
    			if (val2nd > (result[fi]+GAP)) {  //processing is needed    		
					while ( (fl < n) && ((result[fl]+GAP) <= val2nd) ) fl++;	
					fl--;    					
					
					expandAndSearchSkipping (symb2nd); //currValue, fi, fl, result, 
					fi=fl+1; 
					fl=fi;
				}
				else if (val2nd == (result[fi]+GAP)) {
					result[matches++] = val2nd-GAP;				
    				//fi++; fl=fi;	
				}
									
				currVal = val2nd;
			}    			    			
    	}    		    		
	}	
	
	//if (matches == 0) {free(firstlist); firstlist = NULL;}	
	*occcs  = result;
	*noccs = matches;					
		
	return 0;	
}




int RepairPost::int2SkippingExp(uint id1, uint id2, uint *noccs, uint **occcs) {
	//printf("\n call 2 int2skippingExp\n");
	
	if (shouldUseBitmap(id1) && shouldUseBitmap(id2)) {
		uint nids = 2;
		uint ids[2];
		ids[0]=id1; ids[1]=id2;
		int err =  mergeNBitmap (&ids[0], nids, noccs, occcs);
		return err;
	}
	
		
	register uint n;
	register uint first, second;  //small and longest posting.


	if (lenPost[id1] <= lenPost[id2]) 
		{ first =id1; second=id2;}
	else  
		{ first =id2; second=id1;}


	register uint len = lenList(second);
	register uint sampleP;
	SAMPLEPERIOD(sampleP,sampleK,len);	

	result = adj_il(first,&n);
	
	//n= result[0];     //number of elements in the first list
	matches =0;

	if (shouldUseBitmap(second)){	
		int found;
		register uint j;
		
		for (j=0; j<n;j++) {
			found = fsearchBitmap(second, result[j]+GAP); //call to fsearch;
			//fprintf(stderr,"\n  Search for value %d -> found = %d",result[j], found);
			if (found) {
				result[matches++] = result[j];;//-GAP;;
			}
		}
		*occcs  = result;
		*noccs = matches;
		return 0;
	}
	


	currVal =0;	  //curValue expanded in the 2nd list currently.
	fi=0;   //ini non-processed part of the 1st list
	fl=0;   //end non-processed part of the 1st list

	{
		uint v = second -1;
		uint ii = GetField(ptrs, plen, v); //symbols_new[rank_aux]);
		uint j  = GetField(ptrs, plen, v+1); //symbols_new[rank_aux]);
    	uint k, symb2nd, val2nd;

		uint poss,val;
		poss = occs[second]; //poss in zoneoccs	


       	for (k=ii; k<j; k++){ //for all elements in the second list...
    		if (fi>=n) break;	

			/** Searching in the samples */	
			if (val < (result[fi]+GAP)) {			
				/*exp search*/
			
				uint nextSample = (k-ii)/sampleP;
				uint prevk=k, probe = 1;
				val = GetField(zoneOccs, bits_zoneOccs, poss + nextSample);  
				
				while (val < (result[fi]+GAP) ) {
					currVal = val; 
					prevk= ii+(nextSample+1)*sampleP;
					nextSample += probe;
					k = ii+(nextSample)*sampleP; //incremento k sólo si val < result[fi]
					if (k >= j) break;
					val = GetField(zoneOccs, bits_zoneOccs, poss + nextSample);	
					probe *=2;
				}	

		
				register uint l,r,m;
				val = currVal;
				l = (prevk-ii) / sampleP;
				if (k>=j) r = (len-1)/sampleP;
				else r = (k - ii)/sampleP;
				k=prevk;
					
				{
					register uint iniL = l;
					//l++;
					//r--;
					m=l;
					
					while (r > l+1) {
						m = (l + r) / 2;	
						val = GetField(zoneOccs, bits_zoneOccs, poss + m);
						if (val < (result[fi]+GAP) ) l = m;
						else r = m;					
					}
					
					if (l != iniL) {				
						if (val < (result[fi]+GAP)) {
								k= ii + (m+1)*sampleP;		
								currVal =  GetField(zoneOccs, bits_zoneOccs, poss + m);					
						}
						else {
							k= ii + (l+1)*sampleP;		
							currVal =  GetField(zoneOccs, bits_zoneOccs, poss + l);		
						}					 				
					}
				}	

				/**pequeña optimización ???  evita resto bucle si coinciden **/
				/**fari 2008.12.26 **/
				if (val == (result[fi]+GAP)) {
					k++;
					currVal = val;
					result[matches++] = currVal-GAP;
					fi++; fl=fi;    
					continue;					
				}									
			}
			
			if (k >= j) break;

			/** after searching in the samples **/    							
    		symb2nd = GetField(symbols,nbits,k);  // the k-th element of the 2nd list
    
    		if (symb2nd <= max_value) { // a terminal value --> no expansion is needed.	
    			
				currVal += symb2nd;  //the actual value	
    			
			  	while ( (fi < n) && ((result[fi]+GAP) < currVal) ) fi++;
				fl= fi;	
				if (fi>=n) break;
			
				if ((result[fi]+GAP) == currVal) {
    				result[matches++] = currVal-GAP;
    				fi++; fl=fi;
    			}
    		}
    		else { // it is a phrase
    			
    			 val2nd = currVal + GetField(csymb_n, bits_sn, (symb2nd-max_value-1)); //uses skipping-info.    			 
	    		//val2nd is the value after k-th phrase is processed (from the posting of the 2nd list).
	    		//currValue is the starting value before that k-th phrase.
	    		    			
    			if (val2nd > (result[fi]+GAP)) {  //processing is needed    		
					while ( (fl < n) && ((result[fl]+GAP) <= val2nd) ) fl++;	
					fl--;    					
					
					expandAndSearchSkipping (symb2nd); //currValue, fi, fl, result, 
					fi=fl+1; 
					fl=fi;
				}
				else if (val2nd == (result[fi]+GAP)) {
					result[matches++] = val2nd-GAP;				
    				//fi++; fl=fi;	
				}
									
				currVal = val2nd;
			}    			    			
    	}    		    		
	}	
	
	//if (matches == 0) {free(firstlist); firstlist = NULL;}	
	*occcs  = result;
	*noccs = matches;					
		
	return 0;	
}



/*** searches for a searchedValue inside the posting list of term/node "v"    ***/
uint RepairPost::expandAndSearch (uint v, uint *iniabsValue, uint searchedValue) {
	if(v <= max_value) {
		pos++;
		if (!(pos == 1)) {   //**no sumo el d-gap del valor que esta sampleado
			(*iniabsValue) += v;
		}
		if (*iniabsValue != searchedValue) return 0; //** needed for the first recursive loop
		return 1;  //** value was found::   needed for the first recursive loop
	}
	uint aux1 = v-max_value-1;      //** no es un simbolo terminal. ==> 
	assert(BMR->IsBitSet(aux1));
	uint aux2=1;                    //** descomprimir hasta que aux2 ==0;   (mientras haya mas 1's que 0's)
	while(aux2!=0) {
		if(BMR->IsBitSet(aux1)){
			aux2++;
		}
		else {
			aux2--;
			expandAndSearch(GetField(csymb_n, bits_sn, aux1), iniabsValue, searchedValue); //symbols_new[rank_aux]);
			//expandAndSearch(RSS[rank_aux], iniabsValue, searchedValue); //symbols_new[rank_aux]);
			if (*iniabsValue > searchedValue) return 0; 
			else if (*iniabsValue == searchedValue) return 1;
			//** else continues through remaining values.
		}
		aux1++;
	}
	return 0;
}


/*** searches for a searchedValue inside the posting list of term/node "v"    ***/

void RepairPost::expandAndSearchSkipping (uint v) { //, uint *iniabsValue, uint searchedValue) {

	uint aux1 = v-max_value-1;      // ** no es un símbolo terminal. ==> 
	assert(BMR->IsBitSet(aux1));
	uint aux2=1;                    // ** descomprimir hasta que aux2 ==0;  
	while(aux2!=0) {                // ** (mientras haya más 1's que 0's)
		if (fi>fl) return;
			
		if(BMR->IsBitSet(aux1)){
			aux2++;
		}
		else {
			aux2--;	
			{
				uint symb_id = GetField(csymb_n, bits_sn, aux1);
			
				if (symb_id>max_value) {
    			 	uint val2nd = currVal + GetField(csymb_n, bits_sn, (symb_id-max_value-1)); 
    			 	                                                    //uses skipping-info.
					if (val2nd >= (result[fi]+GAP) ) 									
						expandAndSearchSkipping(symb_id); //symbols_new[rank_aux]);	
				    currVal = val2nd;	
				}
				else { // a terminal-value...
//					expandAndSearchSkipping(symb_id); //symbols_new[rank_aux]);	

						currVal += symb_id;
						if (currVal != (result[fi]+GAP) ) {
							if (currVal > (result[fi]+GAP) ){
								while ((fi<=fl) && (currVal > (result[fi]+GAP) )) fi++;
								if ((fi<=fl) && (currVal == (result[fi]+GAP) )) {
									result[matches++]= currVal-GAP;;
	
									fi++; 									
								}
							}	
						}
						else {
							//** value was found::   needed for the first recursive loop
							result[matches++]= currVal-GAP;;
							fi++; 						
						}
				}// else-- a terminal value.
			}		
		}
		aux1++;
	}
}

/** Checking it works ...
    Computes the las value of the posting-list of node "node", without recursion, just using skip-info
 **/
void RepairPost::showLastValueSkipping(uint node) {
			
    	cerr << "\n Last value of list, using skipping info && no sampling ::" ;

		uint v = node -1;
		uint i = GetField(ptrs, plen, v); //symbols_new[rank_aux]);
		uint j = GetField(ptrs, plen, v+1); //symbols_new[rank_aux]);
    	
    	uint currValue = 0;
    	uint k, symb;
    	for (k=i; k<j;k++) {
    		symb = GetField(symbols,nbits,k);
    		if (symb <= max_value)
    			currValue += symb;  //the actual value
    		else 
    			//currValue += (symb-max_value-1); //skip_info
    			currValue += GetField(csymb_n, bits_sn, (symb-max_value-1));
    	}

		cerr << "[" << currValue <<"]\n"; 
}


/*
inline int RepairPost::fsearchUnc_usando_adj_il (uint id, uint value) {
	//fprintf(stderr,"\n Call to FsearchUnc %u :: %u", id, value);

		// searches for "value" inside list idList 
		//	 returns 0 if not found,
		//	        >0 if found
		//	 !!very slow if a very frequent id is needed.
		//
	uint *list;
	register uint i;
	uint len;
	list = adj_il(id,&len);
	
	for (i=0;i<len;i++){
		if (list[i] == value) {
			free(list);
			return 1;			
		}
	} 
	free(list);				
	return 0;	
}

*/
inline int RepairPost::fsearchUnc (uint id, uint value) {
	//fprintf(stderr,"\n Call to FsearchUnc %u :: %u", id, value);

		/** searches for "value" inside list idList 
			 returns 0 if not found,
			        >0 if found
			 !!very slow if a very frequent id is needed.
		 */
	uint *list;
	register uint i;
	uint len;
	//list = adj_il(id,&len);
	list=adj(id);
	len=list[0];
	
	//if (len != lenPost[id]) printf("\n FALLA LEN = %u %u %u, %u",len, lenPost[id-1], lenPost[id], lenPost[id+1]);
	
	for (i=1;i<=len;i++){
		if (list[i] == value) {
			free(list);
			return 1;			
		}
	} 
	free(list);				
	return 0;	
}

	/*  Intersects 2 posting-lists using set-versus-set                 */
	/*   Returns *occs (the result), noccs (number of elements)         */
	/*   Memory for *occs is allocated, and must be freed by the caller */
	    
int RepairPost::svs2Unc (uint id1, uint id2, uint *noccs, uint **occs){
	
	if (shouldUseBitmap(id1) && shouldUseBitmap(id2)) {
		uint nids = 2;
		uint ids[2];
		ids[0]=id1; ids[1]=id2;
		int err =  mergeNBitmap (&ids[0], nids, noccs, occs);
		return err;
	}
			
	register uint n;
	register uint first, second;  //small and longest posting.


	if (lenPost[id1] <= lenPost[id2]) 
		{ first =id1; second=id2;}
	else  
		{ first =id2; second=id1;}

	result = adj_il(first,&n);
	
	//n= result[0];     //number of elements in the first list
	matches =0;

	if (shouldUseBitmap(second)){	
		int found; register uint j;
		
		for (j=0; j<n;j++) {
			found = fsearchBitmap(second, result[j]+GAP); //call to fsearch;
			//fprintf(stderr,"\n  Search for value %d -> found = %d",result[j], found);
			if (found) {
				result[matches++] = result[j]; //-GAP;;
			}
		}
		*occs  = result;
		*noccs = matches;
		return 0;
	}
	else {
		int found; register uint j;
		for (j=0; j<n;j++) {
			found = fsearchUnc(second, result[j]+GAP); //call to fsearch;
			//fprintf(stderr,"\n  Search for value %d -> found = %d",result[j], found);
			if (found) {
				result[matches++] = result[j]; //-GAP;;
			}
		}
		*occs  = result;
		*noccs = matches;
		return 0;
	}
		
	//return 0;
}



/***************  GALLOPING SEARCH ****************/

char * RepairPost::showFirst(uint id, uint n) {
	static char msg[100000];
	uint l;
	uint  *res;
	res = adj_il(id,&l);
	
	int i;
	
	sprintf(msg,"\n list %u -->",id);
	for (i=0; i<n;i++) {
		sprintf(msg,"%s,[%u]",msg,res[i]);
	}
	return msg;	
}

inline int RepairPost::fsearchGal (uint id, uint value) {
	//fprintf(stderr,"\n Call to FsearchGalloping");
// cerr << "\n****### CALL TO FSEARCHGAL VALUE "<< value << " for id " << id  ;

	register uint len = lenList(id);
	register uint sampleP;
	SAMPLEPERIOD(sampleP,sampleK,len);
	
	register uint i,n; //,r
	uint poss,  val;

	poss = occs[id]; //poss in zoneoccs	
	n = (len +sampleP -1)/sampleP; //number of samples to seq search.			
	
	/** Searching in the samples */	

	uint l,r,m;
	r=l=0;
	i=1;
	while (r<n) {
		val = GetField(zoneOccs, bits_zoneOccs, poss+r);
		if (val < value) {l=r; r=i; i<<=1;}
		else if (val==value) 
			return 1;
		else {break;}		
	}
		
	r= min (r,n);
	while (r>(l+1)) {
		m = (l + r) / 2;	
		val = GetField(zoneOccs, bits_zoneOccs, poss+m);
		if (val < value) l = m;
		else if (val == value) return 1;
		else r = m;
	}

    i = l;  //** the sampled position to start decoding from.	
	
	//fprintf(stderr,"\n i= %d",i);
	//** decoding from the i-th sampled value is needed (search on samples did not find the searched value) **
	{
    	uint val =  GetField(zoneOccs, bits_zoneOccs, occs[id]+i); //value of the prev sample.
    	//uint val;
    	//if (i==0) val = 0;
    	//else 
    //		val =  GetField(zoneOccs, bits_zoneOccs, occs[id]+i); //value of the prev sample.
    	
		
		uint v = id -1;
		uint ii = GetField(ptrs, plen, v); //symbols_new[rank_aux]);
		ii += i*sampleP;  //beginning offset...
		uint j = GetField(ptrs, plen, v+1); //symbols_new[rank_aux]);
    	j= min (j, ii+sampleP);
    	pos = 0;    	  //needed to avoid adding the d-gap of the sampled position.
    	info[0] = val;
    	uint k;
    	for (k=ii; k<j;k++) {
			//expand(GetField(symbols,nbits,k));		
			if (expandAndSearch (GetField(symbols,nbits,k), &val, value) ) return 1;
			//else {
			//	printf("\n val %u",val);
			//	return 0;
			//	}
		}

	}

	return 0;	
}


//fst:[45][56][83][89][100][129][133][142][167][191]
//f2nd:[58][59][83][88][136][187][206][213][251][264]

//(gdb) p showFirst(first,16)
//$64 = 0x7374e0 "\n list 1044 -->,[45],[56],[83],[89],[100],[129],[133],[142],[167],[191],[206],[240],[305],[314],[354]"
//(gdb) p showFirst(second,15)
//$65 = 0x7374e0 "\n list 1050 -->,[58],[59],[83],[88],[136],[187],[206],[213],[251],[264],[289],[303],[304],[307],[308]"


int RepairPost::svs2Exp (uint id1, uint id2, uint *noccs, uint **occs){
	
	if (shouldUseBitmap(id1) && shouldUseBitmap(id2)) {
		uint nids = 2;
		uint ids[2];
		ids[0]=id1; ids[1]=id2;
		int err =  mergeNBitmap (&ids[0], nids, noccs, occs);
		return err;
	}
			
	register uint n;
	register uint first, second;  //small and longest posting.


	if (lenPost[id1] <= lenPost[id2]) 
		{ first =id1; second=id2;}
	else  
		{ first =id2; second=id1;}

	uint * result = adj_il(first,&n);
	
	//n= result[0];     //number of elements in the first list
	matches =0;
/*
	uint *result2, n2;
	result2 = adj_il(second,&n2);
	
	{int x;
		printf("\nfst: <%u>:: ",first);
		for (x=0;x<10;x++) printf("[%u]",result[x]);
		printf("\nf2nd: <%u>:: ",second);
		for (x=0;x<10;x++) printf("[%u]",result2[x]);
		printf("\n");
		printf("%s",showFirst(second,10));
		printf("\n");
	}
	free(result2);
//printf("%s",showFirst(second,6000));
fflush(stdout); fflush(stderr);
*/
	if (shouldUseBitmap(second)){	
		int found; register uint j;
		
		for (j=0; j<n;j++) {
			found = fsearchBitmap(second, result[j]+GAP); //call to fsearch;
			//fprintf(stderr,"\n  Search for value %d -> found = %d",result[j], found);
			if (found) {
				result[matches++] = result[j]; //-GAP;;
			}
		}
		*occs  = result;
		*noccs = matches;
		return 0;
	}
	else { 
		int found; register uint j;
		for (j=0; j<n;j++) {
			found = fsearchGal(second, result[j]+GAP); //call to fsearch;
			//fprintf(stderr,"\n  Search for value %d -> found = %d",result[j], found);
			if (found) {
				result[matches++] = result[j]; //-GAP;;
			}
		}
		*occs  = result;
		*noccs = matches;
		return 0;
	}
		
	//return 0;
}




uint RepairPost::outdegree(int v) {
	info = adj(v);
	uint value = info[0];
	free(info);
	//return info[0];
	return value;
}




/******************************************************************************/
/*
int RepairPost::intNSkipping(uint *ids, uint nids, uint *noccs, uint **occs){
	
	uint id1; uint id2; //to delete, fari 16enero.
	id1= ids[0];
	id2=ids[1];

	
	if (shouldUseBitmap(id1) && shouldUseBitmap(id2)) {
		uint nids = 2;
		uint ids[2];
		ids[0]=id1; ids[1]=id2;
		int err =  mergeNBitmap (&ids[0], nids, noccs, occs);
		return err;
	}
	
		
	register uint n;
	register uint first, second;  //small and longest posting.


	if (lenPost[id1] <= lenPost[id2]) 
		{ first =id1; second=id2;}
	else  
		{ first =id2; second=id1;}

	result = adj_il(first,&n);
	
	//n= result[0];     //number of elements in the first list
	matches =0;

	if (shouldUseBitmap(second)){	
		int found;
		register uint j;
		
		for (j=0; j<n;j++) {
			found = fsearchBitmap(second, result[j]+GAP); //call to fsearch;
			//fprintf(stderr,"\n  Search for value %d -> found = %d",result[j], found);
			if (found) {
				result[matches++] = result[j];;//-GAP;;
			}
		}
		*occs  = result;
		*noccs = matches;
		return 0;
	}
	


	currVal =0;	  //curValue expanded in the 2nd list currently.
	fi=0;   //ini non-processed part of the 1st list
	fl=0;   //end non-processed part of the 1st list

	{
		uint v = second -1;
		uint ii = GetField(ptrs, plen, v); //symbols_new[rank_aux]);
		uint j  = GetField(ptrs, plen, v+1); //symbols_new[rank_aux]);
    	uint k, symb2nd, val2nd;

       	for (k=ii; k<j; k++){ //for all elements in the second list...
    		if (fi>n) break;						
    		symb2nd = GetField(symbols,nbits,k);  // the k-th element of the 2nd list
    
    		if (symb2nd <= max_value) { // a terminal value --> no expansion is needed.	
    			
				currVal += symb2nd;  //the actual value	
    			
			  	while ( (fi <= n) && ((result[fi]+GAP) < currVal) ) fi++;
				fl= fi;	
				if (fi>n) break;
			
				if ((result[fi]+GAP) == currVal) {
    				result[matches++] = currVal-GAP;
    				fi++; fl=fi;
    			}
    		}
    		else { // it is a phrase
    			
    			 val2nd = currVal + GetField(csymb_n, bits_sn, (symb2nd-max_value-1)); //uses skipping-info.    			 
	    		//val2nd is the value after k-th phrase is processed (from the posting of the 2nd list).
	    		//currValue is the starting value before that k-th phrase.
	    		    			
    			if (val2nd > (result[fi]+GAP)) {  //processing is needed    		
					while ( (fl <= n) && ((result[fl]+GAP) <= val2nd) ) fl++;	
					fl--;    					
					
					expandAndSearchSkipping (symb2nd); //currValue, fi, fl, result, 
					fi=fl+1; 
					fl=fi;
				}
				else if (val2nd == (result[fi]+GAP)) {
					result[matches++] = val2nd-GAP;				
    				//fi++; fl=fi;	
				}
									
				currVal = val2nd;
			}    			    			
    	}    		    		
	}	
	
	//if (matches == 0) {free(firstlist); firstlist = NULL;}	
	*occs  = result;
	*noccs = matches;					
		
	return 0;	
}

*/














int RepairPost::intNSkipping(uint *ids, uint nids, uint *noccs, uint **occs){

//	{uint i;
//	fprintf(stderr,"\n Call to intNskipping:: ");
//	for (i=0;i<nids;i++) fprintf(stderr,"[%d]",ids[i]);
//	}

	//sorting ids ==> lists are sorted by frequency !!
	register uint i, numbytecodes, numbitmaps;
	tsort_Rep *idss = (tsort_Rep *) malloc (sizeof(tsort_Rep) * nids);
	uint *idsBitmaps = (uint *) malloc (sizeof(uint) * nids);

	numbytecodes=0; numbitmaps=0;
	for (i=0;i<nids;i++) {
		if (!shouldUseBitmap(ids[i])){
			idss[numbytecodes].value=ids[i]; 
			idss[numbytecodes].key = lenPost[ids[i]];
			numbytecodes++;
		}
		else {
			//numbitmaps++;
			idsBitmaps[numbitmaps++]=ids[i]; 
			//idss[nids-numbitmaps].key = il->lenList[ids[i]];	//not needed		
		}
	}	

	if (numbytecodes == 0) { //intersecting only bitmaps --> mergeNbitmap
		int err =  mergeNBitmap ( idsBitmaps, numbitmaps,  noccs, occs);		
		free(idss);
		free(idsBitmaps);
		return err;
	}
	else {  //bytecodes (& could also be bitmaps) --> extract 1st + svsN.

		register uint j, n;

		if (numbytecodes >1)	//sorting the bytecodes by lenPost[]
			simplesortAsc_Rep(idss,numbytecodes); 	

		result = adj_il(idss[0].value,noccs);
		n=*noccs;
		matches =n; //number of elements in the first list

		for (i=1; i<numbytecodes; i++){ // the "nids-1" remaining posting-lists.
			matches =0;	
			currVal =0;	  //curValue expanded in the 2nd list currently.
			fi=0;   //ini non-processed part of the 1st list
			fl=0;   //end non-processed part of the 1st list

			{
				uint v = idss[i].value -1;
				uint ii = GetField(ptrs, plen, v); //symbols_new[rank_aux]);
				uint j  = GetField(ptrs, plen, v+1); //symbols_new[rank_aux]);
				uint k, symb2nd, val2nd;			

				for (k=ii; k<j; k++){ //for all elements in the second list...
					if (fi>=n) break;						
					symb2nd = GetField(symbols,nbits,k);  // the k-th element of the 2nd list
			
					if (symb2nd <= max_value) { // a terminal value --> no expansion is needed.	
						
						currVal += symb2nd;  //the actual value	
						
						while ( (fi < n) && ((result[fi]+GAP) < currVal) ) fi++;
						fl= fi;	
						if (fi>=n) break;
					
						if ((result[fi]+GAP) == currVal) {
							result[matches++] = currVal-GAP;
							fi++; fl=fi;
						}
					}
					else { // it is a phrase
						
						 val2nd = currVal + GetField(csymb_n, bits_sn, (symb2nd-max_value-1)); //uses skipping-info.    			 
						//val2nd is the value after k-th phrase is processed (from the posting of the 2nd list).
						//currValue is the starting value before that k-th phrase.
										
						if (val2nd > (result[fi]+GAP)) {  //processing is needed    		
							while ( (fl < n) && ((result[fl]+GAP) <= val2nd) ) fl++;	
							fl--;    					
							
							expandAndSearchSkipping (symb2nd); //currValue, fi, fl, result, 
							fi=fl+1; 
							fl=fi;
						}
						else if (val2nd == (result[fi]+GAP)) {
							result[matches++] = val2nd-GAP;				
							//fi++; fl=fi;	
						}
											
						currVal = val2nd;
					}    			    			
				}    		    		
			} //result[] , matches was set :: results of intersection of result[] and list for idss[i].
									
			if (!matches) break;  //search ends.
			n=matches;
		}		
				
		if (matches) { //svs on the bitmaps (if there are bitmaps).
			for (i=0; i<numbitmaps; i++){ // the "numbitmaps" remaining posting-lists.
				int found;
				register uint j;
				matches =0;	
						
				for (j=0; j<n;j++) {
					found = fsearchBitmap(idsBitmaps[i], result[j]+GAP); //call to fsearch;
					//fprintf(stderr,"\n  Search for value %d -> result = %d",firstlist[j], result);
					if (found) {
						result[matches++] = result[j];;//-GAP;;
					}
				}
				if (!matches) break;  //search ends.
				n=matches;
			}			
		}		
			
		*occs  = result;
		*noccs = matches;
	}
		
	free(idss);
	free(idsBitmaps);				
		
	return 0;	
}



int RepairPost::intNSkippingSeq(uint *ids, uint nids, uint *noccs, uint **occcs){

//	{uint i;
//	fprintf(stderr,"\n Call to intNskipping:: ");
//	for (i=0;i<nids;i++) fprintf(stderr,"[%d]",ids[i]);
//	}

	//sorting ids ==> lists are sorted by frequency !!
	register uint i, numbytecodes, numbitmaps;
	tsort_Rep *idss = (tsort_Rep *) malloc (sizeof(tsort_Rep) * nids);
	uint *idsBitmaps = (uint *) malloc (sizeof(uint) * nids);

	numbytecodes=0; numbitmaps=0;
	for (i=0;i<nids;i++) {
		if (!shouldUseBitmap(ids[i])){
			idss[numbytecodes].value=ids[i]; 
			idss[numbytecodes].key = lenPost[ids[i]];
			numbytecodes++;
		}
		else {
			//numbitmaps++;
			idsBitmaps[numbitmaps++]=ids[i]; 
			//idss[nids-numbitmaps].key = il->lenList[ids[i]];	//not needed		
		}
	}	

	if (numbytecodes == 0) { //intersecting only bitmaps --> mergeNbitmap
		int err =  mergeNBitmap ( idsBitmaps, numbitmaps,  noccs, occcs);		
		free(idss);
		free(idsBitmaps);
		return err;
	}
	else {  //bytecodes (& could also be bitmaps) --> extract 1st + svsN.

		register uint j, n;

		if (numbytecodes >1)	//sorting the bytecodes by lenPost[]
			simplesortAsc_Rep(idss,numbytecodes); 	

		result = adj_il(idss[0].value,noccs);
		n=*noccs;
		matches =n; //number of elements in the first list

		for (i=1; i<numbytecodes; i++){ // the "nids-1" remaining posting-lists.
			matches =0;	
			currVal =0;	  //curValue expanded in the 2nd list currently.
			fi=0;   //ini non-processed part of the 1st list
			fl=0;   //end non-processed part of the 1st list

			{
				register uint len = lenList(idss[i].value );
				register uint sampleP;
				SAMPLEPERIOD(sampleP,sampleK,len);	
					
				uint v = idss[i].value -1;
				uint ii = GetField(ptrs, plen, v); //symbols_new[rank_aux]);
				uint j  = GetField(ptrs, plen, v+1); //symbols_new[rank_aux]);
				uint k, symb2nd, val2nd;			


				uint poss,val;
				poss = occs[v]; //poss in zoneoccs	
				
				for (k=ii; k<j; k++){ //for all elements in the second list...
				
					if (fi>=n) break;	

						/** Searching in the samples */	
						
						/*seq search*/
						uint nextSample = (k-ii)/sampleP;
						val = GetField(zoneOccs, bits_zoneOccs, poss + nextSample);  
						
//						uint prevkk= k;
						while (val < (result[fi]+GAP)) {
							currVal = val; k = ii+(++nextSample)*sampleP; //incremento k sólo si val < result[fi]
							if (k >= j) break;
							val = GetField(zoneOccs, bits_zoneOccs, poss + nextSample);	
						}
						
//						cerr << "[s" << k-prevkk << "]";	
						if (k >= j) break;

						/** after searching in the samples **/   				
										
					symb2nd = GetField(symbols,nbits,k);  // the k-th element of the 2nd list
			
					if (symb2nd <= max_value) { // a terminal value --> no expansion is needed.	
						
						currVal += symb2nd;  //the actual value	
						
						while ( (fi < n) && ((result[fi]+GAP) < currVal) ) fi++;
						fl= fi;	
						if (fi>=n) break;
					
						if ((result[fi]+GAP) == currVal) {
							result[matches++] = currVal-GAP;
							fi++; fl=fi;
						}
					}
					else { // it is a phrase
						
						 val2nd = currVal + GetField(csymb_n, bits_sn, (symb2nd-max_value-1)); //uses skipping-info.    			 
						//val2nd is the value after k-th phrase is processed (from the posting of the 2nd list).
						//currValue is the starting value before that k-th phrase.
										
						if (val2nd > (result[fi]+GAP)) {  //processing is needed    		
							while ( (fl < n) && ((result[fl]+GAP) <= val2nd) ) fl++;	
							fl--;    					
							
							expandAndSearchSkipping (symb2nd); //currValue, fi, fl, result, 
							fi=fl+1; 
							fl=fi;
						}
						else if (val2nd == (result[fi]+GAP)) {
							result[matches++] = val2nd-GAP;				
							//fi++; fl=fi;	
						}
											
						currVal = val2nd;
					}    			    			
				}    		    		
			} //result[] , matches was set :: results of intersection of result[] and list for idss[i].
									
			if (!matches) break;  //search ends.
			n=matches;
		}		
				
		if (matches) { //svs on the bitmaps (if there are bitmaps).
			for (i=0; i<numbitmaps; i++){ // the "numbitmaps" remaining posting-lists.
				int found;
				register uint j;
				matches =0;	
						
				for (j=0; j<n;j++) {
					found = fsearchBitmap(idsBitmaps[i], result[j]+GAP); //call to fsearch;
					//fprintf(stderr,"\n  Search for value %d -> result = %d",firstlist[j], result);
					if (found) {
						result[matches++] = result[j];;//-GAP;;
					}
				}
				if (!matches) break;  //search ends.
				n=matches;
			}			
		}		
			
		*occcs  = result;
		*noccs = matches;
	}
		
	free(idss);
	free(idsBitmaps);				
		
	return 0;	
}



int RepairPost::intNSkippingExp(uint *ids, uint nids, uint *noccs, uint **occcs){

//	{uint i;
//	fprintf(stderr,"\n Call to intNskipping:: ");
//	for (i=0;i<nids;i++) fprintf(stderr,"[%d]",ids[i]);
//	}

	//sorting ids ==> lists are sorted by frequency !!
	register uint i, numbytecodes, numbitmaps;
	tsort_Rep *idss = (tsort_Rep *) malloc (sizeof(tsort_Rep) * nids);
	uint *idsBitmaps = (uint *) malloc (sizeof(uint) * nids);

	numbytecodes=0; numbitmaps=0;
	for (i=0;i<nids;i++) {
		if (!shouldUseBitmap(ids[i])){
			idss[numbytecodes].value=ids[i]; 
			idss[numbytecodes].key = lenPost[ids[i]];
			numbytecodes++;
		}
		else {
			//numbitmaps++;
			idsBitmaps[numbitmaps++]=ids[i]; 
			//idss[nids-numbitmaps].key = il->lenList[ids[i]];	//not needed		
		}
	}	

	if (numbytecodes == 0) { //intersecting only bitmaps --> mergeNbitmap
		int err =  mergeNBitmap ( idsBitmaps, numbitmaps,  noccs, occcs);		
		free(idss);
		free(idsBitmaps);
		return err;
	}
	else {  //bytecodes (& could also be bitmaps) --> extract 1st + svsN.

		register uint j, n;

		if (numbytecodes >1)	//sorting the bytecodes by lenPost[]
			simplesortAsc_Rep(idss,numbytecodes); 	

		result = adj_il(idss[0].value,noccs);
		n=*noccs;
		matches =n; //number of elements in the first list

		for (i=1; i<numbytecodes; i++){ // the "nids-1" remaining posting-lists.
			matches =0;	
			currVal =0;	  //curValue expanded in the 2nd list currently.
			fi=0;   //ini non-processed part of the 1st list
			fl=0;   //end non-processed part of the 1st list

			{
				register uint len = lenList(idss[i].value );
				register uint sampleP;
				SAMPLEPERIOD(sampleP,sampleK,len);
								
				uint v = idss[i].value -1;
				uint ii = GetField(ptrs, plen, v); //symbols_new[rank_aux]);
				uint j  = GetField(ptrs, plen, v+1); //symbols_new[rank_aux]);
				uint k, symb2nd, val2nd;			

				uint poss,val;
				poss = occs[v]; //poss in zoneoccs	


				for (k=ii; k<j; k++){ //for all elements in the second list...
				
					if (fi>=n) break;	

						/** Searching in the samples */	
						if (val < (result[fi]+GAP)) {			
							/*exp search*/
						
							uint nextSample = (k-ii)/sampleP;
							uint prevk=k, probe = 1;
							val = GetField(zoneOccs, bits_zoneOccs, poss + nextSample);  
							
							while (val < (result[fi]+GAP) ) {
								currVal = val; 
								prevk= ii+(nextSample+1)*sampleP;
								nextSample += probe;
								k = ii+(nextSample)*sampleP; //incremento k sólo si val < result[fi]
								if (k >= j) break;
								val = GetField(zoneOccs, bits_zoneOccs, poss + nextSample);	
								probe *=2;
							}	

							//cerr << "[s" << probe << "]";
							register uint l,r,m;
							val = currVal;
							l = (prevk-ii) / sampleP;
							if (k>=j) r = (len-1)/sampleP;
							else r = (k - ii)/sampleP;
							k=prevk;
								
							{
								register uint iniL = l;
								//l++;
								//r--;
								m=l;
								
								while (r > l+1) {
									m = (l + r) / 2;	
									val = GetField(zoneOccs, bits_zoneOccs, poss + m);
									if (val < (result[fi]+GAP) ) l = m;
									else r = m;					
								}
								
								if (l != iniL) {				
									if (val < (result[fi]+GAP)) {
											k= ii + (m+1)*sampleP;		
											currVal =  GetField(zoneOccs, bits_zoneOccs, poss + m);					
									}
									else {
										k= ii + (l+1)*sampleP;		
										currVal =  GetField(zoneOccs, bits_zoneOccs, poss + l);		
									}					 				
								}
							}	

							/**pequeña optimización ???  evita resto bucle si coinciden **/
							/**fari 2008.12.26 **/
							if (val == (result[fi]+GAP)) {
								k++;
								currVal = val;
								result[matches++] = currVal;
								fi++; fl=fi;    
								continue;					
							}									
						}
						
						if (k >= j) break;

						/** after searching in the samples **/  					
										
					symb2nd = GetField(symbols,nbits,k);  // the k-th element of the 2nd list
			
					if (symb2nd <= max_value) { // a terminal value --> no expansion is needed.	
						
						currVal += symb2nd;  //the actual value	
						
						while ( (fi < n) && ((result[fi]+GAP) < currVal) ) fi++;
						fl= fi;	
						if (fi>=n) break;
					
						if ((result[fi]+GAP) == currVal) {
							result[matches++] = currVal-GAP;
							fi++; fl=fi;
						}
					}
					else { // it is a phrase
						
						 val2nd = currVal + GetField(csymb_n, bits_sn, (symb2nd-max_value-1)); //uses skipping-info.    			 
						//val2nd is the value after k-th phrase is processed (from the posting of the 2nd list).
						//currValue is the starting value before that k-th phrase.
										
						if (val2nd > (result[fi]+GAP)) {  //processing is needed    		
							while ( (fl < n) && ((result[fl]+GAP) <= val2nd) ) fl++;	
							fl--;    					
							
							expandAndSearchSkipping (symb2nd); //currValue, fi, fl, result, 
							fi=fl+1; 
							fl=fi;
						}
						else if (val2nd == (result[fi]+GAP)) {
							result[matches++] = val2nd-GAP;				
							//fi++; fl=fi;	
						}
											
						currVal = val2nd;
					}    			    			
				}    		    		
			} //result[] , matches was set :: results of intersection of result[] and list for idss[i].
									
			if (!matches) break;  //search ends.
			n=matches;
		}		
				
		if (matches) { //svs on the bitmaps (if there are bitmaps).
			for (i=0; i<numbitmaps; i++){ // the "numbitmaps" remaining posting-lists.
				int found;
				register uint j;
				matches =0;	
						
				for (j=0; j<n;j++) {
					found = fsearchBitmap(idsBitmaps[i], result[j]+GAP); //call to fsearch;
					//fprintf(stderr,"\n  Search for value %d -> result = %d",firstlist[j], result);
					if (found) {
						result[matches++] = result[j];;//-GAP;;
					}
				}
				if (!matches) break;  //search ends.
				n=matches;
			}			
		}		
			
		*occcs  = result;
		*noccs = matches;
	}
		
	free(idss);
	free(idsBitmaps);				
		
	return 0;	
}

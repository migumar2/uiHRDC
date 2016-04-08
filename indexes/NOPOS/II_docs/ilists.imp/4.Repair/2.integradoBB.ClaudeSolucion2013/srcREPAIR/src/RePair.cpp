
#include "RePair.h"

RePair::RePair(uint * buff, uint datalen, uint _bufflen, char * _basename, uint _K) {
	data = buff;
	m = datalen;
	n = datalen;
	heap = NULL;
	bufflen = _bufflen;
	int * data_tmp = (int*)buff;
	mini = data_tmp[0];                  //** en ejemplo cnr data_tmp[0] = nodes
	long maxi = data_tmp[0];
	for(uint i=1;i<m;i++) {
		if(mini>data_tmp[i]) mini=data_tmp[i];  //** minimo valor en el buff.
		if(maxi<data_tmp[i]) maxi=data_tmp[i];  //** m�ximo valor en el buff
	}
	mini--;
	for(uint i=0;i<datalen;i++) 
		data[i]=data[i]-mini;   //** valores a assignar son trasladados de n, n' --> n-mini, n' -mini (alguno ser� zero)
	max_assigned = maxi-mini;  
	max_value = max_assigned;   //**
	
	//** printf("\n\n mini %d    maxi %d    max_assigned %d   max_value %d",mini,maxi,max_assigned,max_value);
	//**              mini 0    maxi 650371    max_assigned 650371   max_value 650371
	
	K=_K; 
	CORTE_REPAIR = 0.0;
	lastpos=0;
	basename = string(_basename);
	fpDict = fopen64((basename+".dict").data(),"w");
}

RePair::~RePair() {
	//delete [] data; new[] couldnt get so much memory
	//free(data);
	delete [] data;
	fclose(fpDict);
}

//** mete en "heap" los k pares m�s frecuentes   { <pos_1, freq_1>  <pos_2, freq_2>...<pos_k, freq_k> }
void RePair::fillHeap() {
  if(heap!=NULL) {
    delete heap;
    heap = NULL;
  }
  heap = new Heap(K);           //** heap para almacenar hasta K elementos
  assert(heap!=NULL);

  uint hashsize = (bufflen-m-1)/2;
  //cout << "hashsize=" << hashsize << " m=" << m << " bufflen=" << bufflen << endl;
//	cout << "len=" << (data+bufflen) - (data+m+1) << endl;
  hpos = new HashTablePos((uint*)data,m,(int*)(data+m+1),hashsize);   //** tabla hash es inicializada <posIni,contador>
	assert(hpos!=NULL);

	lastpos %= m;
	uint blastpos = lastpos; 
	bool save=true; 
	uint pos=lastpos;

	do {  // ** recorremos todo el texto de forma circular... desde el valor de lastpos...
		if(!hpos->increment(pos)) {   //** Inserta un par en la tabla hash, y si ya estaba incrementa su contador de ocurrencias.
		  if(save) {
  			save = false;
	  		lastpos = pos;
	  	}
		}
		pos = (pos+1)%m;
	} while(pos!=blastpos);

	int * hasharray = (int*)(data+m+1);
	for(uint i=0;i<hashsize;i++)         //** INSERCI�N EN EL HEAP ==> ya calcula los K m�s frecuentes.
		if(hasharray[2*i]>=0)   //** slot ocupado (pues es != -1)  --> insertar en la heap (se queda con los k m�s frecuentes)
		  heap->insert(new pair<uint,uint>((uint)hasharray[2*i],(uint)hasharray[2*i+1]));
	delete hpos;
}

uint RePair::replacePairs() {
  fillHeap();   //** Heap contendr� los K pares <posInic, contador> m�s frecuentes. Ojo, alguno podr�a quedar con "contador =1" !!
	uint c = 0;
  uint countR = 0;
	uint hashsize = (bufflen-m-1)/2;

	for(uint i=1;i<heap->inserted;i++)   //** cuenta el n�mero de pares con freq > 1
		if(heap->elements[i]!=NULL && heap->elements[i]->second>1) 
			c++;

	if(c==0) {  //** no hay pares con freq > 1   ==> final ==> return 0
		delete heap;
		heap = NULL;
		return 0;
	}


	//** ***** PASO 3: Reemplazo simult�neo en una �nica pasada sobre data. *****
	
  uint _c = c;
	pair<uint,uint> ** ret = new pair<uint,uint>*[c];  //** defino array de c "pares" <pos, count>
	hpairs = new HashTablePairs((int*)(data+m+1),2*hashsize/3);
	for(uint i=1;i<heap->inserted;i++)
		if(heap->elements[i]!=NULL && heap->elements[i]->second>1) {
			uint pos = heap->elements[i]->first;             //** mete los pares en la hash table de Pares (orden decreciente de freq)
			ret[--c] = new pair<uint,uint>(pos,NOT_SEEN);  //** lo pone como "not_seen"
			hpairs->insert(data[pos],data[pos+1],c);  // c es el n�mero de par!!
		}



	for(uint i=0;i<m;i++) {      //**reemplazo de los k pares que se han formado.                
		c = hpairs->get(data[i],data[i+1]);
		if(c!=0) {
			c--;
			switch(ret[c]->second) {
				case NOT_SEEN: ret[c]->first = i;
				               ret[c]->second = FIRST;
											 break;
				case FIRST: 	if(data[ret[c]->first]==data[i] && data[ret[c]->first+1]==data[i+1]) {  //**si a�n es posible a�adirlo
											if(i==ret[c]->first+1) break;
											addRule(++max_assigned,data[i],data[i+1]);
											ret[c]->second = max_assigned;
											data[ret[c]->first] = ret[c]->second;
											data[ret[c]->first+1] = 0;
											countR++;
										}
										else {
											ret[c]->first=i;
											break;
										}
				default:			data[i] = ret[c]->second;
											data[i+1] = 0;
			}
		}
	}


	//** ******* PASO 4. compactar T, y alargar HashTable sobre data  ******
	rearrange();

	delete heap; 
	heap=NULL;
	for(uint i=0;i<_c;i++)
		delete ret[i];
	delete [] ret;
	delete hpairs;

	return countR;
}


//compacta vector (quitando los segundos elementos "inv�lidos == 0"
void RePair::rearrange() {
	uint rptr=0;
	for(uint i=0;i<m;i++) {
		if(data[i] != 0) {
			data[rptr] = data[i];
			rptr++;
		}
	}
	data[rptr] = 0;
	m = rptr;
}



void RePair::addRule(uint nvalue, uint p1, uint p2) {
//static int cuenta=0;
  fwrite(&nvalue,sizeof(uint),1,fpDict);
  fwrite(&p1,sizeof(uint),1,fpDict);  
  fwrite(&p2,sizeof(uint),1,fpDict);
  
//  if (cuenta < 10000) {
//  	cout << "\t Adding rule:  " << nvalue-502260 << " <-- " << "(" << p1-502260 <<","<<p2-502260 << ")\n";	
//  	cuenta++;	
//  }
}

void RePair::compress(uint nodes, double corte) {
	CORTE_REPAIR = corte;
	compress(nodes);
}

void RePair::compress(uint nodes) {
  uint i = 1; uint lasti=1;
  
  double prevRatio=100.0;
  
  while(true) {

    cout << "m vale = " << m << endl;
    while(replacePairs()>0) {  //** mientras se hayan hecho reemplazos...
      //cout.setw(2);
      //cout << "Repair.compress: It: " << i++ << " compressed: " << 100.*m/n << "%" << " m=" << m << endl;
      cout << "Repair.compress: It: " << i++ << " compressed: " << 100.*m/n << "%" << " m=" << m <<  ", diff prev iter =" << (prevRatio - (100.0*m/n)) << endl;
   	  //cout << "Repair.compress: It: " << i++ << " compressed: " << 100.*m/n << "%" << " m=" << m << endl;
	  
	  //if ((100.*m/n)<43.0) break;	  
//	  if ((100.*m/n)<60.0) break;	  
	  //if ((100.*m/n)<46.0) break;
	  //if ((100.*m/n)<44.0) break;
	  //if ((100.*m/n)<45.0) break;
	  //if ((100.*m/n)<47.0) break;
	//  if ((100.*m/n)< CORTE_REPAIR) break;	    //con break_bitmap=8
	  
	//if  (  ((prevRatio - (100.*m/n)) < 0.00005 )  ||  ((100.*m/n)< CORTE_REPAIR) ) break;
	//if  (  ((prevRatio - (100.*m/n)) < 0.00005 )  ||  (((prevRatio - (100.*m/n)) < CORTE_REPAIR) ) ) break;
	  if  (((prevRatio - (100.*m/n)) < CORTE_REPAIR) )  break;
	  prevRatio = (100.*m/n);

    }
    //  cout << "Repair.compress: It: " << i++ << " compressed: " << 100.*m/n << "%" << " m=" << m <<  ", diff prev iter =" << (prevRatio - (100.0*m/n)) << endl;
    
    cout << "lasti vale " << lasti << " i vale " << i ;  //** fari **/
    if(lasti<=i) break;

    	
    	
    uint sorted=0;                           //** Normalmente no entra aqu� !!! **/
    cout << "Reordering symbols: ";
    cout.flush();
    for(uint j=0;j<m;j++) {
      if(data[j]<=nodes) {   
        for(uint k=j+1;k<=m;k++)
          if(data[k]<=nodes) {
            sort(data+j+1,data+k);
            sorted++;
            break;
          }
      }
    } 
    cout << sorted << endl;
    lasti = i;    
  }
}

bool RePair::save(uint nodes) {
  return saveRPFile(nodes); 
}

bool RePair::saveRPFile(uint nodes) {
  nodes++;
  const char * fname = (basename+".rp").data();
	FILE * fp = fopen64(fname,"w");
	assert(fp!=NULL);
	fwrite(&mini,sizeof(uint),1,fp);
	fwrite(&max_value,sizeof(uint),1,fp);
  fwrite(&max_assigned,sizeof(uint),1,fp);     //** max_assigned = max_value + numNuevasReglasCreadas.
	fwrite(&m,sizeof(uint),1,fp);
	fwrite(&n,sizeof(uint),1,fp);
  fwrite(&nodes,sizeof(uint),1,fp);
  uint writ = 0;
	for(uint i=0;i<m;i++) {
	    if(data[i]>nodes) {                           //** s�lo escribe en "rp" los "edges".
	      data[i]-=(nodes);                           //** resta "nodes" a todos los ejes  **
	  		fwrite(&data[i],sizeof(uint),1,fp);
	      writ++;                                     //** cuenta el n�mero de elementos escritos.
	    }
	}
  fwrite(&data[m],sizeof(uint),1,fp);
  fseeko64(fp,3*sizeof(uint),SEEK_SET);          //** reemplaza el valor "max_assigned".
	fwrite(&writ,sizeof(uint),1,fp);
  n -= (m-writ);                                  //** tama�o real del texto comprimido quitando "nodos"
	fwrite(&n,sizeof(uint),1,fp);                //** reemplaza el valor "m".
	fclose(fp);
  return true;
}


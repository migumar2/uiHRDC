
int gonzalosRepair(int * text, uint nodes, uint edges, char *fname, char **argv, char * path2irepair, double CORTE_REPAIR)
{  /// USES GONZALO'S irepair to create the C-sequence and the dictionary of rules.

		//	RePair::RePair(ulong * buff, ulong datalen, ulong _bufflen, char * _basename, ulong _K) {
		int *data =text;
		int mini;
		int max_assigned, max_value;
		int  maxi;
		uint m,n;		
		{
			
				int datalen = (edges+nodes);
				m = datalen;
				n = datalen;
				int * data_tmp = (int*)data;
				mini = data_tmp[0];                  //** en ejemplo cnr data_tmp[0] = nodes
				maxi = data_tmp[0];
				for(uint i=1;i<m;i++) {
					if(mini>data_tmp[i]) mini=data_tmp[i];  //** minimo valor en el buff.
					if(maxi<data_tmp[i]) maxi=data_tmp[i];  //** maximo valor en el buff
				}
				mini--;
				for(int i=0;i<datalen;i++) 
					data[i]=data[i]-mini;   //** valores a assignar son trasladados de n, n' --> n-mini, n' -mini (alguno sera zero)
				max_assigned = maxi-mini;  
				max_value = max_assigned;   //**
				
				printf("\n\n mini %d    maxi %d    max_assigned %d   max_value %d",mini,maxi,max_assigned,max_value);
				//**              mini 0    maxi 650371    max_assigned 650371   max_value 650371
				
		}
		//	}
	  
	  
		sprintf(fname,"%s.gonzalo.data",argv[1]);  
		FILE * f_out = fopen64(fname,"w");
		fwrite(text,sizeof(int),(ulong)(edges+nodes),f_out);   //o nodes+ejes+1 ... ver después!	
		fclose(f_out);


		

		char cmd[2000];
		sprintf(cmd,"%s %s %3.15f", path2irepair,fname, CORTE_REPAIR);
		
		printf("\n\n REPAIR: call to %s\n\n ", cmd);				 
		fflush(stdout);fflush(stderr);
		int x = system(cmd);
		fflush(stdout);fflush(stderr);
		  
		char fnameR [2000];  ssize_t lenR;  int *R;
		char fnameC [2000];  ssize_t lenC;  int *C;
		
		sprintf(fnameR,"%s.gonzalo.data.R",argv[1]);
		sprintf(fnameC,"%s.gonzalo.data.C",argv[1]);
		
		struct stat s;

		// cargar C	
		if (stat (fnameC,&s) != 0) { 
			fprintf (stderr,"Error: cannot stat file %s\n",fnameC);
			exit(1);
		}
		lenC = s.st_size/sizeof(int);
		FILE * f_inC = fopen(fnameC,"r");
		if (f_inC == NULL){ 
			fprintf (stderr,"Error: cannot open file %s for reading\n",fnameC);
			exit(1);
		}	
		C = (int*)malloc((lenC+1)*sizeof(int));
		C[lenC]=0;  //POR COMPATIBILIDAD CON EL REPAIR.SAVE DEl repair de FRANCISCO
		ssize_t leido = fread(C,sizeof(int),lenC,f_inC);
		printf("\n leidos %lu enteros en fichero %s",(ulong)leido,fnameC);
		fclose(f_inC);
		  
		//cargar R
		if (stat (fnameR,&s) != 0) { 
			fprintf (stderr,"Error: cannot stat file %s\n",fnameR);
			exit(1);
		}
		lenR = s.st_size/sizeof(int);
		FILE * f_inR = fopen(fnameR,"r");
		if (f_inR == NULL){ 
			fprintf (stderr,"Error: cannot open file %s for reading\n",fnameR);
			exit(1);
		}	
		R = (int*)malloc(lenR*sizeof(int));
		leido = fread(R,sizeof(int),lenR,f_inR);
		printf("\n leidos %lu enteros en fichero %s",(ulong)leido,fnameR);
		fclose(f_inR);


		//procesar R  --> para generar fichero .rp compatible
		{	//R contiene:   [alpha=nodes][left1][right1][left2][right2],... 
			// Fari:
			// Posto que alpha = num_terminais = valor do primeiro non-terminal.
			//  ** a primeira regla sería: alpha <-- left1,right1
			//  ** a  segunda regla sería: alpha +1 <-- left2, right2
			// por iso no ficheiro de reglas só garda <alpha> e despois: left1, right1, left2, right2,... 
			// pois xa sabe que os valores alpha, alpha+1,... son contiguos ;)
			
			char filename[2000]; sprintf(filename,"%s.dict",argv[1]);
			FILE *fpDict = fopen(filename,"w");       
			lenR--;
			lenR /=2;
			int alpha = R[0];
			int nvalue = alpha;
			for (int i=0;i<lenR;i++) {	
				int p1= R[1+ (i*2)   ];
				int p2= R[1+ (i*2 +1)];		
				fwrite(&nvalue,sizeof(uint),1,fpDict);
				fwrite(&p1,sizeof(uint),1,fpDict);  	
				fwrite(&p2,sizeof(uint),1,fpDict);
				nvalue ++;
			}		
			fclose(fpDict);
		 
		} 
		free(R);
		//lenR contiene ahora el número de reglas creadas durante "repair" (lo necesitamos + abajo para calcular max_assigned)


		//procesar C  --> para generar fichero .dict compatible

		{	
			m = lenC;
		
			int *data = C;
			//bool RePair::saveRPFile(ulong nodes) {
				{
				  max_assigned = lenR + max_value; //
				  nodes++;
				  char filename[2000]; sprintf(filename,"%s.rp",argv[1]);
					FILE * fp = fopen(filename,"w");
					assert(fp!=NULL);
					fwrite(&mini,sizeof(uint),1,fp);
					fwrite(&max_value,sizeof(uint),1,fp);
				  fwrite(&max_assigned,sizeof(uint),1,fp);     //** max_assigned = max_value + numNuevasReglasCreadas.
				  
				  
					fwrite(&m,sizeof(uint),1,fp);
					fwrite(&n,sizeof(uint),1,fp);         //longitud de la secuencia de repair original (nodes + ejes), ver arriba.
				  fwrite(&nodes,sizeof(uint),1,fp);
				  
				  ulong writ = 0;
					for(uint i=0;i<m;i++) {
						if(data[i]>nodes) {                           //** solo escribe en "rp" los "edges". <---------- ya quita los "nodos" = los valores negativos que indicaban inicio de "lista".
						  data[i]-=(nodes);                           //** resta "nodes" a todos los ejes  *************************************
							fwrite(&data[i],sizeof(uint),1,fp);
						  writ++;                                     //** cuenta el numero de elementos escritos.
						}
					}
					
				  fwrite(&data[m],sizeof(uint),1,fp);  //* fari 2013. Realmente se escribe sólo por compatibilidad con el BUILD_INDEX normal */
														//* PUES EN el fichero "fnameC" vienen M ints, no M+1 !!!"
					//data[m]=0;   //POR COMPATIBILIDAD con BUILD_INDEX_NORMAL, QUE al final del fichero .rp metía un "0".
					//             //Y ojo... este "0" se lee también en el "compress_dictionary".
				  
				  fseeko64(fp,3*sizeof(uint),SEEK_SET);          //** reemplaza el valor "max_assigned".
					fwrite(&writ,sizeof(uint),1,fp);
				  n -= (m-writ);                                  //** taman real del texto comprimido quitando "nodos"
					fwrite(&n,sizeof(uint),1,fp);                //** reemplaza el valor "m".
					fclose(fp);
				}
			//}		
		 
		} 
		free(C);

	  return 0;
	}
	
  	/* end of call to Gonzalo's Repair  *********************************************/

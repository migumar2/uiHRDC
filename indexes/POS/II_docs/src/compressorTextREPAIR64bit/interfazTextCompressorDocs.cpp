#include "interfazTextCompressorDocs.h"




/***********************************************************************************/
/*** FUNCTIONS USED FOR PARSING PARAMETERS FROM COMMAND LINE ***********************/
/* Three function to variables to manage parameters */

static bool is_delimeter_ct(char *delimiters, char c) {
   int i=0,len_delimiters=strlen(delimiters);
   bool is=false;
   for (i=0;i<len_delimiters;i++)
     if (c == delimiters[i]) is=true;
   return is;
}

static void parse_parameters_ct(char *options, int *num_parameters, char ***parameters, char *delimiters) {
  int i=0,j=0,temp=0,num=0, len_options=strlen(options);
  char *options_temp;
  while  (i<len_options) {
    while ((i<len_options) && is_delimeter_ct(delimiters,options[i])) i++;
    temp=i;
    while ((i<len_options) && !is_delimeter_ct(delimiters,options[i])) i++;
    if (i!=temp) num++;
  }
  (*parameters) = (char **) malloc(num*sizeof(char * ));
  i=0;
  while  (i<len_options) {
    while ((i<len_options) && is_delimeter_ct(delimiters,options[i])) i++;
    temp=i;
    while ((i<len_options) && !is_delimeter_ct(delimiters,options[i])) i++;
    if (i!=temp) {
      (*parameters)[j]=(char *) malloc((i-temp+1)*sizeof(char));
      options_temp = options+temp;
      strncpy((*parameters)[j], options_temp, i-temp);
      ((*parameters)[j])[i-temp] = '\0';
      j++;
    }
  }
  *num_parameters = num;
}

static void free_parameters_ct(int num_parameters,char ***parameters) {
  int i=0;
  for (i=0; i<num_parameters;i++)
    free((*parameters)[i]);
  free((*parameters));
}
/***********************************************************************************/




int 
build_representation (unsigned char *text, unsigned int length,
	unsigned int *docBeginnings, unsigned int numdocs, 
	char *build_options, void **representation)
{
	
	
	uint sample_ct=16384;
	{
		/** processing the parameters */
		char delimiters[] = " =;";
		char filename[256] = "unnamed";
		int j,num_parameters;
		char ** parameters;

		
		if (build_options != NULL) {
		parse_parameters_ct(build_options,&num_parameters, &parameters, delimiters);
		for (j=0; j<num_parameters;j++) {
		  if ((strcmp(parameters[j], "sample_ct") == 0 ) && (j < num_parameters-1) ) {
			sample_ct=atoi(parameters[j+1]);	    
			j++;
		  }
		}
		free_parameters_ct(num_parameters, &parameters);
		}
		
		printf("\n parameters of compressed text representation \"sample_ct=%u\"  (default \"sample_ct=16384\")\n",sample_ct);	
	
	}
	
	
	
	
	
	
	RePairDocCompressor *compressor = new RePairDocCompressor();
	*representation = compressor;
	return compressor->build_representation(text, length, docBeginnings, numdocs, sample_ct);
}

int 
save_representation (void *representation, char *filename)
{
	RePairDocCompressor *compressor = (RePairDocCompressor*)representation;
	return compressor->save_representation(filename);
}

int 
load_representation (void **representation, char *filename)
{
	*representation = RePairDocCompressor::load_representation(filename);
	return 0;
}

int 
free_representation (void *representation)
{
	RePairDocCompressor *compressor = (RePairDocCompressor*)representation;
	delete compressor;
}

int 
size_representation (void *representation, uint *size)
{
	RePairDocCompressor *compressor = (RePairDocCompressor*)representation;
	compressor->size_representation(size);
}

int 
extract_doc_representation (void *representation, uint i, unsigned char **doc, unsigned int *len)
{
	RePairDocCompressor *compressor = (RePairDocCompressor*)representation;
	return compressor->extract_doc_representation(i, doc, len);
}


	/* Recovers the snippet [from,to] in the collection of documents */

int 
extract_from_to_char (void *representation, uint from, uint to, unsigned char **snippet, unsigned int *slen)
{
	RePairDocCompressor *compressor = (RePairDocCompressor*)representation;
	return compressor->extract_from_to_char(from, to, snippet, slen);
}


/* Sebas, Miguel, esta funcion no tenéis que implementarla! */
/* La usé sólo para debugging */
void 
printTextInOffset(void *representation, uint offset, uint len) {
	return ;
		
}

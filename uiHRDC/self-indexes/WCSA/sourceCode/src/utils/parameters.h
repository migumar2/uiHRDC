#include <string.h>
#include <stdlib.h>
/***********************************************************************************/
/*** FUNCTIONS USED FOR PARSING PARAMETERS FROM COMMAND LINE ***********************/

#ifndef PARAMETERS_INCLUDED
#define PARAMETERS_INCLUDED
 bool is_delimeter(char *delimiters, char c) ;
 void parse_parameters(char *options, int *num_parameters, char ***parameters, char *delimiters);
 void free_parameters(int num_parameters,char ***parameters);
#endif

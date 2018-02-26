#include <ctype.h>

#ifndef ValidCh

 #define ValidCh(ch)  (isalnum(ch)) /* Teste de validacao	*/   
 #define InvalidCh(ch) (!ValidCh(ch))

#endif


extern unsigned char  _Valid[256];
extern unsigned char  _Invalid[256];
extern unsigned char  _toLow[256];

void StartValid();
void StartToLow();

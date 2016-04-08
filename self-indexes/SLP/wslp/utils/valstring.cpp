#include "valstring.h"
#include <stdio.h>

unsigned char  _Valid[256];
unsigned char  _Invalid[256];

unsigned char _toLow[256];


#ifndef ValidCh

 #define ValidCh(ch)  (isalnum(ch)) 
 #define InvalidCh(ch) (!ValidCh(ch))

#endif

void 
ValString::StartValid() {

  unsigned x;

  for(x=0;x<128;x++) {
    if(ValidCh(x)) {
      _Valid[x]=1;
      _Invalid[x]=0;
    }
    else {
      _Valid[x]=0;
      _Invalid[x]=1;
    }
  }
  for(x=128;x<256;x++) {
    _Valid[x]=0;
    _Invalid[x]=1;
  }
  
  _Valid[0]=_Invalid[0]=0;

  // Caracteres especiales (acentuados, dieresis...)
  // Caracter 'ñ'
  _Valid[241]=1;
  _Invalid[241]=0;
  // Caracter 'Ñ'
  _Valid[209]=1;
  _Invalid[209]=0;
  // Caracter 'á'
  _Valid[225]=1;
  _Invalid[225]=0;
  // Caracter 'é'
  _Valid[233]=1;
  _Invalid[233]=0;
  // Caracter 'í'
  _Valid[237]=1;
  _Invalid[237]=0;
  // Caracter 'ó'
  _Valid[243]=1;
  _Invalid[243]=0;
  // Caracter 'ú'
  _Valid[250]=1;
  _Invalid[250]=0;
  // Caracter 'Á'
  _Valid[193]=1;
  _Invalid[193]=0;
  // Caracter 'É'
  _Valid[201]=1;
  _Invalid[201]=0;
  // Caracter 'Í'
  _Valid[205]=1;
  _Invalid[205]=0;
  // Caracter 'Ó'
  _Valid[211]=1;
  _Invalid[211]=0;
  // Caracter 'Ú'
  _Valid[218]=1;
  _Invalid[218]=0;
  // Caracter 'ü'
  _Valid[252]=1;
  _Invalid[252]=0;
  // Caracter 'Ü'
  _Valid[220]=1;
  _Invalid[220]=0;      
}

void 
ValString::StartToLow() {
	int i;
 	unsigned char c;
 	for (i=0;i<256;i++) {
 		c=i;
 		if( (c >= 'A') && (c <= 'Z') ){
 			_toLow[i]= c+ 'a'-'A';
 		}
 		else if (!_Valid[c]) {
 			_toLow[i]=c; 	
 		}		
 		else {
			switch(c) {			
				case 192: case 193: case 194: case 195: case 196: case 197: case 224: case 225: 
				case 226: case 227: case 228: case 229: 
					c = 97; break;
			
				case 201: case 202: case 203: case 232: case 233: case 234: case 235: 
					c = 101; break;
				
				case 204: case 205: case 206: case 207: case 236: case 237: case 238: case 239: 
					c = 105; break;
				
				case 210: case 211: case 212: case 213: case 214: case 242: case 243: case 244: 
				          case 245: case 246: 
					c = 111; break;
				
				case 217: case 218: case 219: case 220: case 249: case 250: case 251: case 252: 
					c = 117; break;
				
				case 209: c = 241; break;
				
				case 138: case 154: c = 115; break;
				
				case 159: case 221: case 253: case 255: 
					c = 121; break;				
			}
			_toLow[i]=c;
		}
	}	
}

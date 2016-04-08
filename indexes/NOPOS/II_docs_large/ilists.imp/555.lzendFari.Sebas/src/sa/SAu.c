/* SAu.c
   Copyright (C) 2005, Rodrigo Gonzalez, all rights reserved.

   This file contains an implementation of Suffix Array in a uncompressed
   form.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/
/*
   This library uses a suffix array construction algorithm developed by
   Giovanni Manzini (Università del Piemonte Orientale) and
   Paolo Ferragina (Università di Pisa).
   You can find more info at:
   http://roquefort.di.unipi.it/~ferrax/SuffixArray/
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#ifndef uchar
#define uchar unsigned char
#endif
#ifndef ulong
#define ulong unsigned long
#endif


typedef struct {
  ulong *pos;
  uchar *text;
  ulong n;
  bool own;
} TSA_Un;

/*/////////////////////
//External procedures//
/////////////////////*/
extern void ds_ssort(uchar *x, ulong *p, long n);
extern int init_ds_ssort(int adist, int bs_ratio);

/*/////////////////////
//Internal procedures//
/////////////////////*/
static inline void SetPos(TSA_Un *index, ulong position, ulong x) {
  index->pos[position]=x;
}
static inline ulong GetPos(TSA_Un *index, ulong position) {
  return index->pos[position];
}
static inline int ComparePos(TSA_Un *index, ulong position, uchar *pattern, ulong m) {
  ulong n=index->n;
  uchar *text=index->text;
  ulong j=0;

  while (position+j < n && j < m && text[position+j]==pattern[j]) {
    j++;
  }
  if (j >= m)
     return 0;
  else if (position+j >= n)
     return +1;
  else if (text[position+j]>pattern[j])
     return -1;
  else
     return +1;
}
/* Three function to variables to manage parameters */
static bool is_delimeter(char *delimiters, char c) {
   int i=0,len_delimiters=strlen(delimiters);
   bool is=false;
   for (i=0;i<len_delimiters;i++)
     if (c == delimiters[i]) is=true;
   return is;
}

static void parse_parameters(char *options, int *num_parameters, char ***parameters, char *delimiters) {
  int i=0,j=0,temp=0,num=0, len_options=strlen(options);
  char *options_temp;
  while  (i<len_options) {
    while ((i<len_options) && is_delimeter(delimiters,options[i])) i++;
    temp=i;
    while ((i<len_options) && !is_delimeter(delimiters,options[i])) i++;
    if (i!=temp) num++;
  }
  (*parameters) = (char **) malloc(num*sizeof(char * ));
  i=0;
  while  (i<len_options) {
    while ((i<len_options) && is_delimeter(delimiters,options[i])) i++;
    temp=i;
    while ((i<len_options) && !is_delimeter(delimiters,options[i])) i++;
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

static void free_parameters(int num_parameters,char ***parameters) {
  int i=0;
  for (i=0; i<num_parameters;i++)
    free((*parameters)[i]);
  free((*parameters));
}


/*////////////////////
//Building the Index//
////////////////////*/
int build_index(uchar *text, ulong length, char *build_options, void **index){
/*if (text[length-1]!='\0') return 2;*/
  ulong i, *p;
  long overshoot;
  TSA_Un *_index= (TSA_Un *) malloc(sizeof(TSA_Un));
  uchar *x;
  char delimiters[] = " =;";
  int j,num_parameters;
  char ** parameters;
  int copy_text=false; /* don't copy text by default */
  int free_text=false; /* don't free text by default */
  if (!_index) return 1;
  if (build_options != NULL) {
    parse_parameters(build_options,&num_parameters, &parameters, delimiters);
    for (j=0; j<num_parameters;j++) {
      if (strcmp(parameters[j], "copy_text") == 0 )
        copy_text=true;
      else if (strcmp(parameters[j], "free_text") == 0 )
        free_text=true;
    }
    free_parameters(num_parameters, &parameters);
  }
  /* Consistence of parameters  */
  if ((!copy_text) && (free_text))
    return 5;
  /*                            */
  if ( !copy_text ) {
    _index->text = text;
     _index->own=false;
  } else {
    _index->text = (uchar *) malloc(sizeof(uchar)*length);
    if (!_index->text) return 1;
    for (i=0;i<length;i++) _index->text[i]=text[i];
    _index->own=true;
  }
  if ( free_text )
    free(text);
  
  _index->n=length;

  /* Make suffix array */
  overshoot = init_ds_ssort(500, 2000);
  p= (ulong *) malloc (sizeof(ulong)*(length));
  if (!p) return 1;
  x= (uchar *) malloc (sizeof(uchar)*(length+overshoot));
  if (!x) return 1;
  for (i=0;i<length;i++) x[i]=_index->text[i];
  ds_ssort( x, p, _index->n);
  free(x);

  _index->pos = p;
  (*index) = _index;
  return 0;
}
int save_index(void *index, char *filename) {
  TSA_Un *_index=(TSA_Un *) index;
  char fnamext[1024];
  FILE *f;
  sprintf (fnamext,"%s.sa",filename);
  f = fopen (fnamext,"w");
  if (f == NULL) return 20;
  if (fwrite (&_index->n,sizeof(ulong),1,f) != 1) return 21;
  if (fwrite (_index->pos,sizeof(ulong),_index->n,f) != _index->n) return 21;
  if (fclose(f) != 0) return 22;
  return 0;
}
int load_index(char *filename, void **index){
  char fnamext[1024];
  FILE *f;
  struct stat sdata;
  TSA_Un *_index;
  ulong file_size;
  /* Read index */
  sprintf (fnamext,"%s.sa",filename);
  f = fopen (fnamext,"r");
  if (f == NULL) return 23;
  _index= (TSA_Un *) malloc(sizeof(TSA_Un));
  if (!_index) return 1;
  if (fread (&_index->n,sizeof(ulong),1,f) != 1) return 25;
  _index->pos= (ulong *) malloc (sizeof(ulong)*_index->n);
  if (!_index->pos) return 1;
  if (fread (_index->pos,sizeof(ulong),_index->n,f) != _index->n) return 25;
  if (fclose(f) != 0) return 26;
  /*read text */
  _index->own = true;
  stat(filename,&sdata);
  file_size = sdata.st_size;

  f = fopen (filename,"r");
  if (f == NULL) return 24;
  _index->text= (uchar *) malloc (sizeof(uchar)*file_size);
  if (!_index->text) return 1;
  if (fread (_index->text,sizeof(uchar),file_size,f) != file_size) return 27;
  if (fclose(f) != 0) return 28;
  (*index) = _index;
  return 0;
}
int free_index(void *index){
  TSA_Un *_index=(TSA_Un *) index;
  free(_index->pos);
  if (_index->own) free(_index->text);
  free(_index);
  return 0;
}
int index_size(void *index, ulong *size) {
  (*size) = sizeof(TSA_Un)+(sizeof(uchar)+sizeof(ulong))*((TSA_Un *) index)->n;
  return 0;
}

int index_size_count(void *index, ulong *size) {
  (*size) = sizeof(TSA_Un)+(sizeof(uchar)+sizeof(ulong))*((TSA_Un *) index)->n;
  return 0;
}
int index_size_locate(void *index, ulong *size) {
  (*size) = sizeof(TSA_Un)+(sizeof(uchar)+sizeof(ulong))*((TSA_Un *) index)->n;
  return 0;
}

/*////////////////////
//Querying the Index//
////////////////////*/
int count(void *index, uchar *pattern, ulong length, ulong *numocc){
  TSA_Un *_index=(TSA_Un *) index;
  ulong L=0;
  ulong R=_index->n;
  ulong i=(L+R)/2;
  ulong Lraja=0;
  ulong Rraja=0;
  /* Search for the left boundary */
  if (ComparePos(_index,GetPos(_index,0),pattern,length) <= 0)
     Lraja=0;
  else {
     while (L<R-1) {
        if (ComparePos(_index,GetPos(_index,i),pattern,length) <= 0)
           R = i;
        else
           L = i;
        i = (R+L)/2;
     }
     Lraja=R;
  }
  /* Search for the right boundary */

  L=0;
  R=_index->n;
  i=(L+R)/2;
  /*if (ComparePos(_index,GetPos(_index,0),pattern,length) <= 0)
     Rraja=0;
  else {*/
     while (L < R-1) {
        if (ComparePos(_index,GetPos(_index,i),pattern,length) >= 0)
           L = i;
        else
           R = i;
        i = (R+L)/2;
     }
     Rraja=L;
  /*}*/
  printf("\n");
  printf("Count: %lu de %lu hasta %lu\n",Rraja-Lraja+1, Lraja, Rraja);fflush(stdout);
  (*numocc)=Rraja-Lraja+1;
  return 0;
}

int locate(void *index, uchar *pattern, ulong length, ulong **occ, ulong *numocc){
  TSA_Un *_index=(TSA_Un *) index;
  ulong L=0;
  ulong R=_index->n;
  ulong i=(L+R)/2;
  ulong Lraja=0;
  ulong Rraja=0;
  ulong matches=0;
  /*pattern[0]=9;
  pattern[1]=9;
  pattern[2]=9;
  pattern[3]=32;
  pattern[4]=32;

  printf("aqui parte\n");
  for (i=23119; i < 23119+10; i++)
    printf("%d ", _index->text[i]);
  printf("\naqui termina\n");
  printf("aqui parte\n");
  for (i=488; i < 488+10; i++)
    printf("%d ", _index->text[i]);
  printf("\naqui termina\n");
*/
  /* Search for the left boundary */
  if (ComparePos(_index,GetPos(_index, 0),pattern,length) <= 0)
     Lraja=0;
  else {
     while (L<R-1) {
        if (ComparePos(_index,GetPos(_index,i),pattern,length) <= 0)
           R = i;
        else
           L = i;
        i = (R+L)/2;
     }
     Lraja=R;
  }
  /* Search for the right boundary */
  L=0;
  R=_index->n;
  i=(L+R)/2;
  /*/if (ComparePos(_index,GetPos(_index,0),pattern,length) <= 0)
  //   Rraja=0;
  //else {*/
     while (L < R-1) {
        if (ComparePos(_index,GetPos(_index,i),pattern,length) >= 0)
           L = i;
        else
           R = i;
        i = (R+L)/2;
     }
     Rraja=L;
  /*}*/
  matches = Rraja-Lraja+1;
  if (matches >0) {
     ulong locate=0;
     (*occ) = (ulong *) malloc(matches*sizeof(ulong)); 
     if (!(*occ)) return 1;
     for (; Lraja<=Rraja; Lraja++) {
        (*occ)[locate]=GetPos(_index,Lraja);
        locate++;
     }
  } else
     *occ = NULL;
  (*numocc)=matches;
  return 0;
}
/*///////////////////////
//Accessing the indexed//
///////////////////////*/
int display(void *index, uchar *pattern, ulong length, ulong numc, ulong *numocc, uchar **snippet_text, ulong **snippet_lengths) {
  ulong *occ, i, j, from, to, len, x;
  ulong n = ((TSA_Un *) index)->n;
  uchar *text = ((TSA_Un *) index)->text;
  uchar *text_aux;
  int aux = locate(index, pattern, length, &occ, numocc);
  if (aux != 0) return aux;
  *snippet_lengths = (ulong *) malloc((*numocc)*sizeof(ulong));
  if (!(*snippet_lengths)) return 1;
  *snippet_text = (uchar *) malloc((*numocc)*(length+2*numc)*sizeof(uchar));
  if (!(*snippet_text)) return 1;
  text_aux=*snippet_text;
  for (i=0;i<(*numocc);i++) {
    x=occ[i];
    if (x>numc) from = x-numc;
    else from=0;
    to= ((x+length+numc-1)<(n-1)?(x+length+numc-1):(n-1));
    len =to-from+1;
    for (j=0; j<len;j++)
      text_aux[j] = text[from+j];
    text_aux+=length+2*numc;
    (*snippet_lengths)[i] = len;
  }
  free(occ);
  return 0;
}
int extract(void *index, ulong from, ulong to, uchar **snippet, ulong *snippet_length){
   ulong n = ((TSA_Un *) index)->n;
   uchar *text = ((TSA_Un *) index)->text;
   if (to >= n) to=n-1;
   if (from > to) {
     *snippet = NULL;
     *snippet_length=0;
   } else {
     ulong j;
     ulong len =to-from+1;
     *snippet = (uchar *) malloc((len)*sizeof(uchar));
     if (!(*snippet)) return 1;
     for (j=from; j<=to;j++) {
       (*snippet)[j-from]=text[j];
     }
     (*snippet_length)=len;
   }
   return 0;
}
int get_length(void *index, ulong *length){
   (*length)=((TSA_Un *) index)->n;
   return 0;
}

/*////////////////
//Error handling//
////////////////*/
char *error_index(int e){
  switch(e) {
    case 0:  return "No error"; break;
    case 1:  return "Out of memory"; break;
    case 2:  return "The text must end with a \\0"; break;
    case 5:  return "You can't free the text if you don't copy it"; break;
    case 20: return "Cannot create files"; break;
    case 21: return "Error writing the index"; break;
    case 22: return "Error writing the index"; break;
    case 23: return "Cannot open index; break";
    case 24: return "Cannot open text; break";
    case 25: return "Error reading the index"; break;
    case 26: return "Error reading the index"; break;
    case 27: return "Error reading the text"; break;
    case 28: return "Error reading the text"; break;
    case 99: return "Not implemented"; break;
    default: return "Unknown error";
  }
}


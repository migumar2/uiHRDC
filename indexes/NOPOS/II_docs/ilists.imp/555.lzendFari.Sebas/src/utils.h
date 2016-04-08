#ifndef __UTILS_H__
#define __UTILS_H__

void startTime();
unsigned long endTime();
unsigned long endTime2();
unsigned char* readText(char* filename,unsigned int* length);
int saveText(unsigned char* text,int length,char* filename);
unsigned int* invertSA(unsigned int* sa,int len);
unsigned int* readArray(char* filename,int* alen,unsigned int size);
unsigned char* reverseText(unsigned char *text,int n);
unsigned int* getSA(unsigned char* text, int len, char* filename, int* salen);
unsigned char* getL(unsigned int* sa,unsigned char* text,unsigned int len);
unsigned int* getC(unsigned int* sa,unsigned char* text,unsigned int len);
/*unsigned int* getLCP(unsigned char* text, int len, char* filename, int q,int *lcplen);*/
char* basename(char* filename);
int lenText(char* filename);
#endif

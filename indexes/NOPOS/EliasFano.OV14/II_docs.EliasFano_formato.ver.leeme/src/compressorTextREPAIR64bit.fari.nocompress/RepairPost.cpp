
#include "RepairPost.h"

	RepairPost::RepairPost()
	{
		text = NULL;
		textSize =0;
	}

  RepairPost::RepairPost(unsigned long block_size) {
		blockSize = block_size;
		cout << "\nRepair_post object initialized no text still loaded";
		fflush(stdout);
		textSize =0;
		text = NULL;
	}	

		//** Comprime la secuencia src[0..len-1] con repair, teniendo en cuenta que los bloques
    //** tienen tamaño "block_size"  

  RepairPost::RepairPost(unsigned char *src, unsigned long len, unsigned long block_size) {
		textSize = len;
		text = (unsigned char *) malloc (sizeof(unsigned char) * textSize);
		
		strncpy ( (char *)text, (const char *) src, textSize);
		
		blockSize = block_size;
		cout << "\nRepair_post object initialized with a text of len = " << len << " bytes.";
		fflush(stdout);
	}	

		//**saves the created structure.
	void RepairPost::save (char * filename) {
		int file;	
		unlink(filename);
		fprintf(stderr,"\tSaving Compressed text (repair) to disk  [%s]\n", filename);
		
		if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
			printf("Cannot open file %s\n", filename);
			exit(0);
		}	
		int errr = write(file, text, sizeof(unsigned char) * textSize); errr=0;		
		close(file);					
	}  


		//**loads the created structure.
	void RepairPost::load(char * filename) {
		int file;
	
		textSize= getFileSize(filename);
		text = (unsigned char *) malloc (sizeof(unsigned char) * textSize);
	
		fprintf(stderr,"\n\tLoading Compressed Text (repair) into memory from [%s], size = %lu bytes", filename, textSize);
		if( (file = open(filename, O_RDONLY)) < 0) {
			printf("Cannot open file %s\n", filename);
			exit(0);
		}
		int errr=read(file, text, textSize);errr=0;
		
		close(file);			
	}


		//** Devuelve el texto "sin comprimir" del bloque "i" (el primer bloque es el 0). 
		//** Lo usaré posteriormente para el horspool
	void RepairPost::extract_block(uint i, unsigned char *block, unsigned long *len) {
		unsigned long  ini, end;
		unsigned long bytes;
		
		ini = i * blockSize;
		end = (i+1) * blockSize;
		end = (end >= textSize) ? textSize : end;
		bytes = end -ini;
		
		unsigned char *dst;// = (unsigned char *) malloc (sizeof(unsigned char) * bytes);
		dst=block;
		strncpy ((char *) dst, (const char *) &(text[ini]), bytes);
		//*block = dst;
		*len = bytes;
	}

		//** Memoria utilizada  (secuencia comprimida + ptr[] + jerarquía frases) 
	unsigned long RepairPost:: get_size() {
			return textSize;
	}

		//** Frees the resources used by this object.
	RepairPost::~RepairPost() {
	 // delete [] symbols;	  
	 // if (BRR) delete BRR;
	   if (text) free(text);
	  	
	}


/*************************************************************/
/** private methods   ****************************************/
unsigned long RepairPost::getFileSize (char *filename){
	FILE *fpText;
	unsigned long fsize;
	fpText = fopen(filename,"rb");
	fsize=0;
	if (fpText) {
		fseek(fpText,0,2);
		fsize= ftell(fpText);
		fclose(fpText);
		////fprintf(stderr,"fileSize = %ld",fsize);
	}
	return fsize;
}

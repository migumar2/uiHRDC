	
What can you find here?
Just this:
	1. original_sources
	2. LZMA_C[original header except uncomp size]
	3. LZMA_C[simplifiedHeader]

Where:

***********
	1. original_sources
	This are the original source code for the decompression of lzma data. No changes done her.


***********
	2. LZMA_C[original header except uncomp size]
	I prepared an interface to allow lzma-compression (via command line call to lzmaEncoder), and decompression.
	The header size of the compressed lzma file was kept, but the 8 bytes allocated originally to keep the value
	"uncompressed-size" were changed to only 4byte (uint 32).

	Format=
		[flag][properties (dict-size)] [uncompressed size] [vocabulary + compressed data]
		 1byte       4bytes                  4 bytes            (file-len) -9 bytes

***********	
	3. LZMA_C[simplifiedHeader]
	This version simplifies (2), by requiring less space in the header from 9 to [2..5] bytes.
	The dictionary size is not needed ==> 4 bytes are not needed.
	The uncompressed size is encoded using "bytecodes" ==> requires 1..4 bytes instead of 4bytes.

	Therefore, we skip from 4 to 7 bytes in the header. The maximum uncompressed size is reduced from 4gb to 512Mb.

	In 2010.12.28, the filename of the tmp file used during compression/decompression includes the "getpid()".
	This permits multiple calls to the api even with programs running in the same directory.
***********






# LZ
The current LZ implementation performs sliglthly simple optimizations over the
original sources available at: http://pizzachili.dcc.uchile.cl/indexes/LZ77-index/.
More information about this class of self-indexes can be found at [1].

[1] S. Kreft and G. Navarro. On Compressing and Indexing Repetitive Sequences. Theoretical Computer Science 483:115-133, 2013.


A simple 'make' is only required for compiling these self-indexes. Executable binaries
will be available in the `bin` folder.


###### Indexing
The indexing process is similar for LZ77 and LZend self-indexes. You must
excute the following instruction (within the `bin` folder) for building a
LZ77 index (for LZend, you must replace `build_lz77` by `build_lzend`):

  `./build_lz77 <collection> <destination> <conf>`

- `<collection>` refers to the path of the input text in the filesystem.
- `<destination>` refers to the path of the output index file in the 
      filesystem.
- `<conf>` refers to the multiple configurations which can be chosen for
      building the self-index. If the parameter is not provided, th by-default
      configuration is assumed. It reports the best performance but also the 
      worst space. The following four values `bsst ssst`, `brev`, `bsst brev ssst`, 
      `bsst brev`, are also allowed. Consider that the latter one reports the
      best numbers in compression, while assures a competitive performance.

###### Pattern Location
A pattern location evaluation can be easily performed using the `locate` script
in the bin `folder`:

  `./locate <index> <docs>`

- `<index>` refers to the path of the index files in the filesystem.
- `<docs>` refers to the path of the file containing document positions. Note 
      that this parameter is requiered for seaching document text collections,
      but it would not be necessary in other scenarios (the current SLP
      implementation is tuned for natural language).

The file containing the patterns to be searched are redirected from the input.


###### Snippet Extraction
Snippet extraction can be also easily evaluated using SLPIndex and wSLPindex
binaries. In this case, the instruction is similar in both cases:

  `./extract <index> `

The `<index>` parameter also refers the index file in the filesystem, and the file 
containing the snippet boundary positions are also redirected from the input.
 
###### Scripts
This folder contains three scripts which help you to reproduce the experiments
reported in our paper:

- `goBuildAll.sh` compiles the self-indexes and builds an evaluation directory 
     which is populated with the SLP indexes (their construction takes many time) 
      and experimental data to test them.
- `goSearchAll.sh` runs pattern location and snippet extraction experiments and
      obtains log files with interesting statistics.
- `goCleanAll.sh` cleans all binaries and deletes the evaluation directory.
 


For more information, please feel free to contact us:

- Francisco Claude: \<fclaude@recoded.cl\>
- Antonio Fariña: \<fari@udc.es\>
- Miguel A. Martínez-Prieto: \<migumar2@infor.uva.es\>
- Gonzalo Navarro:  \<gnavarro@dcc.uchile.cl\>



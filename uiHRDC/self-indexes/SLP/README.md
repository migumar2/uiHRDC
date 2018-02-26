# SLP
SLP self-indexes are designed on the concept of *Straight Line Program (SLP)*
and are based on grammar-based compression. In this case, we use Re-Pair [1] as
core compressor and all index information is organized using a configuration of
succinct data structures from LIBCDS. We propose two SLP-based techniques which
are strongly inspired by the approach described in [2]:

- SLP is a character-oriented self-index which processes input texts as 
  sequences of bytes. It means that can be applied to any class of repetitive 
  text collection, not just natural language ones.
- WSLP adapts the SLP self-index to cope with particularities of natural
  language text collextions. It preprocesses the input text to identify
  its lexicon, and compresses it as a sequence a words.

[1] N.J. Larsson and A. Moffat. Off-line dictionary-based compression. Proc. IEEE, 88(11):1722–1732, 2000.

[2] F. Claude and G Navarro. Self-Indexed Grammar-Based Compression. Fundamenta Informaticae 111(3):313-337, 2011



This folder contains sources for our two SLP-based self indexes:

  + `libcds`: contains the library of succinct data structures which we use for
      implementing the self-indexes.
  + `slp`: contains all sources for the SLP self-index.
  + `wslp`: contains all sources for the WSLP self-index.
  + `scripts`: contains some scripts which assist into indexes evaluation.

Consider the following steps for compiling our self-indexes:

1. For building the libcds library, type "make" within the libcds folder.
2. For compiling the SLP (similar for WSLP), type "make" within its 
    corresponding folder.

Once compiled, SLPIndex and WSLPIndex are ready (in slp and wslp folders 
respectively) to be used:

###### Indexing
The indexing process is quite similar in both cases. For building the SLP
self-index for a given <collection>, you must execute the following 
instruction (within the slp folder):

  `./SLPIndex -i <collection> <destination> <q>`

- `<collection>` refers to the path of the input text in the filesystem.
- `<destination>` refers to the parh of the output index file in the 
      filesystem.
- `<q>` refers to the size of the q-grams indexed for speeding up pattern
      location (by default, `q=4`).

The parameter `<q>` does not apply for the WSLP. However, it is worth noting 
that, in this case, the directory which contains the collection must also 
include an additional file which stores initial position for each document in 
the collection. This file must be called as the collection file, but with 
extension '.DOCBOUNDARIES'. The instruction for the WSLP is the following:

`./wSLPIndex -i <collection> <destination>`

###### Pattern Location
A pattern location evaluation can be easily performed using SLPIndex/wSLPindex
binaries. For the SLP, you must run this instruction:

  `./SLPIndex -l <index> <docs> <n_patterns>`

- `<index>` refers to the path of the index files in the filesystem.
- `<docs>` refers to the path of the file containing document positions. Note 
      that this parameter is required for seaching document text collections,
      but it would not be necessary in other scenarios (the current SLP
      implementation is tuned for natural language).
- `<n_patterns>` indicates the number of patterns to be located from the test 
      file (redirected as input of the SLPIndex binary).

The WSLP is used similarly. Note that, in this case, the <docs> parameter is not
required because document boundaries are included in the self-index:

  `./wSLPIndex -l <index> <n_patterns>`

The file containing the patterns to be searched are redirected from the input.

###### Snippet Extraction
Snippet extraction can be also easily evaluated using SLPIndex and wSLPindex
binaries. In this case, the instruction is similar in both cases:

  `./SLPIndex -e <dest> <n_snippets>`
  `./wSLPIndex -e <dest> <n_snippets>`

- `<index>` refers to the path of the index files in the filesystem.
- `<n_snippets>` indicates the number of snippets to be extracted from the test
      file (redirected as input of the SLPIndex binary).

The file containing the snippet boundary positions are redirected from the input.

###### Scripts
This folder contains three scripts which help you to reproduce the experiments
reported in our paper:

- `goBuildAll.sh` compiles the self-indexes and builds an evaluation directory 
     which is populated with the SLP indexes (their construction takes many time) 
      and experimental data to test them.
- `goSearchAll.sh` runs pattern location and snippet extraction experiments and
      obtains log files with interesting statistics.
- `goCleanAll.sh` cleans all binaries and deletes the evaluation directory.





If you have any doubt or need more information, please feel free to contact us:

- Francisco Claude: \<fclaude@recoded.cl\>
- Antonio Fariña: \<fari@udc.es\>
- Miguel A. Martínez-Prieto: \<migumar2@infor.uva.es\>
- Gonzalo Navarro:  \<gnavarro@dcc.uchile.cl\>

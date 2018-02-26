# Data

This `data` directory contains all files used in the experimental setup provided 
in the paper **"Universal Indexes for Highly Repetitive Document Collections"** 
subtmitted to *Information Systems Journal in April 2016 by Claude, F; Fariña, A.; 
Martinez-Prieto, M.; and Navarro, G.*. In addition, we provide some (small) 
additional files which allow to reproduce a comparable experimental evaluation in
a less powerful computational configuration.

+ The `texts` folder contains different text collections (compressed with 7z) which 
  can be indexed with all of our techniques:
  - `textwiki20gb` and `textwiki2gb` are the text collections used respectively for 
    evaluating non-positional and positional/self-indexes in our paper.
  - `text200mb` and `einstein` are small repetitive collection which can be also used
    for evaluating our techniques in a less powerful computer. We usually use `text200mb`
    for non-positional indexes and `einstein` for positional and self-index ones.
  You can decompress all collections using the `uncompress.sh` script (it requires 7zip
  to be installed in your computer).

+ The `intervals` folder organizes eight different files which contains pairs of
  text positions describing snippets. That is, the pair `[a,b]` is used for
  extracting the snippet `text[a]` to `text[b]`. We provide two sets of files:
  - The set of `wiki2gb`-prefixed files describe snippets for the text collection
    `textwiki2gb` used for analyzing extraction performance in our paper. We consider 
    snippets of 80 chars (around one line) and 13,000 chars (around one document in the 
    collection). In addition, we describe snippets of 20 and 3,000 words for word-oriented 
    indexes.
  - The set of `einstein`-prefixes files describe similar snippets for the 
    (small) `einstein` text collection.

+ The `patterns` folder contains sixteen different files which contains patterns of different
  length to be searched in their corresponding texts. In all cases, we consider 1-word patterns
  with low (1-1000 occurrences) and high frequency (more than 1000 ocurrences). Moreover, we
  test phrases of 2 and 5 words respectively:
  - `wiki2gb` and `wiki20gb`-prefixed files contains patterns to be respectively located on
    the `textwiki2gb` and `textwiki20gb`. These are the patterns used in the experiments of
    our paper.
  - `text200mb` and `einstein`-prefixed files are used for pattern location on the corresponding
    text collections.


If you have any doubt or need more information, please feel free to contact us:

- Francisco Claude: \<fclaude@recoded.cl\>
- Antonio Fariña: \<fari@udc.es\>
- Miguel A. Martínez-Prieto: \<migumar2@infor.uva.es\>
- Gonzalo Navarro:  \<gnavarro@dcc.uchile.cl\>

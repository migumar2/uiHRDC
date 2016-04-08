
We include here source code for the WCSA used in our paper:

"Universal Indexes for Highly Repetitive Document Collections"
submitted to Information Systems Journal in April 2016 
by Claude, F; Fariña, F.; Martinez-Prieto, M.; and Navarro, G.

It is an adaptation of the source code used in our paper:
Brisaboa, N. R.; Fariña, A.; Navarro, G.; Paramá, J. R.: 
"Dynamic Lightweight Text Compression", en ACM Transactions on Information Systems (TOIS), 
28(3), ACM, New York, NY (Estados Unidos), 2010, pp. 1-32.

http://vios.dc.fi.udc.es/wsi

The main modification consisted in allowing WCSA to work in a doc-oriented fashion.
Therefore we provide to the indexer not only a dataset, but a dataset (doc-collection) and the
doc-beginnings (absolute positions were each doc starts with the doc-collection).

Then, we adapted locate to return not actual absolute
positions within a collection, but pairs of <doc,rel-pos> values, that is, 
it returns in which docs a pattern occurs and also the positions within those documents.


-----
We include the source code in directory "sourceCode" and scripts to run our experiments 
in the folder "scripts.swcsa3.is"
  
The resulting results will be saved in 
  (locate)
	N.Wa_swcsa.dat  --> low freq words
	N.Wb_swcsa.dat  --> high freq words
	N.P2_swcsa.dat  --> 2-words phrases
	N.P5_swcsa.dat  --> 5-words phrases

	e.Words20_swcsa.dat    --> extracting 20 words sequences
	e.Words3000_swcsa.dat  --> extracting 3000 words sequences


-----
Steps to run experiments.

cd data
sh go2gb.sh           --> sets links to data/patterns/intervals from root folder (dataset for INF. systems paper)
## sh goEinstein.sh   --> would set links to data/patterns/intervals from root folder (test smaller dataset).
cd ..
cd sourceCode  --> makes source code.
make
cd ..
cd scripts.swcsa3.is
sh goAll.sh    --> runs experiments.
 




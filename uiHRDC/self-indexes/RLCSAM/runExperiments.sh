cd data
sh go2gb.sh         ##  --> sets links to data/patterns/intervals from root folder (dataset for INF. systems paper)
## sh goEinstein.sh ##  --> would set links to data/patterns/intervals from root folder (test smaller dataset).

cd ..
cd rlcsa # --> makes source code.
make clean 
make
cd ..

cd scripts
sh clean.sh
sh goAll.sh #    --> runs experiments (build, locate, extract)
sh clean.sh
cd ..

cd data
sh clean.sh
cd ..


echo "-------------------------------------------"
echo "Preparing links to source data and patterns"
echo "-------------------------------------------"

cd data
sh go2gb.sh         ##  --> sets links to data/patterns/intervals from root folder (dataset for INF. systems paper)
## sh goEinstein.sh ##  --> would set links to data/patterns/intervals from root folder (test smaller dataset).


echo "-------------------------------------------"
echo "Compiling sourceCode                       "
echo "-------------------------------------------"
cd ..
cd sourceCode  #--> makes source code.
make clean
make
cd ..

echo "-------------------------------------------"
echo "Running experiments                        "
echo "-------------------------------------------"
cd scripts.swcsa3.is
sh goAll.sh    --> runs experiments.
sh clean.sh
cd ..

echo "-------------------------------------------"
echo "Cleaning non-needed data"
echo "-------------------------------------------"

cd data
sh clean.sh
cd ..


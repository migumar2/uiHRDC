sh clean.sh

ln -s ../sourceCode/pizza/BUILDwcsa  BUILDwcsa 
ln -s ../sourceCode/pizza/SEARCHwcsa SEARCHwcsa 
mkdir -p indexes
./goBuild
./goSearch



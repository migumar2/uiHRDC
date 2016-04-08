rm -f BUILD
ln -s ../II_docs.EliasFano_formato.ver.leeme/BUILD_VBYTE_PLAIN   BUILD
mkdir -p indexes

echo "##################################################################" 
echo " Generating source files for Ottaviano-Venturini sigir 2014-format"
echo "##################################################################" 

echo "################################################################" 
./BUILD ../text2gb/wiki_src2gb.txt indexes/wiki2gb_ts1 "sample_ct=1"   
echo "################################################################"


echo "################################################################"
mv out.docs  ../TESTDATA/2gb.docs
mv out.freqs ../TESTDATA/2gb.freqs
mv out.sizes ../TESTDATA/2gb.sizes

echo "Those files are in ../TESTDATA directory"
echo "################################################################"

rm -f postings.posts*
rm -f times_info_BUILD.txt

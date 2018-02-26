ln -s ../II_docs.EliasFano_formato.ver.leeme/BUILDP_VBYTE_NOTEXT   BUILDP
ln -s ../II_docs.EliasFano_formato.ver.leeme/BUILD_VBYTE_NOTEXT   BUILD
ln -s ../II_docs.EliasFano_formato.ver.leeme/SEARCH_VBYTE_NOTEXT  SEARCH
mkdir indexes


echo "##################################################################" 
echo " Generating source files for Ottaviano-Venturini sigir 2014-format"
echo "##################################################################" 


#./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap8 "lenBitmapDiv=8"  
#./BUILDP ../../20gb indexes/wiki20gb_bitmap8 "lenBitmapDiv=8"  
#./BUILDP ../../20gb indexes/wiki20gb_bitmap1 "lenBitmapDiv=1; 2.1.scripts.vbytebitmap"  
./BUILD ../textos/text20gb.txt indexes/wiki20gb_bitmap1 "lenBitmapDiv=1; 2.1.scripts.vbytebitmap"  

echo "################################################################"

mv out.docs  ../TESTDATA/20gb.docs
mv out.freqs ../TESTDATA/20gb.freqs
mv out.sizes ../TESTDATA/20gb.sizes

rm -f postinglists.posts*
rm times_info_BUILD.txt

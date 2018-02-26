ln -s ../II_docs.EliasFano_formato.ver.leeme/SEARCH_VBYTE_NOTEXT  SEARCH

echo "################################################################" 
rm -f E.dat
rm -f N.dat

./SEARCH indexes/wiki20gb_bitmap1 E 1 "int2=merge;intn=svs;fsearch=seqM" < ../patterns/wiki20g.words.f1_1000
mv fids.pat wiki20g.wa.ids

./SEARCH indexes/wiki20gb_bitmap1 E 1 "int2=merge;intn=svs;fsearch=seqM" < ../patterns/wiki20g.words.f1001_100k
mv fids.pat wiki20g.wb.ids

./SEARCH indexes/wiki20gb_bitmap1 N 2 "int2=merge;intn=svs;fsearch=seqM" < ../patterns/wiki20g_2_2
mv fids.pat wiki20g.p2.ids

./SEARCH indexes/wiki20gb_bitmap1 N 2 "int2=merge;intn=svs;fsearch=seqM" < ../patterns/wiki20g_5_5
mv fids.pat wiki20g.p5.ids


mv wiki20g.* ../TESTDATA

rm -f indexes/*
echo "################################################################"



rm -f SEARCH
rm -f ????.2.1.vbyte.dat
ln -s ../II_docs.EliasFano_formato.ver.leeme/SEARCH_VBYTE_PLAIN  SEARCH

echo "################################################################" 
rm -f E.dat
rm -f N.dat 

echo "#####################################################################" 
echo " Generating search patterns for Ottaviano-Venturini sigir 2014-format"
echo "   .. and computing parsing [a] and merge2occs [c] times from our    "
echo "      source code. (Recall ../Readme.howto.txt)                      "
echo "#####################################################################" 

./SEARCH indexes/wiki2gb_ts1 E 200 "int2=merge;intn=svs;fsearch=seqM" < ../patterns/wiki2g.words.f1_1000
mv E.dat E_Wa.2.1.vbyte.dat
mv fids.pat 2gb.wa.pat

./SEARCH indexes/wiki2gb_ts1 E 200 "int2=merge;intn=svs;fsearch=seqM" < ../patterns/wiki2g.words.f1001_100k
mv E.dat E_Wb.2.1.vbyte.dat
mv fids.pat 2gb.wb.pat

./SEARCH indexes/wiki2gb_ts1 N 3 "int2=merge;intn=svs;fsearch=seqM" < ../patterns/wiki2g_2_2
mv N.dat N_p2.2.1.vbyte.dat
mv fids.pat 2gb.p2.pat

./SEARCH indexes/wiki2gb_ts1 N 3 "int2=merge;intn=svs;fsearch=seqM" < ../patterns/wiki2g_5_5
mv N.dat N_p5.2.1.vbyte.dat
mv fids.pat 2gb.p5.pat


echo "################################################################"
mv 2gb.??.pat ../TESTDATA
echo "The patterns files are in ../TESTDATA directory"
echo "The partial times [a] and [c] are in files"
echo "    E_Wa.2.1.vbyte.dat    (Wa=low freq patterns)"
echo "    E_Wb.2.1.vbyte.dat    (Wb=high freq patterns)"
echo "    N_p2.2.1.vbyte.dat    (p2= 2-word phrases)"
echo "    N_p5.2.1.vbyte.dat    (p5= 5-word phrases)"
echo "################################################################"

rm -f indexes/*
rm -f log.il.txt times_?_SEARCH.txt


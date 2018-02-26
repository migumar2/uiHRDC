########################################################################################
#### LOCATE OF PATTERNS (retrieving <doc-id,offset> pairs  #############################
########################################################################################
#rm -f e.dat
rm -f m.dat

echo "*****************************************************************************" >>times_m_SEARCH.txt
echo "********************  Intervals Set 1                ************************" >>times_m_SEARCH.txt
echo "*****************************************************************************" >>times_m_SEARCH.txt

./SEARCH ./indexes/wiki2gb_ts1  m  "noopts" < ../../intervals/wiki2g.intervals.80chars.txt
#./SEARCH ./indexes/wiki2gb_ts2  m  "noopts" < ../../intervals/wiki2g.intervals.80chars.txt
./SEARCH ./indexes/wiki2gb_ts8  m  "noopts" < ../../intervals/wiki2g.intervals.80chars.txt
./SEARCH ./indexes/wiki2gb_ts32  m  "noopts" < ../../intervals/wiki2g.intervals.80chars.txt
#./SEARCH ./indexes/wiki2gb_ts64  m  "noopts" < ../../intervals/wiki2g.intervals.80chars.txt
./SEARCH ./indexes/wiki2gb_ts256  m  "noopts" < ../../intervals/wiki2g.intervals.80chars.txt
./SEARCH ./indexes/wiki2gb_ts4k  m  "noopts" < ../../intervals/wiki2g.intervals.80chars.txt

##mv m.dat m.80chars_ii_repairT.dat
mv m.80chars_ii_repairT.dat m.80char.tmp
cat m.dat m.80char.tmp > m.80chars_ii_repairT.dat
rm m.80char.tmp

rm -f m.dat

echo "*****************************************************************************" >>times_m_SEARCH.txt
echo "********************  Intervals Set 2                ************************" >>times_m_SEARCH.txt
echo "*****************************************************************************" >>times_m_SEARCH.txt

./SEARCH ./indexes/wiki2gb_ts1  m  "noopts" < ../../intervals/wiki2g.intervals.13000chars.txt
#./SEARCH ./indexes/wiki2gb_ts2  m  "noopts" < ../../intervals/wiki2g.intervals.13000chars.txt
./SEARCH ./indexes/wiki2gb_ts8  m  "noopts" < ../../intervals/wiki2g.intervals.13000chars.txt
./SEARCH ./indexes/wiki2gb_ts32  m  "noopts" < ../../intervals/wiki2g.intervals.13000chars.txt
#./SEARCH ./indexes/wiki2gb_ts64  m  "noopts" < ../../intervals/wiki2g.intervals.13000chars.txt
./SEARCH ./indexes/wiki2gb_ts256  m  "noopts" < ../../intervals/wiki2g.intervals.13000chars.txt
./SEARCH ./indexes/wiki2gb_ts4k  m  "noopts" < ../../intervals/wiki2g.intervals.13000chars.txt

#mv m.dat m.13000chars_ii_repairT.dat
mv m.13000chars_ii_repairT.dat m.13000char.tmp
cat m.dat m.13000char.tmp > m.13000chars_ii_repairT.dat
rm m.13000char.tmp

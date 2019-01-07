ln -s ../../II_docs/BUILDP_RICE_RLE_NOTEXT BUILDP
ln -s ../../II_docs/BUILD_RICE_RLE_NOTEXT BUILD
ln -s ../../II_docs/SEARCH_RICE_RLE_NOTEXT SEARCH
mkdir indexes

rm -f B-LOG.dat*
python ../../../../../utils-py/starttime.py  B-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES

echo "################################################################" 

#./BUILD ../../textos/text20gb.txt indexes/wiki20gb "nooptions" 
#./BUILDP ../../20gb indexes/wiki20gb "nooptions;  1.5.scripts.rice.rle"
./BUILD ../../textos/text20gb.txt indexes/wiki20gb "nooptions;  1.5.scripts.rice.rle"


echo "################################################################"
python ../../../../../utils-py/endtime.py    B-LOG.dat                  ##LOGS-ELAPSED-TIMES


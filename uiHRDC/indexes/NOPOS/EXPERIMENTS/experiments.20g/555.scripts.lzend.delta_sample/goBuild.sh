ln -s ../../II_docs/BUILDP_LZEND_IL_NOTEXT  BUILDP
ln -s ../../II_docs/BUILD_LZEND_IL_NOTEXT  BUILD
ln -s ../../II_docs/SEARCH_LZEND_IL_NOTEXT SEARCH
mkdir indexes

echo "################################################################" 

#./BUILD ../textos/torsen.text200mb.txt  indexes/torsen200mb  "nooptions"

#./BUILD  ../../textos/text20gb.txt indexes/torsen20gb "nooptions"
#./BUILDP ../../20gb indexes/torsen20gb_ds04  "nooptions;lzend555; delta_sampling=4"  
#./BUILDP ../../20gb indexes/torsen20gb_ds16  "nooptions;lzend555; delta_sampling=16"  
#
#./BUILDP ../../20gb indexes/torsen20gb_ds64  "nooptions;lzend555; delta_sampling=64"  
#./BUILDP ../../20gb indexes/torsen20gb_ds256 "nooptions;lzend555; delta_sampling=256"  


./BUILD ../../textos/text20gb.txt indexes/torsen20gb_ds04  "nooptions;lzend555; delta_sampling=4"  
./BUILD ../../textos/text20gb.txt indexes/torsen20gb_ds16  "nooptions;lzend555; delta_sampling=16"  
                                  
./BUILD ../../textos/text20gb.txt indexes/torsen20gb_ds64  "nooptions;lzend555; delta_sampling=64"  
./BUILD ../../textos/text20gb.txt indexes/torsen20gb_ds256 "nooptions;lzend555; delta_sampling=256"  


rm tmp_filename_il*


echo "################################################################"



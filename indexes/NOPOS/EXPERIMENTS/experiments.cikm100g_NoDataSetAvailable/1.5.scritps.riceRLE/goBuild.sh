ln -s ../../II_docs_large/BUILDP_RICE_RLE_NOTEXT BUILDP
ln -s ../../II_docs_large/BUILD_RICE_RLE_NOTEXT BUILD
ln -s ../../II_docs_large/SEARCH_RICE_RLE_NOTEXT SEARCH
mkdir indexes

echo "################################################################" 

#./BUILD ../../../textos/suel100gb.txt indexes/wiki100gb "nooptions" 

./BUILDP ../wiki100gb indexes/wiki100gb "nooptions;  1.5.scripts.rice.rle"

echo "################################################################"



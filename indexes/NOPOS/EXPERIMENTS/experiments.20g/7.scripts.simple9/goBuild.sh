ln -s ../../II_docs/BUILDP_SIMPLE9_NOTEXT  BUILDP
ln -s ../../II_docs/BUILD_SIMPLE9_NOTEXT  BUILD
ln -s ../../II_docs/SEARCH_SIMPLE9_NOTEXT SEARCH
mkdir indexes

echo "################################################################" 

#./BUILD ../../textos/text20gb.txt indexes/wiki20gb "nooptions"
#./BUILDP ../../20gb indexes/wiki20gb "nooptions; 7.simple9" 

./BUILD ../../textos/text20gb.txt indexes/wiki20gb "nooptions; 7.simple9"

echo "################################################################"



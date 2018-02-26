ln -s ../../II_docs/BUILD_SIMPLE9_REPAIR_T   BUILD
ln -s ../../II_docs/SEARCH_SIMPLE9_REPAIR_T  SEARCH
mkdir indexes

echo "################################################################" 

./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb "s9noparamsForBuild" 

echo "################################################################"



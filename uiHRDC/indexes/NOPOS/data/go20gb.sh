rm -rf  patterns/ textos/

mkdir -p textos
mkdir -p patterns


## point to 20gb text collection
ln -s ../../../../data/texts/text20gb.txt                                 textos/text20gb.txt
ln -s ../../../../data/texts/text20gb.txt.DOCBOUNDARIES.ul                textos/text20gb.txt.DOCBOUNDARIES.ul
ln -s ../../../../data/texts/text20gb.txt.lens                            textos/text20gb.txt.lens

ln -s ../../../../data/patterns/wiki20g.words.f1_1000                     patterns/wiki20g.words.f1_1000    
ln -s ../../../../data/patterns/wiki20g.words.f1001_100k                  patterns/wiki20g.words.f1001_100k 
ln -s ../../../../data/patterns/wiki20g_2_2                               patterns/wiki20g_2_2              
ln -s ../../../../data/patterns/wiki20g_5_5                               patterns/wiki20g_5_5      

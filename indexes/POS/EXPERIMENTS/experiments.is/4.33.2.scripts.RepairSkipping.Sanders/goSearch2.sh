#!/bin/bash
rm -f E.dat
rm -f N.dat
ln -s ../../II_docs/SEARCH_REPAIR_SKIP_SANDERS_REPAIR_T SEARCH
ln -s ../../II_docs/lib lib
echo "################################################################" 

rm -f E.dat
rm -f N.dat


echo "################################################################"


SAMPLEK="2 4 16 64 256"
#SAMPLEK="16"

###
	for i in $SAMPLEK;
	do
		echo "- [Wa]-- II-REPAIR-SKIPPING-SAMPLING-SANDERS SAMPLEK = $i -----------------------"
		./sampleK indexes/wiki2gb $i		
#		./SEARCH indexes/wiki2gb E 1 "int2=skip;intn=skip"   < ../../patterns/wiki2g.words.f1_1000
		./SEARCH indexes/wiki2gb E 20 "int2=lookup;intn=lookup" < ../../patterns/wiki2g.words.f1_1000
		./appendLastLine E.dat sampleK=$i
	done
	mv  E.dat  E_Wa.4.3.2repairG.skipping.lookup.dat 
###
echo "################################################################"

SAMPLEK="2 4 16 64 256"
#SAMPLEK="16"

###
	for i in $SAMPLEK;
	do
		echo "- [Wb]-- II-REPAIR-SKIPPING-SAMPLING-SANDERS SAMPLEK = $i -----------------------"
		./sampleK indexes/wiki2gb $i		
#		./SEARCH indexes/wiki2gb E 1 "int2=skip;intn=skip"   < ../../patterns/wiki2g.words.f1001_100k
		./SEARCH indexes/wiki2gb E 20 "int2=lookup;intn=lookup" < ../../patterns/wiki2g.words.f1001_100k
		./appendLastLine E.dat sampleK=$i
	done
	mv  E.dat  E_Wb.4.3.2repairG.skipping.lookup.dat 
###
echo "################################################################" 


SAMPLEK="2 4 16 64 256"
#SAMPLEK="16"

###
	for i in $SAMPLEK;
	do
		echo "- [P2]-- II-REPAIR-SKIPPING-SAMPLING-SANDERS SAMPLEK = $i -----------------------"
		./sampleK indexes/wiki2gb $i		
#		./SEARCH indexes/wiki2gb N 1 "int2=skip;intn=skip"   < ../../patterns/wiki2g_2_2
		./SEARCH indexes/wiki2gb N 5 "int2=lookup;intn=lookup" < ../../patterns/wiki2g_2_2
		./appendLastLine N.dat sampleK=$i
	done
	mv N.dat N_p2.4.3.2repairG.skipping.lookup.dat 
###
echo "################################################################"


SAMPLEK="2 4 16 64 256"
#SAMPLEK="16"

###
	for i in $SAMPLEK;
	do
		echo "- [P4]-- II-REPAIR-SKIPPING-SAMPLING-SANDERS SAMPLEK = $i -----------------------"
		./sampleK indexes/wiki2gb $i		
#		./SEARCH indexes/wiki2gb N 1 "int2=skip;intn=skip"   < ../../patterns/wiki2g_5_5
		./SEARCH indexes/wiki2gb N 5 "int2=lookup;intn=lookup" < ../../patterns/wiki2g_5_5
		./appendLastLine E.dat sampleK=$i
	done
	mv N.dat N_p5.4.3.2repairG.skipping.lookup.dat 
###

echo "################################################################"





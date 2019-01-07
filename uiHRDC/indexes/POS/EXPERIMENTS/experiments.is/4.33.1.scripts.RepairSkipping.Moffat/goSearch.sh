rm -f S-LOG.dat*
python ../../../../../utils-py/starttime.py  S-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES

#!/bin/bash
rm -f E.dat
rm -f N.dat
ln -s ../../II_docs/SEARCH_REPAIR_SKIP_MOFFAT_REPAIR_T SEARCH
ln -s ../../II_docs/lib lib
echo "################################################################" 

rm -f E.dat
rm -f N.dat


echo "################################################################" 
#./SEARCH indexes/wiki20gb E  500 "int2=merge;intn=merge" < ../../patterns/wiki20g.words.f1_1000
#SAMPLEK="1 4 16 64"
SAMPLEK="1 64"
#SAMPLEK="16"

###
	for i in $SAMPLEK;
	do
		echo "- [Wa]-- II-REPAIR-SKIPPING-SAMPLING-MOFFAT SAMPLEK = $i -----------------------"
		./sampleK indexes/wiki2gb $i		
	#	./SEARCH indexes/wiki2gb E 1 "int2=skip;intn=skip"   < ../../patterns/cr.xt100k_1
		./SEARCH indexes/wiki2gb E 20 "int2=skipE;intn=skipE" < ../../patterns/wiki2g.words.f1_1000
		./appendLastLine E.dat sampleK=$i
	done
	mv  E.dat  E_Wa.4.3.1repairG.skipping.moffat.dat 
###


echo "################################################################"
#./SEARCH indexes/wiki20gb E  500 "int2=merge;intn=merge" < ../../patterns/wiki2g.words.f1001_100k
#SAMPLEK="1 4 16 64"
SAMPLEK="1 64"
#SAMPLEK="16"

###
	for i in $SAMPLEK;
	do
		echo "- [Wb]-- II-REPAIR-SKIPPING-SAMPLING-MOFFAT SAMPLEK = $i -----------------------"
		./sampleK indexes/wiki2gb $i		
	#	./SEARCH indexes/wiki2gb E 1 "int2=skip;intn=skip"   < ../../patterns/cr.xt100k_1
		./SEARCH indexes/wiki2gb E 20 "int2=skipE;intn=skipE" < ../../patterns/wiki2g.words.f1001_100k
		./appendLastLine E.dat sampleK=$i
	done
	mv  E.dat  E_Wb.4.3.1repairG.skipping.moffat.dat 
###


echo "################################################################"
#./SEARCH indexes/wiki20gb N   4 "int2=merge;intn=merge" < ../../patterns/wiki2g_2_2
#SAMPLEK="1 4 16 64"
SAMPLEK="1 64"
#SAMPLEK="16"

###
	for i in $SAMPLEK;
	do
		echo "- [P2]-- II-REPAIR-SKIPPING-SAMPLING-MOFFAT SAMPLEK = $i -----------------------"
		./sampleK indexes/wiki2gb $i		
	#	./SEARCH indexes/wiki2gb N 1 "int2=skip;intn=skip"   < ../../patterns/cr.xt100k_2
		./SEARCH indexes/wiki2gb N 5 "int2=skipE;intn=skipE" < ../../patterns/wiki2g_2_2
		./appendLastLine N.dat sampleK=$i
	done
	mv N.dat N_p2.4.3.1repairG.skipping.moffat.dat 
###

echo "################################################################"

#./SEARCH indexes/wiki20gb N   4 "int2=merge;intn=merge" < ../../patterns/wiki2g_5_5
#SAMPLEK="1 4 16 64"
SAMPLEK="1 64"
#SAMPLEK="16"


###
	for i in $SAMPLEK;
	do
		echo "- [P5]-- II-REPAIR-SKIPPING-SAMPLING-MOFFAT SAMPLEK = $i -----------------------"
		./sampleK indexes/wiki2gb $i		
	#	./SEARCH indexes/wiki2gb N 1 "int2=skip;intn=skip"   < ../../patterns/cr.xt100k_4
		./SEARCH indexes/wiki2gb N 5 "int2=skipE;intn=skipE" < ../../patterns/wiki2g_5_5
		./appendLastLine N.dat sampleK=$i
	done
	mv N.dat N_p5.4.3.1repairG.skipping.moffat.dat 
###
echo "################################################################"



python ../../../../../utils-py/endtime.py    S-LOG.dat                  ##LOGS-ELAPSED-TIMES

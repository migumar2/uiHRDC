

#SAMPLEK="2 4 16 64 256"
SAMPLEK="16  256"
#SAMPLEK="16"

###
	for i in $SAMPLEK;
	do
		echo "- [P4]-- II-REPAIR-SKIPPING-SAMPLING-SANDERS SAMPLEK = $i -----------------------"
		./sampleK indexes/wiki2gb $i		
#		./SEARCH indexes/wiki2gb N 1 "int2=skip;intn=skip"   < ../../patterns/wiki2g_5_5
		./SEARCH indexes/wiki2gb N 5 "int2=lookup;intn=lookup" < ../../patterns/wiki2g_5_5
		./appendLastLine N.dat sampleK=$i
	done
	mv N.dat N_p5.4.3.2repairG.skipping.lookup.dat 
###

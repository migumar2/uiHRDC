This folder contains some python scripts that are used for timing purposes.
Assume you whan to measure elapsed-time for a given experiment "experiment.sh".

You can basically do:
  
	python starttime.py  OUT.dat "Build-starts!"
	sh experiment.sh
	python endtime.py OUT.dat

Scripts starttime and endtime.py register the initial and ending time respectively. 
In addition endtime.py calculates the ELAPSED-TIME. Those values (ini, end, elapsed)
are stored within OUT.dat which will have the format:


   Build-starts!
   start-time  : 2018-12-15 02:28:43.441220
   end-time    : 2018-12-15 15:49:12.979600
   elapsed-time: 13h20m29.53s

These scripts are used to track the building-time and querying-time of each technique, 
a TIME-LOG.dat file is generated for each technique. Finally, in directory 
"/benchmark/report/summaryTables" we have additional python-scripts that collect all 
those times for each technique, and make up tables showing which experiments where 
performed, and their elapsed times.

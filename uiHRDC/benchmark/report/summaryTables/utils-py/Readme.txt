We want to generate the summary tables that include, for ii-nopos, ii-pos, and self-indexes:

 - The name of the index/self-index type
 - The overall building time
 - The overall query time
 - Wether Locate searches either succeeded or failed for:
    - Wa: low freq words
    - Wb: high freq words
    - P2: Phrases with 2 words
    - P5: Phrases with 5 words
 - Wether Extract queries either succeeded or failed (these are only available for ii-pos and self-indexes)


We prepared a set of python scripts that permit to:

1. For a gnuplot-data file check:
   if it exists:
      - AND it has more than 1 line, and 
      - AND it has a numerical value in the second line (the first line should be a header preceeded by #
      RETURN a string \ok
   else:
      RETURN a string \ko

2. For a Overall-timing file (S.LOG.dat or B.log.dat) check:
   if it exists:
		RETURN the elapset-time value  (elapsed-time: 00h00m06.33s)
   else
        RETURN a string \ko

Then, we have also scripts to create those corresponding latex-tables.


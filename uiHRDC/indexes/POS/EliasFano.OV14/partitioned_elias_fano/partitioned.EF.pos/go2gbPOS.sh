rm -f S-*LOG.dat*
rm -f B-*LOG.dat*

echo "===================================================="
echo "== CREATING 2gb OPT ==============================="
echo "===================================================="

python ../../../../../utils-py/starttime.py  B-OPT-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES
./create_freq_index opt ../../TESTDATA/2gb indexes/2gb.opt --check
./create_wand_data      ../../TESTDATA/2gb indexes/2gb.wand
python ../../../../../utils-py/endtime.py    B-OPT-LOG.dat                  ##LOGS-ELAPSED-TIMES


python ../../../../../utils-py/starttime.py  S-OPT-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES
echo "== Wa =============================================="
./queries opt indexes/2gb.opt  < ../../TESTDATA/2gb.wa.pat 
echo "== Wb =============================================="
./queries opt indexes/2gb.opt  < ../../TESTDATA/2gb.wb.pat 
echo "== P2 =============================================="
./queries opt indexes/2gb.opt  < ../../TESTDATA/2gb.p2.pat 
echo "== P4 =============================================="
./queries opt indexes/2gb.opt  < ../../TESTDATA/2gb.p5.pat 
echo "===================================================="
python ../../../../../utils-py/endtime.py    S-OPT-LOG.dat                  ##LOGS-ELAPSED-TIMES


echo "===================================================="
echo "== CREATING 2gb UNIF =============================="
echo "===================================================="
python ../../../../../utils-py/starttime.py  B-UNIF-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES
./create_freq_index uniform ../../TESTDATA/2gb indexes/2gb.uniform --check
./create_wand_data ../../TESTDATA/2gb indexes/2gb.wand
python ../../../../../utils-py/endtime.py    B-UNIF-LOG.dat                  ##LOGS-ELAPSED-TIMES

python ../../../../../utils-py/starttime.py  S-UNIF-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES
echo "== Wa =============================================="
./queries uniform indexes/2gb.uniform  < ../../TESTDATA/2gb.wa.pat 
echo "== Wb =============================================="
./queries uniform indexes/2gb.uniform  < ../../TESTDATA/2gb.wb.pat 
echo "== P2 =============================================="
./queries uniform indexes/2gb.uniform  < ../../TESTDATA/2gb.p2.pat 
echo "== P4 =============================================="
./queries uniform indexes/2gb.uniform  < ../../TESTDATA/2gb.p5.pat 
echo "===================================================="
python ../../../../../utils-py/endtime.py    S-UNIF-LOG.dat                  ##LOGS-ELAPSED-TIMES




echo "===================================================="
echo "== CREATING 2gb OPTPFOR ==========================="
echo "===================================================="

python ../../../../../utils-py/starttime.py  B-OPTPFOR-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES
./create_freq_index block_optpfor ../../TESTDATA/2gb indexes/2gb.block_optpfor --check
./create_wand_data ../../TESTDATA/2gb indexes/2gb.wand
python ../../../../../utils-py/endtime.py    B-OPTPFOR-LOG.dat                  ##LOGS-ELAPSED-TIMES

python ../../../../../utils-py/starttime.py  S-OPTPFOR-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES
echo "== Wa =============================================="
./queries block_optpfor indexes/2gb.block_optpfor  < ../../TESTDATA/2gb.wa.pat 
echo "== Wb =============================================="
./queries block_optpfor indexes/2gb.block_optpfor  < ../../TESTDATA/2gb.wb.pat 
echo "== P2 =============================================="
./queries block_optpfor indexes/2gb.block_optpfor  < ../../TESTDATA/2gb.p2.pat 
echo "== P4 =============================================="
./queries block_optpfor indexes/2gb.block_optpfor  < ../../TESTDATA/2gb.p5.pat 
echo "===================================================="
python ../../../../../utils-py/endtime.py    S-OPTPFOR-LOG.dat                  ##LOGS-ELAPSED-TIMES



echo "===================================================="
echo "== CREATING 2gb VARINT ==========================="
echo "===================================================="

python ../../../../../utils-py/starttime.py  B-VARINT-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES
./create_freq_index block_varint ../../TESTDATA/2gb indexes/2gb.block_varint --check
./create_wand_data ../../TESTDATA/2gb indexes/2gb.wand
python ../../../../../utils-py/endtime.py    B-VARINT-LOG.dat                  ##LOGS-ELAPSED-TIMES


python ../../../../../utils-py/starttime.py  S-VARINT-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES
echo "== Wa =============================================="
./queries block_varint indexes/2gb.block_varint  < ../../TESTDATA/2gb.wa.pat 
echo "== Wb =============================================="
./queries block_varint indexes/2gb.block_varint  < ../../TESTDATA/2gb.wb.pat 
echo "== P2 =============================================="
./queries block_varint indexes/2gb.block_varint  < ../../TESTDATA/2gb.p2.pat 
echo "== P4 =============================================="
./queries block_varint indexes/2gb.block_varint  < ../../TESTDATA/2gb.p5.pat 
echo "===================================================="
python ../../../../../utils-py/endtime.py    S-VARINT-LOG.dat                  ##LOGS-ELAPSED-TIMES





echo "===================================================="
echo "== CREATING 2gb INTERPOLATIVE ====================="
echo "===================================================="

python ../../../../../utils-py/starttime.py  B-INTERPOLATIVE-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES
./create_freq_index block_interpolative ../../TESTDATA/2gb indexes/2gb.block_interpolative --check
./create_wand_data ../../TESTDATA/2gb indexes/2gb.wand
python ../../../../../utils-py/endtime.py    B-INTERPOLATIVE-LOG.dat                  ##LOGS-ELAPSED-TIMES

python ../../../../../utils-py/starttime.py  S-INTERPOLATIVE-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES
echo "== Wa =============================================="
./queries block_interpolative indexes/2gb.block_interpolative  < ../../TESTDATA/2gb.wa.pat 
echo "== Wb =============================================="
./queries block_interpolative indexes/2gb.block_interpolative  < ../../TESTDATA/2gb.wb.pat 
echo "== P2 =============================================="
./queries block_interpolative indexes/2gb.block_interpolative  < ../../TESTDATA/2gb.p2.pat 
echo "== P4 =============================================="
./queries block_interpolative indexes/2gb.block_interpolative  < ../../TESTDATA/2gb.p5.pat 
echo "===================================================="
python ../../../../../utils-py/endtime.py    S-INTERPOLATIVE-LOG.dat                  ##LOGS-ELAPSED-TIMES


#exit   ##closes  script -a

#echo "Output saved in file  salida.posicionales.queries.txt"
#grep "Time per occ" *
#grep "Time per occ" *   >>salida.posicionales.queries.txt


rm -f S-*LOG.dat*
rm -f B-*LOG.dat*

rm -rf indexes 
mkdir indexes

echo "===================================================="
echo "== CREATING 20gb OPT ==============================="
echo "===================================================="

python ../../../../../utils-py/starttime.py  B-OPT-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES
./create_freq_index opt ../../TESTDATA/20gb indexes/20gb.opt --check
./create_wand_data ../../TESTDATA/20gb indexes/20gb.wand
python ../../../../../utils-py/endtime.py    B-OPT-LOG.dat                  ##LOGS-ELAPSED-TIMES


python ../../../../../utils-py/starttime.py  S-OPT-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES
echo "== Wa =============================================="
./queries opt indexes/20gb.opt  < ../../TESTDATA/wiki20g.wa.ids 
echo "== Wb =============================================="
./queries opt indexes/20gb.opt  < ../../TESTDATA/wiki20g.wb.ids 
echo "== P2 =============================================="
./queries opt indexes/20gb.opt  < ../../TESTDATA/wiki20g.p2.ids 
echo "== P4 =============================================="
./queries opt indexes/20gb.opt  < ../../TESTDATA/wiki20g.p5.ids 
echo "===================================================="
python ../../../../../utils-py/endtime.py    S-OPT-LOG.dat                  ##LOGS-ELAPSED-TIMES



echo "===================================================="
echo "== CREATING 20gb UNIF =============================="
echo "===================================================="

python ../../../../../utils-py/starttime.py  B-UNIF-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES
./create_freq_index uniform ../../TESTDATA/20gb indexes/20gb.uniform --check
./create_wand_data ../../TESTDATA/20gb indexes/20gb.wand
python ../../../../../utils-py/endtime.py    B-UNIF-LOG.dat                  ##LOGS-ELAPSED-TIMES

python ../../../../../utils-py/starttime.py  S-UNIF-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES
echo "== Wa =============================================="
./queries uniform indexes/20gb.uniform  < ../../TESTDATA/wiki20g.wa.ids 
echo "== Wb =============================================="
./queries uniform indexes/20gb.uniform  < ../../TESTDATA/wiki20g.wb.ids 
echo "== P2 =============================================="
./queries uniform indexes/20gb.uniform  < ../../TESTDATA/wiki20g.p2.ids 
echo "== P4 =============================================="
./queries uniform indexes/20gb.uniform  < ../../TESTDATA/wiki20g.p5.ids 
echo "===================================================="
python ../../../../../utils-py/endtime.py    S-UNIF-LOG.dat                  ##LOGS-ELAPSED-TIMES




echo "===================================================="
echo "== CREATING 20gb OPTPFOR ==========================="
echo "===================================================="

python ../../../../../utils-py/starttime.py  B-OPTPFOR-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES
./create_freq_index block_optpfor ../../TESTDATA/20gb indexes/20gb.block_optpfor --check
./create_wand_data ../../TESTDATA/20gb indexes/20gb.wand
python ../../../../../utils-py/endtime.py    B-OPTPFOR-LOG.dat                  ##LOGS-ELAPSED-TIMES

python ../../../../../utils-py/starttime.py  S-OPTPFOR-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES
echo "== Wa =============================================="
./queries block_optpfor indexes/20gb.block_optpfor  < ../../TESTDATA/wiki20g.wa.ids 
echo "== Wb =============================================="
./queries block_optpfor indexes/20gb.block_optpfor  < ../../TESTDATA/wiki20g.wb.ids 
echo "== P2 =============================================="
./queries block_optpfor indexes/20gb.block_optpfor  < ../../TESTDATA/wiki20g.p2.ids 
echo "== P4 =============================================="
./queries block_optpfor indexes/20gb.block_optpfor  < ../../TESTDATA/wiki20g.p5.ids 
echo "===================================================="
python ../../../../../utils-py/endtime.py    S-OPTPFOR-LOG.dat                  ##LOGS-ELAPSED-TIMES




echo "===================================================="
echo "== CREATING 20gb VARINT ==========================="
echo "===================================================="

python ../../../../../utils-py/starttime.py  B-VARINT-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES
./create_freq_index block_varint ../../TESTDATA/20gb indexes/20gb.block_varint --check
./create_wand_data ../../TESTDATA/20gb indexes/20gb.wand
python ../../../../../utils-py/endtime.py    B-VARINT-LOG.dat                  ##LOGS-ELAPSED-TIMES


python ../../../../../utils-py/starttime.py  S-VARINT-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES
echo "== Wa =============================================="
./queries block_varint indexes/20gb.block_varint  < ../../TESTDATA/wiki20g.wa.ids 
echo "== Wb =============================================="
./queries block_varint indexes/20gb.block_varint  < ../../TESTDATA/wiki20g.wb.ids 
echo "== P2 =============================================="
./queries block_varint indexes/20gb.block_varint  < ../../TESTDATA/wiki20g.p2.ids 
echo "== P4 =============================================="
./queries block_varint indexes/20gb.block_varint  < ../../TESTDATA/wiki20g.p5.ids 
echo "===================================================="
python ../../../../../utils-py/endtime.py    S-VARINT-LOG.dat                  ##LOGS-ELAPSED-TIMES






echo "===================================================="
echo "== CREATING 20gb INTERPOLATIVE ====================="
echo "===================================================="

python ../../../../../utils-py/starttime.py  B-INTERPOLATIVE-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES
./create_freq_index block_interpolative ../../TESTDATA/20gb indexes/20gb.block_interpolative --check
./create_wand_data ../../TESTDATA/20gb indexes/20gb.wand
python ../../../../../utils-py/endtime.py    B-INTERPOLATIVE-LOG.dat                  ##LOGS-ELAPSED-TIMES

python ../../../../../utils-py/starttime.py  S-INTERPOLATIVE-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES
echo "== Wa =============================================="
./queries block_interpolative indexes/20gb.block_interpolative  < ../../TESTDATA/wiki20g.wa.ids 
echo "== Wb =============================================="
./queries block_interpolative indexes/20gb.block_interpolative  < ../../TESTDATA/wiki20g.wb.ids 
echo "== P2 =============================================="
./queries block_interpolative indexes/20gb.block_interpolative  < ../../TESTDATA/wiki20g.p2.ids 
echo "== P4 =============================================="
./queries block_interpolative indexes/20gb.block_interpolative  < ../../TESTDATA/wiki20g.p5.ids 
echo "===================================================="
python ../../../../../utils-py/endtime.py    S-INTERPOLATIVE-LOG.dat                  ##LOGS-ELAPSED-TIMES


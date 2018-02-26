

echo "===================================================="
echo "== CREATING 2gb OPT ==============================="
echo "===================================================="
./create_freq_index opt ../../TESTDATA/2gb indexes/2gb.opt --check
./create_wand_data      ../../TESTDATA/2gb indexes/2gb.wand

echo "== Wa =============================================="
./queries opt indexes/2gb.opt  < ../../TESTDATA/2gb.wa.pat 
echo "== Wb =============================================="
./queries opt indexes/2gb.opt  < ../../TESTDATA/2gb.wb.pat 
echo "== P2 =============================================="
./queries opt indexes/2gb.opt  < ../../TESTDATA/2gb.p2.pat 
echo "== P4 =============================================="
./queries opt indexes/2gb.opt  < ../../TESTDATA/2gb.p5.pat 
echo "===================================================="



echo "===================================================="
echo "== CREATING 2gb UNIF =============================="
echo "===================================================="
./create_freq_index uniform ../../TESTDATA/2gb indexes/2gb.uniform --check
./create_wand_data ../../TESTDATA/2gb indexes/2gb.wand

echo "== Wa =============================================="
./queries uniform indexes/2gb.uniform  < ../../TESTDATA/2gb.wa.pat 
echo "== Wb =============================================="
./queries uniform indexes/2gb.uniform  < ../../TESTDATA/2gb.wb.pat 
echo "== P2 =============================================="
./queries uniform indexes/2gb.uniform  < ../../TESTDATA/2gb.p2.pat 
echo "== P4 =============================================="
./queries uniform indexes/2gb.uniform  < ../../TESTDATA/2gb.p5.pat 
echo "===================================================="




echo "===================================================="
echo "== CREATING 2gb OPTPFOR ==========================="
echo "===================================================="
./create_freq_index block_optpfor ../../TESTDATA/2gb indexes/2gb.block_optpfor --check
./create_wand_data ../../TESTDATA/2gb indexes/2gb.wand

echo "== Wa =============================================="
./queries block_optpfor indexes/2gb.block_optpfor  < ../../TESTDATA/2gb.wa.pat 
echo "== Wb =============================================="
./queries block_optpfor indexes/2gb.block_optpfor  < ../../TESTDATA/2gb.wb.pat 
echo "== P2 =============================================="
./queries block_optpfor indexes/2gb.block_optpfor  < ../../TESTDATA/2gb.p2.pat 
echo "== P4 =============================================="
./queries block_optpfor indexes/2gb.block_optpfor  < ../../TESTDATA/2gb.p5.pat 
echo "===================================================="




echo "===================================================="
echo "== CREATING 2gb VARINT ==========================="
echo "===================================================="
./create_freq_index block_varint ../../TESTDATA/2gb indexes/2gb.block_varint --check
./create_wand_data ../../TESTDATA/2gb indexes/2gb.wand

echo "== Wa =============================================="
./queries block_varint indexes/2gb.block_varint  < ../../TESTDATA/2gb.wa.pat 
echo "== Wb =============================================="
./queries block_varint indexes/2gb.block_varint  < ../../TESTDATA/2gb.wb.pat 
echo "== P2 =============================================="
./queries block_varint indexes/2gb.block_varint  < ../../TESTDATA/2gb.p2.pat 
echo "== P4 =============================================="
./queries block_varint indexes/2gb.block_varint  < ../../TESTDATA/2gb.p5.pat 
echo "===================================================="






echo "===================================================="
echo "== CREATING 2gb INTERPOLATIVE ====================="
echo "===================================================="
./create_freq_index block_interpolative ../../TESTDATA/2gb indexes/2gb.block_interpolative --check
./create_wand_data ../../TESTDATA/2gb indexes/2gb.wand

echo "== Wa =============================================="
./queries block_interpolative indexes/2gb.block_interpolative  < ../../TESTDATA/2gb.wa.pat 
echo "== Wb =============================================="
./queries block_interpolative indexes/2gb.block_interpolative  < ../../TESTDATA/2gb.wb.pat 
echo "== P2 =============================================="
./queries block_interpolative indexes/2gb.block_interpolative  < ../../TESTDATA/2gb.p2.pat 
echo "== P4 =============================================="
./queries block_interpolative indexes/2gb.block_interpolative  < ../../TESTDATA/2gb.p5.pat 
echo "===================================================="


exit   ##closes  script -a

#echo "Output saved in file  salida.posicionales.queries.txt"
#grep "Time per occ" *
#grep "Time per occ" *   >>salida.posicionales.queries.txt


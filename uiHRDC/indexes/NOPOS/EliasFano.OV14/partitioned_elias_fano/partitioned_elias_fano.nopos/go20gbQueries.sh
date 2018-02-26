rm -rf indexes 
mkdir indexes

echo "===================================================="
echo "== CREATING 20gb OPT ==============================="
echo "===================================================="
./create_freq_index opt ../../TESTDATA/20gb indexes/20gb.opt --check
./create_wand_data ../../TESTDATA/20gb indexes/20gb.wand

echo "== Wa =============================================="
./queries opt indexes/20gb.opt  < ../../TESTDATA/wiki20g.wa.ids 
echo "== Wb =============================================="
./queries opt indexes/20gb.opt  < ../../TESTDATA/wiki20g.wb.ids 
echo "== P2 =============================================="
./queries opt indexes/20gb.opt  < ../../TESTDATA/wiki20g.p2.ids 
echo "== P4 =============================================="
./queries opt indexes/20gb.opt  < ../../TESTDATA/wiki20g.p5.ids 
echo "===================================================="



echo "===================================================="
echo "== CREATING 20gb UNIF =============================="
echo "===================================================="
./create_freq_index uniform ../../TESTDATA/20gb indexes/20gb.uniform --check
./create_wand_data ../../TESTDATA/20gb indexes/20gb.wand

echo "== Wa =============================================="
./queries uniform indexes/20gb.uniform  < ../../TESTDATA/wiki20g.wa.ids 
echo "== Wb =============================================="
./queries uniform indexes/20gb.uniform  < ../../TESTDATA/wiki20g.wb.ids 
echo "== P2 =============================================="
./queries uniform indexes/20gb.uniform  < ../../TESTDATA/wiki20g.p2.ids 
echo "== P4 =============================================="
./queries uniform indexes/20gb.uniform  < ../../TESTDATA/wiki20g.p5.ids 
echo "===================================================="




echo "===================================================="
echo "== CREATING 20gb OPTPFOR ==========================="
echo "===================================================="
./create_freq_index block_optpfor ../../TESTDATA/20gb indexes/20gb.block_optpfor --check
./create_wand_data ../../TESTDATA/20gb indexes/20gb.wand

echo "== Wa =============================================="
./queries block_optpfor indexes/20gb.block_optpfor  < ../../TESTDATA/wiki20g.wa.ids 
echo "== Wb =============================================="
./queries block_optpfor indexes/20gb.block_optpfor  < ../../TESTDATA/wiki20g.wb.ids 
echo "== P2 =============================================="
./queries block_optpfor indexes/20gb.block_optpfor  < ../../TESTDATA/wiki20g.p2.ids 
echo "== P4 =============================================="
./queries block_optpfor indexes/20gb.block_optpfor  < ../../TESTDATA/wiki20g.p5.ids 
echo "===================================================="




echo "===================================================="
echo "== CREATING 20gb VARINT ==========================="
echo "===================================================="
./create_freq_index block_varint ../../TESTDATA/20gb indexes/20gb.block_varint --check
./create_wand_data ../../TESTDATA/20gb indexes/20gb.wand

echo "== Wa =============================================="
./queries block_varint indexes/20gb.block_varint  < ../../TESTDATA/wiki20g.wa.ids 
echo "== Wb =============================================="
./queries block_varint indexes/20gb.block_varint  < ../../TESTDATA/wiki20g.wb.ids 
echo "== P2 =============================================="
./queries block_varint indexes/20gb.block_varint  < ../../TESTDATA/wiki20g.p2.ids 
echo "== P4 =============================================="
./queries block_varint indexes/20gb.block_varint  < ../../TESTDATA/wiki20g.p5.ids 
echo "===================================================="






echo "===================================================="
echo "== CREATING 20gb INTERPOLATIVE ====================="
echo "===================================================="
./create_freq_index block_interpolative ../../TESTDATA/20gb indexes/20gb.block_interpolative --check
./create_wand_data ../../TESTDATA/20gb indexes/20gb.wand

echo "== Wa =============================================="
./queries block_interpolative indexes/20gb.block_interpolative  < ../../TESTDATA/wiki20g.wa.ids 
echo "== Wb =============================================="
./queries block_interpolative indexes/20gb.block_interpolative  < ../../TESTDATA/wiki20g.wb.ids 
echo "== P2 =============================================="
./queries block_interpolative indexes/20gb.block_interpolative  < ../../TESTDATA/wiki20g.p2.ids 
echo "== P4 =============================================="
./queries block_interpolative indexes/20gb.block_interpolative  < ../../TESTDATA/wiki20g.p5.ids 
echo "===================================================="


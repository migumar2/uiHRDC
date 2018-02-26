

cd 1.4.scripts.rice.bitmap                 ; sh clean.sh ; rm -f posting*; rm -f *.dat; rm -f indexes/*; rm -f *.txt; cd ..
cd 1.4.scripts.rice.bitmap                 ; sh goBuild.sh ; cd ..
cd 1.4.scripts.rice.bitmap                 ; sh goSearch.sh ; cd ..
cd 1.4.scripts.rice.bitmap                 ; sh clean.sh ; rm -f posting*; rm -f indexes/*;  cd ..

cd 2.1.scripts.vbyte.TestExtract           ; sh clean.sh ; rm -f posting*; rm -f *.dat; rm -f indexes/*; rm -f *.txt; cd ..
cd 2.1.scripts.vbyte.TestExtract           ; sh goBuild.sh ; cd ..
cd 2.1.scripts.vbyte.TestExtract           ; sh goSearch.sh ; sh goExtractIntervals.sh; cd ..
cd 2.1.scripts.vbyte.TestExtract           ; sh clean.sh ; rm -f posting*; rm -f indexes/*; cd ..

cd 2.scripts.vbytebitmapmoffat             ; sh clean.sh ; rm -f posting*; rm -f *.dat; rm -f indexes/*; rm -f *.txt; cd ..
cd 2.scripts.vbytebitmapmoffat             ; sh goBuild.sh ; cd ..
cd 2.scripts.vbytebitmapmoffat             ; sh goSearch.sh ; cd ..
cd 2.scripts.vbytebitmapmoffat             ; sh clean.sh ; rm -f posting*; rm -f indexes/*;  cd ..

cd 3.scripts.vbytebitmapSanders            ; sh clean.sh ; rm -f posting*; rm -f *.dat; rm -f indexes/*; rm -f *.txt; cd ..
cd 3.scripts.vbytebitmapSanders            ; sh goBuild.sh ; cd ..
cd 3.scripts.vbytebitmapSanders            ; sh goSearch.sh ; cd ..
cd 3.scripts.vbytebitmapSanders            ; sh clean.sh ; rm -f posting*; rm -f indexes/*;  cd ..

#cd 4.2.scripts.Repair                     ; sh clean.sh ; rm -f posting*; rm -f *.dat; rm -f indexes/*; rm -f *.txt; cd ..
#cd 4.2.scripts.Repair                     ; sh goBuild.sh ; cd ..
#cd 4.2.scripts.Repair                     ; sh goSearch.sh ; cd ..
#cd 4.3.scripts.Repair.skipping            ; sh clean.sh ; rm -f posting*; rm -f *.dat; rm -f indexes/*; rm -f *.txt; cd ..
#cd 4.3.scripts.Repair.skipping            ; sh goBuild.sh ; cd ..
#cd 4.3.scripts.Repair.skipping            ; sh goSearch.sh ; cd ..

cd 4.22.scripts.RepairGonzalo              ; sh clean.sh ; rm -f posting*; rm -f *.dat; rm -f indexes/*; rm -f *.txt; cd ..
cd 4.22.scripts.RepairGonzalo              ; sh goBuild.sh ; cd ..
cd 4.22.scripts.RepairGonzalo              ; sh goSearch.sh ; cd ..
cd 4.22.scripts.RepairGonzalo              ; sh clean.sh ; rm -f posting*; rm -f indexes/*;  cd ..

cd 4.33.scripts.Repair.skipping.Gonzalo    ; sh clean.sh ; rm -f posting*; rm -f *.dat; rm -f indexes/*; rm -f *.txt; cd ..
cd 4.33.scripts.Repair.skipping.Gonzalo    ; sh goBuild.sh ; cd ..
cd 4.33.scripts.Repair.skipping.Gonzalo    ; sh goSearch.sh ; cd ..
cd 4.33.scripts.Repair.skipping.Gonzalo    ; sh clean.sh ; rm -f posting*; rm -f indexes/*;  cd ..

cd 4.33.1.scripts.RepairSkipping.Moffat    ; sh clean.sh ; rm -f posting*; rm -f *.dat; rm -f indexes/*; rm -f *.txt; cd ..
cd 4.33.1.scripts.RepairSkipping.Moffat    ; sh goBuild.sh ; cd ..
cd 4.33.1.scripts.RepairSkipping.Moffat    ; sh goSearch.sh ; cd ..
cd 4.33.1.scripts.RepairSkipping.Moffat    ; sh clean.sh ; rm -f posting*; rm -f indexes/*;  cd ..

cd 4.33.2.scripts.RepairSkipping.Sanders   ; sh clean.sh ; rm -f posting*; rm -f *.dat; rm -f indexes/*; rm -f *.txt; cd ..
cd 4.33.2.scripts.RepairSkipping.Sanders   ; sh goBuild.sh ; cd ..
cd 4.33.2.scripts.RepairSkipping.Sanders   ; sh goSearch.sh ; cd ..
cd 4.33.2.scripts.RepairSkipping.Sanders   ; sh clean.sh ; rm -f posting*; rm -f indexes/*;  cd ..

cd 6.scripts.vbytelzma                     ; sh clean.sh ; rm -f posting* vbyte.lzma.* ; rm -f *.dat; rm -f indexes/*; rm -f *.txt; cd ..
cd 6.scripts.vbytelzma                     ; sh goBuild.sh ; cd ..
cd 6.scripts.vbytelzma                     ; sh goSearch.sh ; cd ..
cd 6.scripts.vbytelzma                     ; sh clean.sh ; rm -f posting*; rm -f indexes/*;  cd ..

cd 7.1.scripts.simple9e                    ; sh clean.sh ; rm -f posting*; rm -f *.dat; rm -f indexes/*; rm -f *.txt; cd ..
cd 7.1.scripts.simple9e                    ; sh goBuild.sh ; cd ..
cd 7.1.scripts.simple9e                    ; sh goSearch.sh ; cd ..
cd 7.1.scripts.simple9e                    ; sh clean.sh ; rm -f posting*; rm -f indexes/*;  cd ..

cd 9.scripts.qmx                           ; sh clean.sh ; rm -f posting*; rm -f *.dat; rm -f indexes/*; rm -f *.txt; cd ..
cd 9.scripts.qmx                           ; sh goBuild.sh ; cd ..
cd 9.scripts.qmx                           ; sh goSearch.sh ; cd ..
cd 9.scripts.qmx                           ; sh clean.sh ; rm -f posting*; rm -f indexes/*;  cd ..

cd 9.scripts.qmx.bis                       ; sh clean.sh ; rm -f posting*; rm -f *.dat; rm -f indexes/*; rm -f *.txt; cd ..
cd 9.scripts.qmx.bis                       ; sh goBuild.sh ; cd ..
cd 9.scripts.qmx.bis                       ; sh goSearch.sh ; cd ..
cd 9.scripts.qmx.bis                       ; sh clean.sh ; rm -f posting*; rm -f indexes/*;  cd ..

#---------- For EliasFano Ottaviano&Venturini - sigir 2014. comparison -------
cd 2.1.aaa.EliasFano.PartialMeasurement    ; sh clean.sh ; rm -f posting*; rm -f *.dat; rm -f indexes/*; rm -f *.txt; cd ..
cd 2.1.aaa.EliasFano.PartialMeasurement    ; sh goBuild.sh ; cd ..
cd 2.1.aaa.EliasFano.PartialMeasurement    ; sh goSearch.sh ; cd ..
cd 2.1.aaa.EliasFano.PartialMeasurement    ; sh clean.sh ; rm -f posting*; rm -f indexes/*;  cd ..


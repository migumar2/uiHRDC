#cd extract/length80 ; gnuplot locate.gp  ; cd ../..
#cd extract/length13000 ; gnuplot locate.gp  ; cd ../..
#cd words1-1000    ; gnuplot locate.gp  ; cd ..
#cd words1001-100k ; gnuplot locate.gp  ; cd ..
#cd phrases2_2     ; gnuplot locate.gp  ; cd ..
#cd phrases5_5     ; gnuplot locate.gp  ; cd ..


cd words1-1000   ; bash getFiles.sh; sed s/2087767849/$SSIZEPOS/ locate.gp > locate2.gp; gnuplot locate2.gp  ; cd ..
cd words1001-100k; bash getFiles.sh; sed s/2087767849/$SSIZEPOS/ locate.gp > locate2.gp; gnuplot locate2.gp  ; cd ..
cd phrases2-2    ; bash getFiles.sh; sed s/2087767849/$SSIZEPOS/ locate.gp > locate2.gp; gnuplot locate2.gp  ; cd ..
cd phrases5-5    ; bash getFiles.sh; sed s/2087767849/$SSIZEPOS/ locate.gp > locate2.gp; gnuplot locate2.gp  ; cd ..

reset
rm -f output.posicionales.queries.txt
script -a output.posicionales.queries.txt

sh go2gbPOS.sh

exit   ##sale de script -a
echo "output saved in file output.posicionales.queries.txt"


grep "Time per occ" output.posicionales.queries.txt 

echo " " > salida_grep
echo " " >> salida_grep
echo "==================================================" >> salida_grep
echo "==================================================" >> salida_grep
grep "Time per occ" output.posicionales.queries.txt    >>salida_grep

cat salida_grep >> output.posicionales.queries.grep.txt
rm -f salida_grep

cp output.posicionales.* ..



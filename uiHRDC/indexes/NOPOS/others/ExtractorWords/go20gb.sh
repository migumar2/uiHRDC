echo "----------------------------------------------"
echo "Generating random words"
echo "----------------------------------------------"
echo "Obtaining words from a given len and frecuency"
echo "----------------------------------------------"
cd 1.Obtain_words_vocab_filtered
sh go20gb
cd ..

echo "----------------------------------------------"
echo "Generating randomized patterns.               "
echo "----------------------------------------------"
cd 3.GenerateRandomWords
sh go20gb
cd ..
rm *.xt
mv wiki20g.* ..
echo "----------------------------------------------"
echo "finished see parent directory!"
echo "----------------------------------------------"



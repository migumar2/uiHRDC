ln -s ../data/textos/ texts


cd ExtractorWords
	sh go20gb.sh
	cd ..

cd extractor.phrases.randLengh
	sh go20gb.sh
	cd ..

rm texts

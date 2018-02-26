all:  pdf 

pdf:
	latex paper.tex
	bibtex paper
	latex paper.tex
	latex paper.tex
	dvips paper.dvi
	ps2pdf paper.ps
	#dvipdfm paper.dvi 
	rm -f paper.blg
	rm -f paper.aux
	rm -f paper.bbl
	rm -f paper.log
	rm -f paper.dvi
	rm -f paper.ps
	rm -f paper.spl

diff: 
	latexdiff paper_last.tex paper.tex > paper_diff.tex
	latex paper_diff.tex
	latex paper_diff.tex
	bibtex paper_diff
	latex paper_diff.tex
	latex paper_diff.tex
	dvips paper_diff.dvi
	ps2pdf paper_diff.ps
	rm -f paper_diff.blg
	rm -f paper_diff.aux
	rm -f paper_diff.bbl
	rm -f paper_diff.log	
	rm -f paper_diff.ps
	
clean:
	#dos2unix *.tex
	rm -f paper.dvi
	rm -f paper.ps
	rm -f paper.pdf
	rm -f paper_diff*	

##images:
##	cd figures && pdftops -eps *.pdf && cd ..	

tar:
	rm -f jasist.tar.gz
	tar czvf is.tar.gz images \
			 paper.tex paper.bib makefile *.cls *.txt


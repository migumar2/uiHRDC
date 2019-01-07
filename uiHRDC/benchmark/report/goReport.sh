#NON-POSITIONAL-DATA ----------

  #SET THIS FOR THE 20GB COLLECTION.
  export SSIZE=26597454863
  #SET THIS FOR THE 200MB collection
  #export SSIZE=260000000

#POSITIONAL-DATA ----------

  #SET THIS FOR THE 2GB COLLECTION.
  export SSIZEPOS=2087767849
  #SET THIS FOR THE EINSTEIN collection
  #export SSIZEPOS=467626544



#sed s/26597454863/$SSIZE/ locate.gp
#sed s/2087767849/$SSIZEPOS/ locate.gp
echo $SSIZE
echo $SSIZEPOS

cd figures; bash goCreateFigures.sh; cd ..

cd summaryTables; bash goGenerateTables.sh; cd ..



rm -f report.pdf
cd src;  make; mv paper.pdf ../report.pdf; make clean; cd ..




unset SSIZE

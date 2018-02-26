FILES=( aprev28 cere coreutils dna.001.1 einstein.de.txt einstein.en.txt english.001.2 Escherichia_Coli fib41 influenza kernel para proteins.001.1 rs.13 sources.001.2 tm29 world_leaders )
NAMES=( "A_{28}" "Cere" "Coreutils" "DNA.001^{1}" "Einstein.de" "Einstein.en" "English.001^{2}" "Escherichia Coli" "F_{41}" "Influenza" "Kernel" "Para" "Proteins.001^{1}" "R_{13}" "Sources.001^{2}" "T_{29}" "World Leaders" )
SCRIPT=$(readlink -f $0)
# Absolute path this script is in. /home/user/bin
SCRIPTPATH=`dirname $SCRIPT`
GNUPLOT=$SCRIPTPATH/gnuplot
mkdir -p gnuplot
for i in {1..17}
do
    echo ${FILES[$i]};
    echo ${NAMES[$i]};
done

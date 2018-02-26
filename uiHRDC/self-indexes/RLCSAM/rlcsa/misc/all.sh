FILES="aprev28 cere coreutils dna.001.1 einstein.de.txt einstein.en.txt english.001.2 Escherichia_Coli fib41 influenza kernel para proteins.001.1 rs.13 sources.001.2 tm29 world_leaders"
SCRIPT=$(readlink -f $0)
# Absolute path this script is in. /home/user/bin
SCRIPTPATH=`dirname $SCRIPT`
for f in $FILES; do
    $SCRIPTPATH/build.sh $f;
    $SCRIPTPATH/create_query.sh $f;
    $SCRIPTPATH/process_build.sh $f;
    $SCRIPTPATH/extract.sh $f;
    $SCRIPTPATH/process_extract.sh $f;
    $SCRIPTPATH/exist.sh $f;
    $SCRIPTPATH/process_exist.sh $f;
    $SCRIPTPATH/locate.sh $f;
    $SCRIPTPATH/process_locate.sh $f;
    $SCRIPTPATH/locate_small.sh $f;
    $SCRIPTPATH/process_locate_small.sh $f;
done
    


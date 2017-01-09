#!/bin/bash
# Frontend for the dec_win ("decoder wave-in") program

if [[ $# -lt 2 || $# -gt 2 ]]
then
 echo "This script must have no more or less than 2 arguments:"
 echo "[input file] [output file]"
 echo " "
 echo "By the way, the input file must be mono raw unsigned 8 bit PCM"
 echo "(or mono raw signed 16 bit PCM, if converts16% is set to TRUE in dec_win)"
 exit
fi
sleep 0.05s

if [[ -e "$2" ]]
then
 echo "File exists. Exiting"
 exit
fi

echo "Starting"

if [[ -e /tmp/readdata ]]
then
 rm /tmp/readdata
fi
if [[ -e miscfiles/record.raw ]]
then
 let hold=1
 export holdfile=$RANDOM
 mv miscfiles/record.raw miscfiles/hold_$holdfile
fi
if [[ -e miscfiles/recovered_file ]]
then
 let holda=1
 export holdfilea=$RANDOM
 mv miscfiles/recovered_file miscfiles/holda_$holdfilea
fi

mkfifo /tmp/readdata
cat "$1" > /tmp/readdata &
 
brandy -quit dec_win  
#brandy dec_win
mv miscfiles/recovered_file "$2" 
rm /tmp/readdata


if [[ hold -eq 1 ]]
then
 mv miscfiles/hold_$holdfile miscfiles/record.raw
fi
if [[ holda -eq 1 ]]
then
 mv miscfiles/holda_$holdfilea miscfiles/recovered_file
fi

echo "Ending"
# DBZ FOREVER!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

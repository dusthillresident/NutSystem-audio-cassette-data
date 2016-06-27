#!/bin/bash
# This script is made for use with the 'decwin.c' program. Compile it with 'gcc decwin.c -o decwin', because this script expects the executable file to be named 'decwin'.
if [[ -e /tmp/readdata ]]
then
 rm /tmp/readdata
fi
############### temporarily move existing files out of the way ############
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
###########################################################################

mkfifo /tmp/readdata


# The arecord command, used to record the sound in from the audio input device, for the dec_win ("decoder wave-in") program to read and decode, goes here.
# This command will need different options for different sound cards, devices, setups, etc. I've included two examples here.
# This one only takes the sound from the left audio channel (the audio from the right channel is discarded), which is useful for mono tape recorders. 
# If you want to use this script, you'll probably need to work out what specific options you need to give to arecord for your particular setup.

arecord -I -r 44100 -t raw -f U8 -c 2 /tmp/readdata /dev/null &
# arecord -D hw:1 -c 1 -r 44100 -t raw -f S16_LE > /tmp/readin &

# This one is suitable for use with cheap & common USB-audio adapters. The ones I have only support recording 16bit little endian mono. The dec_win program normally can only use 8bit audio. In order to use this arecord command, you will need to edit the dec_win file and set the variable 'converts16%' to 'TRUE'
#arecord -D hw:1 -r 44100 -t raw -f S16_LE > /tmp/readdata &
# Note that the data is written into the FIFO using a > redirection operator thing. This is essential because for some reason, if you tell arecord to write to the file directly, it insists on turning the FIFO into a normal file, which messes everything up (the dec_win program crashes, complaining about reaching the end of the file). For some reason, when you tell arecord to use separate files for the separate audio channels (left/right/etc), it doesn't mess up the FIFO and turn it into a normal file. It only does it when it's writing to a single file. I don't know why this happens. 



./decwin
killall arecord
rm /tmp/readdata
mv miscfiles/recovered_file _recovered_files/recovered_file_`date +%a%d%b%Y-%H\-%M-%S`
if [[ -e miscfiles/record.raw ]]
then
 mv miscfiles/record.raw _waveinput_recordings/record.raw_`date +%a%d%b%Y-%H\-%M-%S`
fi


####################### restore old files #################################
if [[ hold -eq 1 ]]
then
 mv miscfiles/hold_$holdfile miscfiles/record.raw
fi
if [[ holda -eq 1 ]]
then
 mv miscfiles/holda_$holdfilea miscfiles/recovered_file
fi

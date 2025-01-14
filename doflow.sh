#/bin/bash

# This script is used to generate a list of functions by file by function use
#
# Get  a list of unique functions in file function order
grep -E '^[a-zA-Z0-9\-]{1,}' *.c > .temp1 

# Isolate the functions called, stripping off the return and arguments
sed -n -E 's/^([a-zA-Z\-]{1,})\.c:(.*)$/\2 \1\.c/p' .temp1 > .temp2 #flip order to function file
sed -n -E 's/^(.*)\((.*)/\1/p' .temp2 > .temp3   # isolate function
sed -n -E 's/^(.*) ([a-zA-Z0-9_]{1,})$/\2/p' .temp3 > .temp4

# Get rid of dups
sort .temp4 > .temp5
old=""
touch .temp6
while read -r line; do
	if [ "$line" != "$old" ]; then
		echo $line >> .temp5
		old=$line
	fi	
done < .temp5	

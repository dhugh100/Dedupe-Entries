# program to format
# Passed a program?
if [ -z "$1" ]; then
	echo "No program"
	exit 1
fi

# Program exists
if [ ! -f "$1" ]; then
	echo "Not a regular file"
	exit 1
fi

# Program is a file
indent -l 120 -linux $1

# insert a space between a function name and opening (
 sed -i -E 's/^(int |void ).*[a-zA-Z_]+(/1/' *.c



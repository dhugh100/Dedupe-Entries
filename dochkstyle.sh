# insert a space between a cast and a variable
sed -i -E 's/\)([a-Z0-9]{1})/\) \1/' *.c
sed -i -E 's/\)([a-Z0-9]{1})/\) \1/' *.h

# remove a space between pointer operater and variable
sed -i -E 's/\* ([a-Z0-9])/\*\1/' *.c
sed -i -E 's/\* ([a-Z0-9])/\*\1/' *.h

# insert a space between a cast and a variable
sed -i -E 's/([a-Z0-9])\(/\1 \(/g' *.c
sed -i -E 's/([a-Z0-9])\(/\1 \(/g' *.h

# insert a space between a comma and the next alphanumeric
sed -i -E 's/,([a-Z0-9])/, \1/g' *.c
sed -i -E 's/,([a-Z0-9])/, \1/g' *.h

# replace spaces at the start of a line with a tab
sed -i -E 's/^ {2,}([a-Z0-9_*{}/]{1})/\t\1/' *.c
sed -i -E 's/^ {2,}([a-Z0-9_/*{}]{1})/\t\1/' *.h

# replace spaces after a tab, alphanumeric with a tab
sed -i -E 's/^\t([a-Z0-9_/*{}]{1,}) {2,}/\t\1\t/' *.c
sed -i -E 's/^\t([a-Z0-9_/*{}]{1,}) {2,}/\t\1\t/' *.h

# replace spaces after a tab, alphanumeric, tab, alphanumberic
sed -i -E 's/^\t([a-Z0-9_/*{}]{1,})\t([a-Z0-9_/*{}]{1,}) {2,}/\t\1\t\2\t/' *.c
sed -i -E 's/^\t([a-Z0-9_/*{}]{1,})\t([a-Z0-9_/*{}]{1,}) {2,}/\t\1\t\2\t/' *.h

# Make first character following // uppercase
sed -i -E 's/(.*\/\/) {1,}([a-z]{1,})/\1 \u\2/' *.c

# Make one space before and after =
sed -i -E 's/ {2,}=/ =/g' *.c
sed -i -E 's/ {2,}=/ =/g' *.h
sed -i -E 's/= {2,}/= /g' *.c
sed -i -E 's/= {2,}/= /g' *.h

# Get rid of space after open parens
sed -i -E 's/\( ([a-Z0-9_(!&|])/\(\1/g' *.c
sed -i -E 's/\( ([a-Z0-9_(!&|])/\(\1/g' *.h

# Get rid of space before closed parens
sed -i -E 's/([a-Z0-9_)!&|]) \)/\1\)/g' *.c
sed -i -E 's/([a-Z0-9_)!&|]) \)/\1\)/g' *.h

# Check main.h for use
#
declare -a types=(
[0]=char
[1]=int
[2]=float
[3]=double
[4]=void
[5]=GtkWidget
[6]=gboolean
[7]=GtkEntryBuffer
[8]=GtkApplication
[9]=GListStore
[10]=const
[11]=guint
[12]=GtkMultiSelecion
[13]=GtkBitSet
[14]=GtkCustomFilter
[15]=filter_entry
[16]=search_entry
[17]=uint32_t
)

# Check for defines
while read var1 var2 var3 var4; do
	if [ "$var1" = "#define" ]; then
		echo "** Checking for $var2"
        	grep $var2 ~/dd/*.c > seeit
		cnt=`wc -l seeit`
		echo "$var2 has $cnt in .c files"
        	grep $var2 ~/dd/*.h > seeit
		cnt=`wc -l seeit`
		echo "$var2 has $cnt in .h files"
	fi
done < ~/dd/main.h

# Check for C types 
while read var1 var2 var3 var4; do
	for i in ${types[@]}; do
		if [ "$var1" = "$i" ]; then
			# Check for const and adjust if found
			if [ "$var1" = "const" ]; then
				var1=$var2
				var2=$var3
			fi 
			# Need to see if should remove the leading *
			if [[ ${var2:0:1} == "*" ]]; then
				var2="${var2:1}" 
			fi	
			# Remove trailing ;
			if [[ ${var2: -1} == ";" ]]; then
				var2="${var2:0:-1}" 
			fi	
			
			echo "Checking $var2"
			grep $var2 ~/dd/*.c > seeit
			cnt=`wc -l seeit`
			echo "$var2 has $cnt in .c files"
		fi
	done
done < ~/dd/main.h

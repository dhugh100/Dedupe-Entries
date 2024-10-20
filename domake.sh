# -save-temps option if want to see code
if [  -z $1 ]; then
  echo "missing parm for file count"
  exit 1
fi  
if [ $1 -eq 0 ]; then
  echo "skipping testdd file create"
else
  /home/dhugh/Do/doddf.sh $1
fi  
gcc -iquote /home/dhugh/C/dd `pkg-config --cflags gtk4` -g -o main  main.c get_folders.c load_entry_data.c traverse.c get_hash.c get_results.c show_columns.c install_property.c work_selected.c see_entry_data.c view_file.c sort_columns.c filter_columns.c work_trash.c work_options.c -lcrypto `pkg-config --libs gtk4` 
#rm /home/dhugh/C/dd/*.o
#rm /home/dhugh/C/dd/*.i
#rm /home/dhugh/C/dd/*.s

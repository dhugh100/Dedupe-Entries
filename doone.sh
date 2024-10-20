gcc -iquote /home/dhugh/C/dd `pkg-config --cflags gtk4` -g -o $1 $1.c -lcrypto `pkg-config --libs gtk4` 

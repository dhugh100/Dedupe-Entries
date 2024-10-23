# Purpose: Compile a C program with GTK4 and OpenSSL
gcc -iquote /home/dhugh/C/dd `pkg-config --cflags gtk4` -g -o $1 $1.c -lcrypto `pkg-config --libs gtk4` 

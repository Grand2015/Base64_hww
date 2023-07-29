# Base64_hww
Base64 Encryption and Decryption

compile cmd
1. static libary file compile cmd
 gcc base64_hww.c -lm -c
 ar rcs libbase64_hww.a base64_hww.o
2. test demo compile cmd
gcc base64_hww.c -L ./ -l base64_hww -lm -o base64

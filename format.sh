gcc -shared -fPIC -o test.so test.c -ldl
echo -e "1\nLD_PRELOAD=./test.so" > ./outputs/pr.txt
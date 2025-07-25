gcc -shared -fPIC -o ./outputs/test.so test.c -ldl
echo -e "1\nLD_PRELOAD=./test.so" > ./outputs/pr.txt
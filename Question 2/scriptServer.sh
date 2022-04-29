gcc -o server server.c list.c stringFunc.c colors.c -lpthread;
./server $1;
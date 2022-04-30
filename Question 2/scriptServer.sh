gcc -o server server.c colors.c list.c stringFunc.c sema.c -lpthread;
./server $1;
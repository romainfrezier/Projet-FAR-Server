gcc -o server server.c lib/colors.c lib/list.c lib/stringFunc.c lib/sema.c lib/fileServer.c lib/admin.c lib/commandServer.c -lpthread;
./server $1;
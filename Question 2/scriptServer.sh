gcc -lpthread -o server server.c;
PORT=${1:-8686};
./server PORT;
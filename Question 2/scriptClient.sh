gcc -lpthread -o client client.c;
IP=${1:-127.0.0.1};
PORT=${2:-8686};
./client IP PORT;
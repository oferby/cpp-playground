
### compile and create shared library

g++ -c socket_impl.cpp -o socket_impl.o

gcc -shared socket_impl.o -o socket_impl.so

### run server withtout preload library
./server 127.0.0.1 7676

### run server with preload library

LD_PRELOAD=../redirect/socket_impl.so ./server 127.0.0.1 7676
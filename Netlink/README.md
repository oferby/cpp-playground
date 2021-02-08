to compile with libnl, do:

gcc file.cpp -o exec-file $(pkg-config --cflags --libs libnl-3.0 libnl-genl-3.0) 



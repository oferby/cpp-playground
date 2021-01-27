#Slides

https://www.slideshare.net/JianxinXiong/getting-started-with-libfabric
https://www.slideshare.net/dgoodell/ofi-libfabric-tutorial


#Code

get the code from:

https://github.com/ofiwg/libfabric/releases

after ded-compressing do:

mkdir build
cd build
../configure --prefix=/opt/libfabric
make
sudo make install

run fi_info to test the installation
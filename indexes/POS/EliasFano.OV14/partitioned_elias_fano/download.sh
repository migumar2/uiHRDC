## tar xzvf snappy-1.1.1.tar.gz
## cd snappy-1.1.1
## ./configure
## sudo make install

##URL: https://github.com/ot/partitioned_elias_fano
sudo apt-get install libboost-all-dev

git clone https://github.com/ot/partitioned_elias_fano.git --recursive

cmake . -DCMAKE_BUILD_TYPE=Release

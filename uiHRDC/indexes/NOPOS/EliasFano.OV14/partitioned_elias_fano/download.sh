## 1. Install snappy in your system

## tar xzvf snappy-1.1.1.tar.gz
## cd snappy-1.1.1
## ./configure
## sudo make install

## 2. Install libboost-all-dev
sudo apt-get install libboost-all-dev

## 3. Partitioned elias fano codes
## . Not needed, as we provide a (slightly?) modified version of partitioned_elias_fano <makefile, and a few scripts>
## URL: https://github.com/ot/partitioned_elias_fano

# git clone https://github.com/ot/partitioned_elias_fano.git --recursive
# cmake . -DCMAKE_BUILD_TYPE=Release

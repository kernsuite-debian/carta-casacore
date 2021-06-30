# Install carta-casacore

First make sure that you have installed the casacore data, for example by installing the `casacore-data` package from the [KERN suite](https://kernsuite.info/).

The `casacore-data` package installs the data directory in `/usr/share`, and the `carta-casacore` package expects to find it in that location. That is also the default location used by the source build, but you can specify a custom location by adding a flag to `cmake`, for example `-DDATA_DIR=/usr/local/share/casacore/data`.

## Install carta-casacore from the PPA

```
sudo add-apt-repository ppa:cartavis/carta-casacore
sudo apt-get update
sudo apt-get install carta-casacore

```

## Install carta-casacore from source

The build dependencies are the same as for [casacore](https://github.com/casacore/casacore#requirements).

```
git clone https://github.com/CARTAvis/carta-casacore.git

cd carta-casacore
git submodule init && git submodule update

cd casa6
git submodule init && git submodule update

cd ../
mkdir -p build
cd build

cmake .. -DUSE_FFTW3=ON -DUSE_HDF5=ON -DUSE_THREADS=ON -DUSE_OPENMP=ON -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DBUILD_PYTHON=OFF -DUseCcache=1 -DHAS_CXX11=1
make -j8
sudo make install
```

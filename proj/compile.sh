cd src
make clean
make
mv proj ../
cd ..
strip --strip-all proj

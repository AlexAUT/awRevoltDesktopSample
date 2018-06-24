set -e

cd ..
./buildLib.sh
cd test/

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DAW_BUILD_TEST=true ..
make -j 8
cd ..
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/
if [ $1 -gt 1 ]
then
  apitrace trace build/awRevoltTest
elif [ $1 -gt 0 ]
then
  gdb build/awRevoltTest
else
  build/awRevoltTest
fi

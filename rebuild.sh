cd $(dirname $0)
rm -rf build
mkdir build
(cd build && cmake .. && make) && python3.3 testsuite.py


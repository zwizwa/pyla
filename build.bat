call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"

rmdir /s /q build
mkdir build
cd build
cmake -G "NMake Makefiles" -DBOOST_ROOT=C:\local\boost_1_53_0 -DBOOST_LIBRARYDIR=C:\local\boost_1_53_0\lib32-msvc-10.0 -DSWIG_EXECUTABLE=c:\local\swigwin-2.0.12\swig.exe ..

if NOT %ERRORLEVEL%==0 goto end

nmake VERBOSE=1 /K

:end
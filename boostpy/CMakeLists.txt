# https://github.com/TNG/boost-python-examples/blob/master/CMakeLists.txt

cmake_minimum_required(VERSION 2.8.3)
FIND_PACKAGE(PythonInterp)
FIND_PACKAGE(PythonLibs)
FIND_PACKAGE(Boost COMPONENTS python)

ENABLE_TESTING()
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})

PYTHON_ADD_MODULE(experiment experiment.cpp)

# FILE(COPY experiment.py DESTINATION .)
# ADD_TEST(Experiment experiment.py)

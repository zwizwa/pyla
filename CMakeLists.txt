project(pyla)

cmake_minimum_required (VERSION 2.8.6)



# http://www.swig.org/Doc1.3/Introduction.html#Introduction_build_system
# http://swig.10945.n7.nabble.com/Unable-to-find-string-td8928.html
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
SET(CMAKE_SWIG_FLAGS "")
SET_SOURCE_FILES_PROPERTIES(pylacore.i PROPERTIES CPLUSPLUS ON)
# SET_SOURCE_FILES_PROPERTIES(pylacore.i PROPERTIES SWIG_FLAGS "-includeall")
SWIG_ADD_MODULE(pylacore python pylacore.i 
  pylacore.cpp
  uart.cpp
)
SWIG_LINK_LIBRARIES(pylacore ${PYTHON_LIBRARIES})



#add_library(_pylacore SHARED
#  pylacore.cpp
#  pylacore_wrap.cxx
#  uart.cpp
#)

if(WIN32)
#  include_directories("C:/Python33/include")
#  link_directories("C:/Python33/libs")
  SET_TARGET_PROPERTIES(_pylacore PROPERTIES
    COMPILE_FLAGS "/MP /wd4251"
    SUFFIX ".pyd"
    )
else(WIN32)
#  include_directories("/usr/include/python3.3")
  SET_TARGET_PROPERTIES(_pylacore PROPERTIES
    COMPILE_FLAGS "-fPIC"
    PREFIX ""
    )
endif(WIN32)


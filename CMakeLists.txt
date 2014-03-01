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
SWIG_ADD_MODULE(pylacore python pylacore.i 
  pylacore.cpp
  uart.cpp
  saleae.cpp
)

include(ExternalProject)
ExternalProject_Add(SaleaeDeviceSdk
  URL "http://downloads.saleae.com/SDK/SaleaeDeviceSdk-1.1.14.zip"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/SaleaeDeviceSdk-prefix/src/SaleaeDeviceSdk/include)

SET(SALEADEVICESDK_LIB ${CMAKE_BINARY_DIR}/SaleaeDeviceSdk-prefix/src/SaleaeDeviceSdk/lib/)
SET(SALEADEVICE_LIBRARY ${SALEADEVICESDK_LIB}/libSaleaeDevice64.so) #FIXME

SWIG_LINK_LIBRARIES(pylacore
  ${PYTHON_LIBRARIES}
  ${SALEADEVICE_LIBRARY})


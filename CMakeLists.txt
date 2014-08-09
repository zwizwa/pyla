project(pyla)
cmake_minimum_required (VERSION 2.8.6)

IF(WIN32)
ELSE(WIN32)
  find_package(PkgConfig)
  PKG_CHECK_MODULES(Sigrok REQUIRED libsigrok)
ENDIF(WIN32)

find_package(Boost COMPONENTS system thread REQUIRED)


# http://www.swig.org/Doc1.3/Introduction.html#Introduction_build_system
# http://swig.10945.n7.nabble.com/Unable-to-find-string-td8928.html
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${BOOST_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_SWIG_FLAGS "")
SET_SOURCE_FILES_PROPERTIES(pylacore.i PROPERTIES CPLUSPLUS ON)
SWIG_ADD_MODULE(pylacore python pylacore.i 
  pylacore.cpp
  uart.cpp
  saleae.cpp
  syncser.cpp
  measure.cpp
  sigrok.cpp
)  


include(ExternalProject)
ExternalProject_Add(SaleaeDeviceSdk
  URL "http://downloads.saleae.com/SDK/SaleaeDeviceSdk-1.1.14.zip"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

INCLUDE_DIRECTORIES(
        ${Sigrok_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CMAKE_BINARY_DIR}/SaleaeDeviceSdk-prefix/src/SaleaeDeviceSdk/include
)


SET(SALEADEVICESDK_LIB ${CMAKE_BINARY_DIR}/SaleaeDeviceSdk-prefix/src/SaleaeDeviceSdk/lib/)

if(WIN32)
  SET(SALEADEVICE_LIBRARY ${SALEADEVICESDK_LIB}/SaleaeDevice.lib)
else(WIN32)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(SALEADEVICE_LIBRARY ${SALEADEVICESDK_LIB}/libSaleaeDevice64.so)
  else()
    SET(SALEADEVICE_LIBRARY ${SALEADEVICESDK_LIB}/libSaleaeDevice.so)
  endif()
endif(WIN32)



SWIG_LINK_LIBRARIES(pylacore
  ${PYTHON_LIBRARIES}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_THREAD_LIBRARY}
  ${Sigrok_LDFLAGS}
  ${SALEADEVICE_LIBRARY})

## DEBUG: print out all variables available to CMAKE
#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()


project(pyla)
cmake_minimum_required (VERSION 2.8.6)

SET(Boost_USE_STATIC_LIBS     OFF)
SET(Boost_USE_MULTITHREADED    ON)
SET(Boost_USE_STATIC_RUNTIME     OFF)
if(WIN32)
  find_package(Boost COMPONENTS system python REQUIRED)
else(WIN32)
  find_package(Boost COMPONENTS system python-py34 REQUIRED)
endif(WIN32)



# http://www.swig.org/Doc1.3/Introduction.html#Introduction_build_system
# http://swig.10945.n7.nabble.com/Unable-to-find-string-td8928.html
# FIND_PACKAGE(SWIG REQUIRED)
# INCLUDE(${SWIG_USE_FILE})
FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${BOOST_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
# SET(CMAKE_SWIG_FLAGS "")
# SET_SOURCE_FILES_PROPERTIES(pylacore.i PROPERTIES CPLUSPLUS ON)
# SWIG_ADD_MODULE(pylacore python pylacore.i 
#  pylacore.cpp
#  uart.cpp
#  saleae.cpp
#  syncser.cpp
#  measure.cpp
# )

# http://www.recheliu.org/memo/pythonboostcmakemucheasierthabboostbuild
ADD_LIBRARY(pylacore MODULE
  pylacore_bp.cpp
  pylacore.cpp
  uart.cpp
  saleae.cpp
  syncser.cpp
  measure.cpp
)  

if(WIN32)
  set_target_properties(pylacore PROPERTIES SUFFIX ".pyd")
else(WIN32)
  set_target_properties(pylacore PROPERTIES PREFIX "")
endif(WIN32)

include(ExternalProject)
ExternalProject_Add(SaleaeDeviceSdk
  URL "http://downloads.saleae.com/SDK/SaleaeDeviceSdk-1.1.14.zip"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

INCLUDE_DIRECTORIES(
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



# SWIG_LINK_LIBRARIES(pylacore
#   ${PYTHON_LIBRARIES}
#   ${Boost_SYSTEM_LIBRARY}
#   ${SALEADEVICE_LIBRARY})

TARGET_LINK_LIBRARIES(pylacore
  ${Boost_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${SALEADEVICE_LIBRARY})

# # DEBUG: print out all variables available to CMAKE
# get_cmake_property(_variableNames VARIABLES)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()


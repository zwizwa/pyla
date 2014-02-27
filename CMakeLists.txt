project(pyla)

cmake_minimum_required (VERSION 2.8.6)

add_library(_pylacore SHARED
  pylacore.cpp
  pylacore_wrap.cxx
)

if(WIN32)
  include_directories("C:/Python33/include")
  link_directories("C:/Python33/libs")
  SET_TARGET_PROPERTIES(_pylacore PROPERTIES
    COMPILE_FLAGS "/MP /wd4251"
    SUFFIX ".pyd"
    )
endif(WIN32)

if(LINUX)
  include_directories("/usr/include/python3.3")
  SET_TARGET_PROPERTIES(_pylacore PROPERTIES
    COMPILE_FLAGS "-fPIC"
    PREFIX ""
    )
endif(LINUX)


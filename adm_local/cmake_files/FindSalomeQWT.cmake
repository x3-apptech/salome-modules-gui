
# QWT detection for salome

set(QWT_ROOT_DIR $ENV{QWT_ROOT_DIR} CACHE PATH "Path to Qwt directory")
if(EXISTS ${QWT_ROOT_DIR})
  set(CMAKE_INCLUDE_PATH ${QWT_ROOT_DIR}/include)
  set(CMAKE_LIBRARY_PATH ${QWT_ROOT_DIR}/lib)
  set(CMAKE_PROGRAM_PATH ${QWT_ROOT_DIR}/bin)
endif(EXISTS ${QWT_ROOT_DIR})
find_package(QWT)

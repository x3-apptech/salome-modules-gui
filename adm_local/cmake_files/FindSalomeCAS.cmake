
# OpenCascade detection for Salome

set(CAS_ROOT_DIR $ENV{CAS_ROOT_DIR} CACHE PATH "Path to OpenCascade directory")
if(EXISTS ${CAS_ROOT_DIR})
  set(CMAKE_INCLUDE_PATH ${CAS_ROOT_DIR}/inc)
  set(CMAKE_PROGRAM_PATH ${CAS_ROOT_DIR}/bin)
  if(WINDOWS)
    if(CMAKE_BUILD_TYPE STREQUAL Debug)
      set(CMAKE_LIBRARY_PATH ${CAS_ROOT}/win32/libd)
    else(CMAKE_BUILD_TYPE STREQUAL Debug)
      set(CMAKE_LIBRARY_DIR ${CAS_ROOT}/win32/lib)
    endif(CMAKE_BUILD_TYPE STREQUAL Debug)
  else(WINDOWS)
    set(CMAKE_LIBRARY_PATH ${CAS_ROOT_DIR}/lib)
  endif(WINDOWS)
endif(EXISTS ${CAS_ROOT_DIR})
find_package(CAS)

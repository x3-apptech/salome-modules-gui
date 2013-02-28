
# Qt4 detection for Salome

#set(QT_MIN_VERSION 4.8.0)
set(QT_ROOT_DIR $ENV{QT_ROOT_DIR} CACHE PATH "Path to Qt directory")
if(EXISTS ${QT_ROOT_DIR})
  set(CMAKE_INCLUDE_PATH ${QT_ROOT_DIR}/include)
  set(CMAKE_LIBRARY_PATH ${QT_ROOT_DIR}/lib)
  set(CMAKE_PROGRAM_PATH ${QT_ROOT_DIR}/bin)
endif(EXISTS ${QT_ROOT_DIR})
find_package(Qt4 REQUIRED QtCore QtGui QtXml QtWebKit QtOpenGL)
IF(NOT QT4_FOUND)
  MESSAGE(FATAL_ERROR "Qt4 not found, please set QT_ROOT_DIR environment or cmake variable")
ELSE(NOT QT4_FOUND})
  INCLUDE(${QT_USE_FILE})
ENDIF(NOT QT4_FOUND)

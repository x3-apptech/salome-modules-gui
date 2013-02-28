
# PyQt4 detection for Salome

set(PYQT_ROOT_DIR $ENV{PYQT_ROOT_DIR} CACHE PATH "Path to PyQt4 directory")
if(EXISTS ${PYQT_ROOT_DIR})
  set(CMAKE_INCLUDE_PATH ${PYQT_ROOT_DIR}/share/sip ${PYQT_ROOT_DIR}/sip)
  set(CMAKE_PROGRAM_PATH ${PYQT_ROOT_DIR}/bin)
endif(EXISTS ${PYQT_ROOT_DIR})
FIND_PROGRAM(PYQT_PYUIC_EXECUTABLE NAMES pyuic4 pyuic4.bat)
FIND_PATH(PYQT_SIPS_DIR NAMES QtCore QtGui QtXml QtOpenGL QtAssistant QtDesigner QtNetwork QtSql QtSvg QtTest HINTS /usr/share/sip/PyQt4)
SET(PYQT_PYTHONPATH "")
if(EXISTS ${PYQT_ROOT_DIR})
  set(PYQT_PYTHONPATH ${PYQT_ROOT_DIR}/lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)
endif(EXISTS ${PYQT_ROOT_DIR})
EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} -c "import sys; sys.path[:0] = ['${PYQT_PYTHONPATH}']; from PyQt4 import pyqtconfig; sys.stdout.write(pyqtconfig.Configuration().pyqt_sip_flags)"
  OUTPUT_VARIABLE PYQT_SIPFLAGS)
SEPARATE_ARGUMENTS(PYQT_SIPFLAGS)
SET(PYQT_SIPFLAGS ${PYQT_SIPFLAGS} -s .cc -c .
  -I ${PYQT_SIPS_DIR}
  -I ${PYQT_SIPS_DIR}/QtCore -I ${PYQT_SIPS_DIR}/QtGui  -I ${PYQT_SIPS_DIR}/QtXml -I ${PYQT_SIPS_DIR}/QtOpenGL
  -I ${PYQT_SIPS_DIR}/QtAssistant  -I ${PYQT_SIPS_DIR}/QtDesigner -I ${PYQT_SIPS_DIR}/QtNetwork -I ${PYQT_SIPS_DIR}/QtSql
  -I ${PYQT_SIPS_DIR}/QtSvg -I ${PYQT_SIPS_DIR}/QtTest )

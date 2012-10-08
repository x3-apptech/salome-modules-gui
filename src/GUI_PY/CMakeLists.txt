# Copyright (C) 2012  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/SalomeMacros.cmake)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
)
# Python modules to be installed
SET(mypkgpython_PYTHON
  __init__.py
  selectvars.py
  genericdialog.py
  mytestdialog.py
  helper.py
)
SALOME_INSTALL_SCRIPTS("${mypkgpython_PYTHON}" ${GUI_salomepythondir}/salome/gui)

SET(PYUIC_FILES
  SelectVarsDialog_ui.py
  genericdialog_ui.py
  mytestdialog_ui.py
)

FOREACH(output ${PYUIC_FILES})
  STRING(REPLACE "_ui.py" ".ui" input ${output})
  SET(input ${CMAKE_CURRENT_SOURCE_DIR}/${input})
  SET(output ${CMAKE_CURRENT_BINARY_DIR}/${output})
  ADD_CUSTOM_COMMAND(
    OUTPUT ${output}
    COMMAND ${PYUIC_EXECUTABLE} -o ${output} ${input}
    MAIN_DEPENDENCY ${input}
  )
ENDFOREACH(output ${PYUIC_FILES})

ADD_CUSTOM_TARGET(BUILD_PY_UI_FILES ALL DEPENDS ${PYUIC_FILES})

FOREACH(output ${PYUIC_FILES})
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${output} DESTINATION ${GUI_salomepythondir}/salome/gui)
ENDFOREACH(output ${PYUIC_FILES})
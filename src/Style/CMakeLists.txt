# Copyright (C) 2012-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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
INCLUDE(${CMAKE_SOURCE_DIR}/adm_local/cmake_files/UseQT4EXT.cmake)

INCLUDE_DIRECTORIES(
  ${QT_INCLUDES}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
)

SET(COMMON_LIBS ${QT_LIBRARIES} qtx)

SET(GUI_HEADERS   
  Style_PrefDlg.h
  Style_Salome.h
)
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})

SET(RCCS Style.qrc)
QT4_ADD_RESOURCES(RCC_SRCS ${RCCS})

SET(SalomeStyle_SOURCES
  Style.h
  Style_Model.h
  Style_PrefDlg.h
  Style_Salome.h
  Style_Tools.h
  Style_ResourceMgr.h
  Style_Model.cxx
  Style_PrefDlg.cxx
  Style_ResourceMgr.cxx
  Style_Salome.cxx
  Style_Tools.cxx
)

SET(GUITS_SOURCES
  resources/Style_msg_fr.ts
)

ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_LIBRARY(SalomeStyle ${SalomeStyle_SOURCES} ${GUI_HEADERS_MOC} ${RCC_SRCS})
TARGET_LINK_LIBRARIES(SalomeStyle ${COMMON_LIBS})
INSTALL(TARGETS SalomeStyle DESTINATION ${GUI_salomelib_LIBS})

SET(COMMON_HEADERS_H 
  Style.h
  Style_PrefDlg.h
  Style_Salome.h
)

INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")

INSTALL(FILES resources/SalomeStyle.xml DESTINATION ${GUI_salomeres_DATA})

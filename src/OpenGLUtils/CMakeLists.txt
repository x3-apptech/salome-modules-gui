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

INCLUDE_DIRECTORIES(
  ${PTHREAD_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
)

SET(COMMON_LIBS ${OPENGL_LIBRARIES} ${KERNEL_SALOMELocalTrace})

ADD_LIBRARY(OpenGLUtils OpenGLUtils_FrameBuffer.cxx)
TARGET_LINK_LIBRARIES(OpenGLUtils ${COMMON_LIBS})
INSTALL(TARGETS OpenGLUtils DESTINATION ${GUI_salomelib_LIBS})

SET(COMMON_HEADERS_H 
  OpenGLUtils.h
  OpenGLUtils_FrameBuffer.h
)

INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})

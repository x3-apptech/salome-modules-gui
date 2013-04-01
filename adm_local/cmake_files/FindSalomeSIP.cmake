# Copyright (C) 2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

# Sip detection for Salome

set(SIP_ROOT_DIR $ENV{SIP_ROOT_DIR} CACHE PATH "Path to Sip directory")
if(EXISTS ${SIP_ROOT_DIR})
  set(CMAKE_INCLUDE_PATH ${SIP_ROOT_DIR}/include/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR} ${SIP_ROOT_DIR}/include)
  set(CMAKE_PROGRAM_PATH ${SIP_ROOT_DIR}/bin)
endif(EXISTS ${SIP_ROOT_DIR})
find_program(SIP_EXECUTABLE sip REQUIRED)
find_path(SIP_INCLUDE_DIR sip.h REQUIRED HINTS /usr/include/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR})

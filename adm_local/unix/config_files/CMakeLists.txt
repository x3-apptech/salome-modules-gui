# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

SET(dist_admlocalm4_DATA
  check_disable_Corba.m4
  check_GLViewer.m4
  check_GUI.m4
  check_msg2qm.m4
  check_OCCViewer.m4
  check_opengl.m4
  check_Plot2dViewer.m4
  check_PyConsole.m4
  check_pyqt.m4
  check_qt.m4
  check_qwt.m4
  check_SalomeObject.m4
  check_sip.m4
  check_SupervGraphViewer.m4
  check_VTKViewer.m4
  check_QxGraphViewer.m4
  check_TestRecorder.m4
  check_salome_version.m4
)
            
FOREACH(f ${dist_admlocalm4_DATA})
  SET(DEST adm_local/unix/config_files)
  INSTALL(FILES ${f} DESTINATION ${DEST})
ENDFOREACH(f ${dist_admlocalm4_DATA})

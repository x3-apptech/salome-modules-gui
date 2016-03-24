#! /usr/bin/env python
#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

import os, re

# -----------------------------------------------------------------------------

def set_env( args ):
    """Add environment required for GUI module"""
    vtk_overloads_dir = os.path.join( os.getenv( "GUI_ROOT_DIR" ), "lib", "paraview" )
    dirs = re.split( ":|;", os.getenv( 'VTK_AUTOLOAD_PATH', vtk_overloads_dir ) )
    if vtk_overloads_dir not in dirs: dirs[0:0] = [vtk_overloads_dir]
    os.environ['VTK_AUTOLOAD_PATH'] = os.pathsep.join(dirs)
    os.environ['QT_PLUGIN_PATH'] = ''
    return



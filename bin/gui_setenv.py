#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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
    qt_plugin_dir = os.path.join( os.getenv( "QTDIR" ), "plugins" )
    dirs = re.split( ":|;", os.getenv( 'QT_PLUGIN_PATH', qt_plugin_dir ) )
    if qt_plugin_dir not in dirs: dirs[0:0] = [qt_plugin_dir]
    dirs2 = list(set(dirs))
    os.environ['QT_PLUGIN_PATH'] = os.pathsep.join(dirs2)
    return

# Copyright (C) 2015-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
# Author : Adrien BRUNETON (CEA)
#

from omniORB import CORBA
from SALOME_NamingServicePy import SALOME_NamingServicePy_i
import SALOMEDS

from PVServer_ServiceLoader import PVServer_ServiceLoader
import PVSERVER_ORB

###
# Get verbose level
### 
__verbose__ = None
def verbose():
    import os
    global __verbose__
    if __verbose__ is None:
        try:
            __verbose__ = int( os.getenv( 'SALOME_VERBOSE', 0 ) )
        except:
            __verbose__ = 0
            pass
        pass
    return __verbose__

###
# Get ORB reference
###
__orb__ = None
def getORB():
    global __orb__
    if __orb__ is None:
        __orb__ = CORBA.ORB_init( [''], CORBA.ORB_ID )
        pass
    return __orb__

###
# Get naming service instance
###
__naming_service__ = None
def getNS():
    global __naming_service__
    if __naming_service__ is None:
        __naming_service__ = SALOME_NamingServicePy_i( getORB() )
        pass
    return __naming_service__

###
# Get PVSERVER service
###
__service__ = None
__serviceLoader__ = None
def getService():    
    global __service__, __serviceLoader__
    containerName = "FactoryServer"
    if __serviceLoader__ is None:
        __serviceLoader__ = PVServer_ServiceLoader() 
    if __service__ is None:
        import PVSERVER
        ior = __serviceLoader__.findOrLoadService(containerName)
        obj = getORB().string_to_object( ior )
        __service__ = obj._narrow(PVSERVER.PVSERVER)
        pass
    return __service__

###
# Get PVSERVER engine IOR
###
def getServiceIOR():
    IOR = ""
    if getORB() and getService():
        IOR = getORB().object_to_string( getService() )
        pass
    return IOR

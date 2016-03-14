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
# Author : Adrien Bruneton (CEA)
#

import subprocess as subp
import socket
from time import sleep
import os
from SALOME import SALOME_Exception
#from SALOME_utilities import MESSAGE

def MESSAGE(m):
    """ Debug function """
    pass
    #os.system("echo \"%s\" >> /tmp/paravis_log.txt" % m)

class PVSERVER_impl(object):
    """ The core implementation (non CORBA, or Study related).
        See the IDL for the documentation.
    """
    MAX_PVSERVER_PORT_TRIES = 1000  # Maximum number of tries to get a free port for the PVServer
    PVSERVER_DEFAULT_PORT = 11111   # First port being tried to launch the pvserver
    
    def __init__(self):
        self.pvserverPort = -1
        self.pvserverPop = None  # Popen object from subprocess module
        self.isGUIConnected = False  # whether there is an active connection from the GUI.
        try:
            import paraview
            tmp=paraview.__file__
        except:
            raise Exception("PVSERVER_Impl.__init__ : \"import paraview\" failed !")
        # deduce dynamically PARAVIEW_ROOT_DIR from the paraview module location
        self.PARAVIEW_ROOT_DIR = None
        ZE_KEY_TO_FIND_PV_ROOT_DIR="lib"
        li=tmp.split(os.path.sep) ; li.reverse()
        if ZE_KEY_TO_FIND_PV_ROOT_DIR not in li:
            raise SALOME_Exception(SALOME.ExceptionStruct(SALOME.INTERNAL_ERROR,
                      "PVSERVER_Impl.__init__ : error during dynamic deduction of PARAVIEW_ROOT_DIR : Loc of paraview module is \"%s\" ! \"%s\" is supposed to be the key to deduce it !"%(tmp,ZE_KEY_TO_FIND_PV_ROOT_DIR),
                      "PVSERVER.py", 0))
        li=li[li.index("lib")+1:] ; li.reverse()
        self.PARAVIEW_ROOT_DIR = os.path.sep.join(li)

    """
    Private. Identify a free port to launch the PVServer. 
    This is done by trying to bind a socket on the port.
    We are still subject to a race condition between this detection mechanism and the actual launch of the pvserver
    itself ...
    """
    def __getFreePort(self, startPort):
        cnt = 0
        currPort = startPort
        while cnt < self.MAX_PVSERVER_PORT_TRIES:
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.bind(('', currPort))
                s.close()
                return currPort
            except socket.error as e:
                cnt += 1
                currPort += 1
                pass
        raise SALOME_Exception(SALOME.ExceptionStruct(SALOME.INTERNAL_ERROR,
                            "[PVSERVER] maximum number of tries to retrieve a free port for the PVServer",
                            "PVSERVER.py", 0))
                                           
    def FindOrStartPVServer( self, port ):
        MESSAGE("[PVSERVER] FindOrStartPVServer ...")
        host = "localhost"
        alive = True
        if self.pvserverPop is None:
            alive = False
        else:
            # Poll active server to check if still alive
            self.pvserverPop.poll()
            if not self.pvserverPop.returncode is None:  # server terminated
                alive = False
        
        if alive:
            return "cs://%s:%d" % (host, self.pvserverPort)  
          
        # (else) Server not alive, start it:
        pvServerPath = os.path.join(self.PARAVIEW_ROOT_DIR, 'bin', 'pvserver')
        opt = []
        if port <= 0:
            port = self.__getFreePort(self.PVSERVER_DEFAULT_PORT)
        self.pvserverPop = subp.Popen([pvServerPath, "--multi-clients", "--server-port=%d" % port, "--use-offscreen-rendering"])
        sleep(3)  # Give some time to the server to start up to avoid 
                  # ugly messages on the client side saying that it cannot connect
        # Is PID still alive? If yes, consider that the launch was successful
        self.pvserverPop.poll()
        if self.pvserverPop.returncode is None:
            success = True
            self.pvserverPort = port
            MESSAGE("[PVSERVER] pvserver successfully launched on port %d" % port)
        else:
            raise SALOME_Exception(SALOME.ExceptionStruct(SALOME.INTERNAL_ERROR,
                            "[PVSERVER] Unable to start PVServer on port %d!" % port,
                            "PVSERVER.py", 0))
        return "cs://%s:%d" % (host, self.pvserverPort)

    def StopPVServer( self ):
        MESSAGE("[PVSERVER] Trying to stop PVServer (sending KILL) ...")
        if not self.pvserverPop is None:
            self.pvserverPop.poll()
            if self.pvserverPop.returncode is None:
                # Terminate if still running:
                self.pvserverPop.terminate()
                MESSAGE("[PVSERVER] KILL signal sent.")
                return True
        MESSAGE("[PVSERVER] Nothing to kill.")
        return False
      
    def SetGUIConnected( self, isConnected ):
        self.isGUIConnected = isConnected
        
    def GetGUIConnected( self ):
        return self.isGUIConnected


      
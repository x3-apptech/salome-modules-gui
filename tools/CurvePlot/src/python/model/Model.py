# Copyright (C) 2016-2019  CEA/DEN, EDF R&D
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

class Model(object):
    START_ID = -1
  
    @classmethod
    def __GenerateID(cls):
        cls.START_ID += 1
        return cls.START_ID

    def __init__( self, controller ):
        """Constructor"""
    
        self._name = None
        self._controller = controller
        self._id = self.__GenerateID()  # A unique ID for this class of object
    
    def getID(self):
        return self._id
      
    def getController(self):
        """
        :returns: Controller -- This model's controller.
        """
        return self._controller
    
    def setController(self, controller):
        """
        Sets the controller of this model.
        
        :param controller: Controller -- The controller of the model.
        
        """
        self._controller = controller
    
    def notifyChange( self, what="" ) :
        """
        Notifies the controller that this model's data has changed.
        """
        if self._controller != None:
            self._controller.notify(self, what)
    
    def updateTimeStamps (self, modifiesList):
        raise NotImplementedError

pass

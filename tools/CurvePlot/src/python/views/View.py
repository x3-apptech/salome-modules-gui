# Copyright (C) 2016-2020  CEA/DEN, EDF R&D
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

class View(object) :

    def __init__( self, controller, model=None ) :
        """Constructor"""
        
        self._model = model
        self._controller = controller
        pass

    def getModel( self ) :
        """
        :returns: Model -- The view's model. 
        """
        return self._model

    def setModel( self, model ) :
        """
        Associates a model to the view.
        
        :param model: Model -- The model to be associated.
        
        """
        self._model = model
        pass
    
    def getController( self ) :
        """
        :returns: Controller -- The controller of the view.
        """
        return self._controller
    
    def setController( self, controller ) :
        """
        Associates a controller to the view.
        
        :param controller: Controller -- The controller to be associated.
        
        """
        self._controller = controller
        pass
    
    def update( self) :
        """
        Updates the view contents.
        
        .. note::
        
           Virtual Method.
           
        """
        raise NotImplementedError

pass

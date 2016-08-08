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

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

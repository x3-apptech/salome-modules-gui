from PyQt4.QtCore import *
from PyQt4.QtGui import *

import SalomePyQt

_dockWidgetNames = {
    SalomePyQt.WT_ObjectBrowser : "objectBrowserDock",
    SalomePyQt.WT_PyConsole     : "pythonConsoleDock",
    SalomePyQt.WT_LogWindow     : "logWindowDock",
    SalomePyQt.WT_NoteBook      : "noteBookDock",
}

def findDockWidgetByTitle( title ):
    """
    Find and return dock widget by its window title.
    Returns None if dock widget does not exist or is not created yet.
    
    WARNING: this function is language-dependant as the title of the
    dock widget is normally internationalized according to the currently
    used language.

    Example:
      # get object browser
      findDockWidgetByTitle( "Object Browser" )
    """
    sg = SalomePyQt.SalomePyQt()
    dwl = sg.getDesktop().findChildren( QDockWidget )
    dw = filter(lambda a: a.windowTitle() == QString( title ), dwl)
    if dw: return dw[0]
    return None

def findDockWidgetByName( dwName ):
    """
    Find and return dock widget by its internal name
    Returns None if dock widget does not exist or is not created yet
    
    Note: this function is language-independant: internal name
    of the dock widget does not depend on the currently used language.

    Example:
      # get object browser
      findDockWidgetByName( "objectBrowserDock" )
    """
    sg = SalomePyQt.SalomePyQt()
    return sg.getDesktop().findChild( QDockWidget, dwName )

def findDockWidgetById( dwId ):
    """
    Find and return dock widget by its id
    Returns None if dock widget does not exist or is not created yet

    WARNING: this function works only with dock widget ids
    specified in SalomePyQt interface.
    
    Example:
      # get object browser
      findDockWidgetById( SalomePyQt.WT_ObjectBrowser )
    """
    try:
        return findDockWidgetByName( _dockWidgetNames[ dwId ] )
    except:
        pass
    return None

def getAllDockWindows():
    """
    Get all dock widgets.
    
    WARNING: this function searches all dock widgets starting from the
    top-level main window; thus, resulting list contains also dock windows
    that belong to the lower-level windows (e.g. view windows).
    """
    sg = SalomePyQt.SalomePyQt()
    return sg.getDesktop().findChildren( QDockWidget )

def tabifyDockWidgets( dw1, dw2 ):
    """
    Tabify two dock widgets.
    """
    sg = SalomePyQt.SalomePyQt()
    if dw1 and dw2: sg.getDesktop().tabifyDockWidget( dw1, dw2 )
    pass

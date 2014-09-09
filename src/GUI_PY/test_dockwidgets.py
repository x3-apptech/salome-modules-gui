from salome.gui.dockwidgets import *

print "-- Search dock windows by title"
ob = findDockWidgetByTitle( "Object Browser" )
if ob:
    print "object browser:", ob
else:
    print "object browser was not found"

pc  = findDockWidgetByTitle( "Python Console" )
if pc:
    print "python console:", pc
else:
    print "python console was not found"
print 

print "-- Search dock windows by name"
ob = findDockWidgetByName( "objectBrowserDock" )
if ob:
    print "object browser:", ob
else:
    print "object browser was not found"
pc  = findDockWidgetByName( "pythonConsoleDock" )
if pc:
    print "python console:", pc
else:
    print "python console was not found"
print 

print "-- Search dock windows by id"
ob = findDockWidgetById( SalomePyQt.WT_ObjectBrowser )
if ob:
    print "object browser:", ob
else:
    print "object browser was not found"
pc  = findDockWidgetById( SalomePyQt.WT_PyConsole )
if pc:
    print "python console:", pc
else:
    print "python console was not found"
print 

print "-- Tabify dock windows"
tabifyDockWidgets( findDockWidgetById( SalomePyQt.WT_ObjectBrowser ),
                   findDockWidgetById( SalomePyQt.WT_PyConsole ) )

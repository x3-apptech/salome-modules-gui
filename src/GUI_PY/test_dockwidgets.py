# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

from salome.gui.dockwidgets import *

print("-- Search dock windows by title")
ob = findDockWidgetByTitle( "Object Browser" )
if ob:
    print("object browser:", ob)
else:
    print("object browser was not found")

pc  = findDockWidgetByTitle( "Python Console" )
if pc:
    print("python console:", pc)
else:
    print("python console was not found")
print() 

print("-- Search dock windows by name")
ob = findDockWidgetByName( "objectBrowserDock" )
if ob:
    print("object browser:", ob)
else:
    print("object browser was not found")
pc  = findDockWidgetByName( "pythonConsoleDock" )
if pc:
    print("python console:", pc)
else:
    print("python console was not found")
print() 

print("-- Search dock windows by id")
ob = findDockWidgetById( SalomePyQt.WT_ObjectBrowser )
if ob:
    print("object browser:", ob)
else:
    print("object browser was not found")
pc  = findDockWidgetById( SalomePyQt.WT_PyConsole )
if pc:
    print("python console:", pc)
else:
    print("python console was not found")
print() 

print("-- Tabify dock windows")
tabifyDockWidgets( findDockWidgetById( SalomePyQt.WT_ObjectBrowser ),
                   findDockWidgetById( SalomePyQt.WT_PyConsole ) )

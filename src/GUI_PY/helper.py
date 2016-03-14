# -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

# Author: Guillaume Boulant (EDF/R&D)

# ==================================================================
# This file provides helper functions to drive some gui features of a
# SALOME Application (the selection in the object browser for now,
# further development coming soon). Note that some of these helper
# functions are specialized for a GEOM or SMESH context (see the
# guihelper.py modules comming with the packages salome.geom and
# salome.smesh.
# ==================================================================

#
# ==================================================================
# Definition of usefull objects and functions to deal with the SALOME
# technical context.
# ==================================================================
#

from salome.kernel import services

#
# Get SALOME PyQt interface to manipulate the GUI context
# (Get the active study for example. Note that the active study is a
# GUI concept only: it is the study that is currently active in the
# desktop)
#
import SalomePyQt
sgPyQt=SalomePyQt.SalomePyQt()

#
# Get SALOME Swig interface to manipulate the GUI widget
#
import libSALOME_Swig
SalomeGUI = libSALOME_Swig.SALOMEGUI_Swig()

#
# ==================================================================
# General helper function for GUI programming actions
# ==================================================================
# 
# Get the active study
#
def getActiveStudy():
    """
    This returns a study object that corresponds to the active
    study. The active study is a GUI concept: it's the study currently
    active on the desktop.
    """
    studyId = sgPyQt.getStudyId()
    study = services.getStudyManager().GetStudyByID( studyId )
    return study

#
# ==================================================================
# Functions to manipulate the objects in the browser (generic)
# ==================================================================
#
def getSObjectSelected():
    '''
    Returns the sobject and the entry of the item currently selected
    in the objects browser. Returns "None, None" if no item is
    selected. If several objects are selected, it returns the first
    one of the list.
    '''
    sobj = None
    entry = None
    study = getActiveStudy()
    if SalomeGUI.SelectedCount() == 1:
        # We only considere the first element of the list. If you need
        # something else, create another function in your own context.
        entry = SalomeGUI.getSelected( 0 )
        if entry != '':
            sobj = study.FindObjectID( entry )
    return sobj, entry

def showSObjectSelected():
    '''
    This function print the attributes of the selected object.
    (this function is  only for test purpose)
    '''
    sobj, entry = getSObjectSelected()
    if ( sobj ):
        test, attr = sobj.FindAttribute( "AttributeName" )
        if test:
            message = "My name is '%s'" % attr.Value()
            print message
    pass

def deleteSObjectSelected(): 
    '''
    This function deletes the selected object.
    '''
    sobj, entry = getSObjectSelected()
    if ( sobj ):
        study = getActiveStudy()
        builder = study.NewBuilder()
        builder.RemoveObject( sobj )
        SalomeGUI.updateObjBrowser(True)
    pass

#
# ==================================================================
# Use cases and demo functions
# ==================================================================
#

# CAUTION: Before running this test functions, you first have to
# create (or import) an object and select this object in the objects
# browser.

#
# Definitions:
# - the SObject is an item in the active study (Study Object).
# - the entry is the identifier of an item.
# - the object (geom object or smesh object) is a CORBA servant
#   embedded in the SALOME component container and with a reference in
#   the SALOME study, so that it can be retrieved.
#

def TEST_getSObjectSelected():
    mySObject, myEntry = getSObjectSelected()
    myName = mySObject.GetName()
    print "The name of the selected object is %s"%myName

def TEST_showSObjectSelected():
    showSObjectSelected()

if __name__ == "__main__":
    TEST_getSObjectSelected()
    TEST_showSObjectSelected()

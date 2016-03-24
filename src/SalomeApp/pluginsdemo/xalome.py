# -*- coding: iso-8859-1 -*-
# Copyright (C) 2010-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
# Author : Guillaume Boulant (EDF)

#
# XALOME means "eXtension for sALOME. This module contains specific
# helper functions that extends salome for the needs of the salome
# plugin examples, or hide some complex technical parts of SALOME to
# ease the global understanding of the examples.
#
# (gboulant - 09/02/2012)
#
import salome
from salome.kernel.studyedit import getStudyEditor
from salome.kernel.services import IDToSObject, IDToObject
from salome.geom import geomtools

# ======================================================================
# Helper functions to add/remove a geometrical shape in/from the study
# ======================================================================

def addToStudy(study,shape,shapeName,folderName=None):
    """
    Add a GEOM shape in the study. It returns the associated entry
    that corresponds to the identifier of the entry in the study. This
    entry can be used to retrieve an object in the study. A folderName
    can be specified. In this case, a folder with this name is first
    created in the Geometry part of the study, and the shape study
    object is stored in this folder of the study. 
    """
    studyId = study._get_StudyId()
    geompy = geomtools.getGeompy(studyId)

    if folderName is None:
        # Insert the shape in the study by the standard way
        entry = geompy.addToStudy( shape, shapeName )
    else:
        # A folder name has been specified to embed this shape. Find
        # or create a folder with this name in the Geometry study, and
        # then store the shape in this folder.
        studyEditor = getStudyEditor(studyId)
        geomStudyFolder = studyEditor.findOrCreateComponent("GEOM")
        shapeStudyFolder = studyEditor.findOrCreateItem(geomStudyFolder,folderName)

        shapeIor = salome.orb.object_to_string(shape)
        geomgui = salome.ImportComponentGUI("GEOM")
        shapeIcon = geomgui.getShapeTypeIcon(shapeIor)

        shapeStudyObject = studyEditor.createItem(shapeStudyFolder,
                                                  name=shapeName,
                                                  IOR=shapeIor,
                                                  icon=shapeIcon)
        entry = shapeStudyObject.GetID()

    return entry

def removeFromStudy(study,shapeStudyEntry):
    """
    This removes the specified entry from the study. Note that this
    operation does not destroy the underlying GEOM object, neither
    erase the drawing in the viewer.
    The underlying GEOM object is returned (so that it can be destroyed)
    """
    studyId = study._get_StudyId()
    shape = IDToObject(shapeStudyEntry)    
    studyObject = IDToSObject(shapeStudyEntry)
    studyEditor = getStudyEditor(studyId)
    studyEditor.removeItem(studyObject,True)
    return shape


# ======================================================================
# Helper functions to display/erase a shape in/from the viewer. The
# shape must be previously put in the study and the study entry must
# be known.
# ======================================================================

ModeShading = 1
DisplayMode=ModeShading
PreviewColor=[236,163,255]
def displayShape(shapeStudyEntry, color=None):
    """This displays the shape specified by its entry in the study"""
    geomgui = salome.ImportComponentGUI("GEOM")            
    geomgui.createAndDisplayFitAllGO(shapeStudyEntry)
    geomgui.setDisplayMode(shapeStudyEntry, DisplayMode)
    if color is not None:
        geomgui.setColor(shapeStudyEntry, color[0], color[1], color[2])

def eraseShape(shapeStudyEntry):
    """
    This erases from the viewers the shape specified by its study
    entry.
    """
    geomgui = salome.ImportComponentGUI("GEOM")
    eraseFromAllWindows=True
    geomgui.eraseGO(shapeStudyEntry,eraseFromAllWindows)

# Available in SALOME 6.5 only
def displayShape_version65(shapeStudyEntry):
    gst = geomtools.GeomStudyTools()
    gst.displayShapeByEntry(shapeStudyEntry)

def eraseShape_version65(shapeStudyEntry):
    gst = geomtools.GeomStudyTools()
    gst.eraseShapeByEntry(shapeStudyEntry)


# ======================================================================
# Helper functions for a complete suppression of a shape from the
# SALOME session.
# ======================================================================
def deleteShape(study,shapeStudyEntry):
    """
    This completly deletes a geom shape.

    WARNING: please be aware that to delete a geom object, you have
    three operations to perform:

    1. erase the shape from the viewers
    2. remove the entry from the study
    3. destroy the underlying geom object
    """
    eraseShape(shapeStudyEntry)
    shape = removeFromStudy(study, shapeStudyEntry)
    if shape is not None:
      shape.Destroy()
    

#
# ======================================================================
# Unit tests
# ======================================================================
#
# To experiment this unit test, just execute this script in
# SALOME. The script is self-consistent.

def TEST_createAndDeleteShape():
    """
    This test is a simple use case that illustrates how to create a
    GEOM shape in a SALOME session (create the GEOM object, put in in
    the study, and display the shape in a viewer) and delete a shape
    from a SALOME session (erase the shape from the viewer, delete the
    entry from the study, and finally destroy the underlying GEOM
    object).
    """
    import salome
    salome.salome_init()
    study   = salome.myStudy
    studyId = salome.myStudyId

    from salome.geom import geomtools
    geompy = geomtools.getGeompy(studyId)

    # --------------------------------------------------
    # Create a first shape (GEOM object)
    radius = 5
    length = 100
    cylinder = geompy.MakeCylinderRH(radius, length)

    # Register the shape in the study, at the root of the GEOM
    # folder. A name must be specified. The register operation
    # (addToStudy) returns an identifier of the entry in the study.
    cylinderName = "cyl.r%s.l%s"%(radius,length)
    cylinderStudyEntry = addToStudy(study, cylinder, cylinderName)

    # Display the registered shape in a viewer
    displayShape(cylinderStudyEntry)

    # --------------------------------------------------
    # A second shape
    radius = 10
    sphere = geompy.MakeSphereR(radius)
    sphereName = "sph.r%s"%radius
    sphereStudyEntry = addToStudy(study, sphere, sphereName)
    displayShape(sphereStudyEntry)

    # --------------------------------------------------
    # This new shape is stored in the study, but in a specific
    # sub-folder, and is displayed in the viewer with a specific
    # color.
    length = 20
    box = geompy.MakeBoxDXDYDZ(length,length,length)
    boxName = "box.l%s"%length
    folderName = "boxset" 
    boxStudyEntry = addToStudy(study, box, boxName, folderName)
    displayShape(boxStudyEntry,PreviewColor)

    # --------------------------------------------------
    # In this example, we illustrate how to erase a shape (the sphere)
    # from the viewer. After this operation, the sphere is no longer
    # displayed but still exists in the study. You can then redisplay
    # it using the context menu of the SALOME object browser.
    eraseShape(sphereStudyEntry)

    # --------------------------------------------------
    # In this last example, we completly delete an object from the
    # SALOME session (erase from viewer, remove from study and finnaly
    # destroy the object). This is done by a simple call to
    # deleteShape().
    deleteShape(study,cylinderStudyEntry)

    # --------------------------------------------------
    # At the end of the executioon of this test, you should have in
    # the SALOME session:
    # - the box, in a dedicated folder of the study, and displayed in the viewer
    # - the sphere, in the standard place of the study, and not displayed 

    # If you comment the deleteShape line, you should see the cylinder
    # in the study and displayed in the viewer. 

if __name__=="__main__":
    TEST_createAndDeleteShape()

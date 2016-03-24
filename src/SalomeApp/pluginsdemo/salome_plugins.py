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

import salome_pluginsmanager

DEMO_IS_ACTIVATED = False

if DEMO_IS_ACTIVATED:
  # Check that GEOM and SMESH modules are present
  try:
    import GEOM
    from salome.geom import geomBuilder
    geompy = geomBuilder.New(salome.myStudy)
    
    import SMESH, SALOMEDS
    from salome.smesh import smeshBuilder
    smesh =  smeshBuilder.New(salome.myStudy)
  except:
    DEMO_IS_ACTIVATED = False

if DEMO_IS_ACTIVATED:
  # -------------------------------------------------------------------------
  # Example 1: creation of basic objects.
  # The plugin function is implemented here and declared in the pluginsmanager.
  #
  import salome

  def trihedron(context):
      import GEOM
      from salome.geom import geomBuilder

      # Intialize the geompy factory with the active study
      activeStudy = context.study
      geompy = geomBuilder.New(activeStudy)

      # Create the objects
      Vx = geompy.MakeVectorDXDYDZ(10, 0, 0)
      Vy = geompy.MakeVectorDXDYDZ(0, 10, 0)
      Vz = geompy.MakeVectorDXDYDZ(0, 0, 10)
      origin = geompy.MakeVertex(0, 0, 0)

      # Register the objects in the active study
      geompy.addToStudy( Vx, "Vx" )
      geompy.addToStudy( Vy, "Vy" )
      geompy.addToStudy( Vz, "Vz" )
      geompy.addToStudy( origin, "origin" )


  salome_pluginsmanager.AddFunction('DEMO/O,Vx,Vy,Vz',
                                    'Creates the trihedron',
                                    trihedron)


  # -------------------------------------------------------------------------
  # Example 1 bis: creation of basic objects and automatic display
  def trihedron_withdisplay(context):
      import GEOM
      from salome.geom import geomBuilder

      # Intialize the geompy factory with the active study
      activeStudy = context.study
      geompy = geomBuilder.New(activeStudy)

      # Create the objects
      Vx = geompy.MakeVectorDXDYDZ(10, 0, 0)
      Vy = geompy.MakeVectorDXDYDZ(0, 10, 0)
      Vz = geompy.MakeVectorDXDYDZ(0, 0, 10)
      origin = geompy.MakeVertex(0, 0, 0)

      # Register the objects in the active study
      entries=[]
      entries.append(geompy.addToStudy( Vx, "Vx" ))
      entries.append(geompy.addToStudy( Vy, "Vy" ))
      entries.append(geompy.addToStudy( Vz, "Vz" ))
      entries.append(geompy.addToStudy( origin, "origin" ))

      # This part is to automatically display the objects in the active viewer.
      gcomp = salome.ImportComponentGUI("GEOM")
      for entry in entries:
          gcomp.createAndDisplayFitAllGO(entry)


  salome_pluginsmanager.AddFunction('DEMO/O,Vx,Vy,Vz (2)',
                                    'Creates Basic GEOM objects',
                                    trihedron_withdisplay)

  # -------------------------------------------------------------------------
  # Example 2: creation of a shape with parameters that can be read from a GUI.
  # The plugin function (tube_shapewithgui) delegates some action to
  # dedicated imported modules (tube.py and tubedialog.py).
  #

  import tubebuilder
  import xalome

  # A single dialog box is defined and recycled for every call. The
  # fields are initialized with default values given by the tube factory
  # tube.py.
  import tubedialog
  dialog = tubedialog.TubeDialog()
  dialog.setData(tubebuilder.DEFAULT_RADIUS,
                tubebuilder.DEFAULT_LENGTH,
                tubebuilder.DEFAULT_WIDTH)

  def tube_shapewithgui(context):
      global tubebuilder, xalome, dialog
      activeStudy = context.study

      # Get the parameter values from a gui dialog box. If the dialog is
      # closed using the Ok button, then the data are requested from the
      # gui and used to create the shape of the tube.
      dialog.exec_()
      if dialog.wasOk():
          radius, length, width = dialog.getData()
          shape = tubebuilder.createGeometry(activeStudy, radius, length, width)
          entry = xalome.addToStudy(activeStudy, shape, "Tube" )
          xalome.displayShape(entry)


  salome_pluginsmanager.AddFunction('DEMO/Tube shape from parameters',
                                    'Creates a tube object from specified parameters',
                                    tube_shapewithgui)


  # -------------------------------------------------------------------------
  # Example 2 bis: creation of a mesh with parameters that can be read from a GUI.
  # The plugin function (tube_meshwithgui) delegates some action to
  # dedicated imported modules (tubebuilder.py and tubedialog.py).
  #
  def tube_meshwithgui(context):
      global tube, dialog
      activeStudy = context.study

      # Get the parameter values from a gui dialog box. If the dialog is
      # closed using the Ok button, then the data are requested from the
      # gui and used to create the shape of the tube.
      dialog.exec_()
      if dialog.wasOk():
          radius, length, width = dialog.getData()
          mesh = tubebuilder.createModel(activeStudy, radius, length, width)


  salome_pluginsmanager.AddFunction('DEMO/Tube mesh from parameters',
                                    'Creates a tube object from specified parameters',
                                    tube_meshwithgui)


  # -------------------------------------------------------------------------
  # Example 2 ter: creation of a geom object with a preview function in
  # the dialog box. This use case is more complex from the gui point of
  # view because the dialog box is a not modal so that we can have
  # interaction with the complete SALOME application. This modal
  # situation requires to connect button click signal on global
  # functions as a "callback" mechanism.
  #
  # TODO:
  # - coloring the preview in pink
  # - store the tmp study objects in a "preview" folder
  #
  dialogWithApply = tubedialog.TubeDialogOnTopWithApply()
  dialogWithApply.setData(tubebuilder.DEFAULT_RADIUS,
                          tubebuilder.DEFAULT_LENGTH,
                          tubebuilder.DEFAULT_WIDTH)
  activeStudy = None
  previewShapeEntry = None

  DEFAULT_FOLDER_NAME="TubeList"
  DEFAULT_SHAPE_NAME="tube"
  PREVIEW_SHAPE_NAME="preview"

  def acceptCallback():
      """Action to be done when click on Ok"""
      global tubebuilder, xalome
      global dialogWithApply, activeStudy
      global previewShapeEntry, deletePreviewShape
      global DEFAULT_FOLDER_NAME,DEFAULT_SHAPE_NAME

      dialogWithApply.accept()

      if previewShapeEntry is not None:
          deletePreviewShape()

      radius, length, width = dialogWithApply.getData()
      shape = tubebuilder.createGeometry(activeStudy, radius, length, width)
      entry = xalome.addToStudy(activeStudy, shape, DEFAULT_SHAPE_NAME, DEFAULT_FOLDER_NAME)
      xalome.displayShape(entry)

  def rejectCallback():
      """Action to be done when click on Cancel"""
      global dialogWithApply, previewShapeEntry, deletePreviewShape

      dialogWithApply.reject()

      if previewShapeEntry is not None:
          deletePreviewShape()

  import SALOMEDS
  PREVIEW_COLOR=SALOMEDS.Color(1,0.6,1) # pink

  def applyCallback():
      """Action to be done when click on Apply"""
      global tubebuilder, xalome
      global dialogWithApply, activeStudy
      global previewShapeEntry, deletePreviewShape
      global PREVIEW_COLOR, DEFAULT_SHAPE_NAME, DEFAULT_FOLDER_NAME, PREVIEW_SHAPE_NAME

      # We first have to destroy the currently displayed preview shape.
      if previewShapeEntry is not None:
          deletePreviewShape()

      # Then we can create the new shape with the new parameter values
      radius, length, width = dialogWithApply.getData()
      shape = tubebuilder.createGeometry(activeStudy, radius, length, width)
      # We apply a specific color on the shape for the preview state
      shape.SetColor(PREVIEW_COLOR)
      previewShapeEntry = xalome.addToStudy(activeStudy, shape, PREVIEW_SHAPE_NAME, DEFAULT_FOLDER_NAME )
      xalome.displayShape(previewShapeEntry)

  def deletePreviewShape():
      """This delete the shape currently being displayed as a preview"""
      global activeStudy, previewShapeEntry, xsalome
      xalome.deleteShape(activeStudy,previewShapeEntry)
      previewShapeEntry = None

  # Connection of callback functions to the dialog butoon click signals
  dialogWithApply.handleAcceptWith(acceptCallback)
  dialogWithApply.handleRejectWith(rejectCallback)
  dialogWithApply.handleApplyWith(applyCallback)

  def tube_shapewithguiAndPreview(context):
      """
      This plugin open a dialog in an asynchronous mode. Then it
      required callback functions to be associated to the button
      signals.
      """
      global dialogWithApply, activeStudy
      activeStudy = context.study
      dialogWithApply.open()


  salome_pluginsmanager.AddFunction('DEMO/Tube geometry from parameters with preview',
                                    'Creates a tube object from specified parameters',
                                    tube_shapewithguiAndPreview)



# -------------------------------------------------------------------------
# Example 3: run a shell session in a xterm to get a SALOME python console
def runSalomeShellSession(context):
    import os,subprocess
    import salome_version
    version = salome_version.getVersion(full=True)
    kernel_appli_dir = os.environ['KERNEL_ROOT_DIR']
    command = ""
    if os.path.exists("/usr/bin/gnome-terminal"):
      command = 'gnome-terminal -t "SALOME %s - Shell session" -e "%s/salome shell" &'%(version,kernel_appli_dir)
    elif os.path.exists("/usr/bin/konsole"):
      command = 'PATH="/usr/bin:/sbin:/bin" LD_LIBRARY_PATH="" konsole -e "%s/salome shell" &'%(kernel_appli_dir)
    elif os.path.exists("/usr/bin/xterm"):
      command = 'xterm -T "SALOME %s - Shell session" -e "%s/salome shell" &'%(version,kernel_appli_dir)
    else:
      print "Neither xterm nor gnome-terminal nor konsole is installed."

    if command is not "":
      try:
        subprocess.check_call(command, shell = True)
      except Exception, e:
        print "Error: ",e


salome_pluginsmanager.AddFunction('SALOME shell session',
                                  'Execute a SALOME shell session in an external xterm',
                                  runSalomeShellSession)

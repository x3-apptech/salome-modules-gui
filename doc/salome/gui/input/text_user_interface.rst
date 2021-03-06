.. _tui_page: 

#################################
Using SALOME GUI python interface
#################################


The extended salome.py Python module provides **sg** variable, which gives access to some GUI functions.

**Note**, that this variable is not available if you use salome.py
Python module outside of the GUI desktop, i.e. without the embedded Python
console (since SWIG library is linked directly to the GUI library).

The example of usage:

.. code-block:: python
   :linenos:

	# update Object browser contents
	salome.sg.updateObjBrowser(True)

	# get the active study ID
	studyId = salome.sg.getActiveStudyId() 

	# get the active study name
	studyName = salome.sg.getActiveStudyName()

	# get the selected objects
	selCount = salome.sg.SelectedCount() # the number of selected items
	for i in range(selCount):
	    print salome.sg.getSelected(i) # print the entry ID of i-th selected item

.. code-block:: python
   :linenos:

	# get the list of IDs of all selected objects
	selected = salome.sg.getAllSelected()

	# add an object to the selection
	salome.sg.AddIObject("0:1:1:1") # "0:1:1:1" is an object ID 

	# remove an object from the selection (make it unselected)
	salome.sg.RemoveIObject("0:1:1:1") # "0:1:1:1" is an object ID 

	# clear the selection (set all objects unselected)
	salome.sg.ClearIObjects()

	# display an object in the current view (if possible)
	salome.sg.Display("0:1:1:1") # "0:1:1:1" is an object ID 
	salome.sg.UpdateView() # update view

	# erase an object from the current view
	salome.sg.Erase("0:1:1:1") # "0:1:1:1" is an object ID 
	salome.sg.UpdateView() # update view

	# display all objects in the current view (if possible)
	salome.sg.DisplayAll()
	salome.sg.UpdateView() # update view

	# erase all objects from the current view
	salome.sg.EraseAll()
	salome.sg.UpdateView() # update view

	# set top, bottom, front, rear, left, right view
	salome.sg.ViewTop() # top view
	salome.sg.ViewBottom() # bottom view
	salome.sg.ViewFront() # front view
	salome.sg.ViewTop() #  back view
	salome.sg.ViewLeft() # left view
	salome.sg.ViewRight() # right view

	# reset the current view
	salome.sg.ResetView()

	# get the component symbolic name by its user name
	compName = salome.sg.getComponentName("Geometry") # compoName = "GEOM"

	# get the component user name by its symbolic name
	compName = salome.sg.getComponentUserName("SMESH") # compoName = "Mesh"

	# ...



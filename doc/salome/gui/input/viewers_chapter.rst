.. _viewers_page: 

#######
Viewers
#######

SALOME GUI module includes several 2D and 3D viewers, that are used for
different purposes within the platform:

* :ref:`occ_3d_viewer_page` has been developed on the basis of Open CASCADE Technology (http://www.opencascade.org). This is the default viewer for SALOME Geometry module, it provides good representation of construction and transformation of geometrical objects.

* :ref:`vtk_3d_viewer_page` has been developed basing on Kitware Visualization ToolKit library (http://www.vtk.org). This is the default viewer for SALOME Mesh module, where it is used for visualization of meshes. OCC and VTK 3d viewers share some  :ref:`common_functionality_page`. 

* :ref:`plot2d_viewer_page` has been developed basing on the open-source Qwt library (http://qwt.sourceforge.net). It can be used, for example, for the representation of 2D plots and graphs.

* :ref:`gl_2d_viewer_page` is a general purpose OpenGL-based viewer, which can be used for visualization of 2D scenes. This type of viewer is not currently used in SALOME platform directly, but can be used in custom modules for 2D visualization purposes.

* :ref:`qxscene_2d_viewer_page` has been developed on the basis of Qt QGraphicsView scene. This viewer is used in SALOME YACS module for visualization of computation schemes.

* :ref:`python_viewer_page` is an editor for Python scripts.


.. toctree::
	:maxdepth: 2

        occ_3d_viewer.rst
	vtk_3d_viewer.rst
	common_functionality.rst
	plot2d_viewer.rst
	gl_2d_viewer.rst
	qxscene_2d_viewer.rst
	python_viewer.rst


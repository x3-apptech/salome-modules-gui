.. _introduction_to_gui_page: 

###################
Introduction to GUI
###################

.. image:: ../images/view2.png
	:align: center

**SALOME GUI** is based on SUIT (Salome User Interface
Toolkit) that proposes very flexible, powerful and safe mechanisms of
interaction with SALOME tool components (both CORBA and standalone),
resource management, viewers and selection handling,
exception and signals processing.

All Graphical User Interfaces of standard modules have plug-in
structure, i.e. each module is placed in a dynamic library which is
loaded on demand. So, all modules add their own menu items, buttons
in toolbar, windows etc.

**GUI** (Graphical User Interface) provides a common shell for all
components, which can be integrated into the SALOME platform and some
basic GUI functionalities, common for all modules.

.. image:: ../images/mainmenu.png
	:align: center


* **File** menu corresponds to :ref:`study_management_page` functionalities (creation, saving, loading, connecting, disconnecting, editing studies, etc.)
* **Edit** menu gives access to ** Copy/Paste** allowing to paste the objects from one study into the other. The availability of this functionality depends on the module and the nature of an operation or an object.
* **View** and **Window** menus provide functionalities of :ref:`salome_desktop_page`. Basically, they allow to show/hide toolbars, activate and manage study windows, change the appearance of Salome, etc. Among key parts of study window are: 

	* Viewer window, used for visualization of objects,
	* :ref:`using_object_browser_page`, used for management of objects created or imported into the SALOME application, also providing search possibilities with the :ref:`using_find_tool_page`, and 
	* :ref:`using_notebook` - allows to predefine study variables.
	* :ref:`python_interpreter_page`, used for direct input of python commands and dumping studies into Python scripts.  
	* Almost all aspects of Salome look and feel can be tuned by the user in :ref:` themes_page "Themes" dialog. 

* **Tools** menu gives access to: :ref:`using_catalog_generator_page`, :ref:`using_registry_tool_page` and :ref:`using_pluginsmanager`
* **Help** menu gives access to the help on Salome modules. The appropriate help page can also be called from any operation dialog via **Help** button.


Help for the GUI module provides information about standard
:ref:`viewers_page`.

General application preferences are described in the :ref:`setting_preferences_page` 
section of SALOME GUI Help.


:ref:`using_input_widgets_page` sub-section
covers specific aspects of behavior of certain kinds of SALOME GUI
input fields.


.. toctree::
	:maxdepth: 3

        study_management_chapter.rst
	salome_desktop.rst
	using_object_browser.rst
	using_find_tool.rst
	using_notebook.rst
	themes.rst
	using_catalog_generator.rst
	working_with_python_scripts.rst 
	using_registry_tool.rst
	viewers_chapter.rst
	setting_preferences.rst
	using_input_widgets.rst




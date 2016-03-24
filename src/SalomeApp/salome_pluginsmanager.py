# -*- coding: utf-8 -*-
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

"""
This module is imported from C++ SalomeApp_Application and initialized
(call to initialize function with 4 parameters)
module :       0 if it is plugins manager at the application level, 1 if it is at the module level
name :         the name of the plugins manager. This name is used to build the name of the plugins files
basemenuname : the name of the menu into we want to add the menu of the plugins ("Tools" for example)
menuname :     the name of plugins menu

A plugins manager is created when calling initialize.

The plugins manager creates a submenu <menuname> in the <basemenuname>
menu.

The plugins manager searches in $HOME/.config/salome/Plugins,
$HOME/$APPLI/Plugins, $SALOME_PLUGINS_PATH directories files named
<name>_plugins.py and executes them.

These files should contain python code that register functions into
the plugins manager.

Example of a plugins manager with name salome. It searches files with
name salome_plugins.py (example follows)::

  import salome_pluginsmanager

  def about(context):
    from qtsalome import QMessageBox
    QMessageBox.about(None, "About SALOME pluginmanager", "SALOME plugins manager in SALOME virtual application ")

  salome_pluginsmanager.AddFunction('About plugins','About SALOME pluginmanager',about)

All entries in menu are added in the same order as the calls to
AddFunction.  It is possible to customize this presentation by getting
the entries list (salome_pluginsmanager.entries()) and modifying it in
place. For example, you can do that :
salome_pluginsmanager.entries().sort() to order them alphabetically or
salome_pluginsmanager.entries().remove("a") to remove the entry named "a".

It is possible to put entries in submenus. You only need to give a
name with / to the entry. for example::

  salome_pluginsmanager.AddFunction('a/b/About','About SALOME pluginmanager',about)

will add 2 submenus a and b before creating the entry.

In short to add a plugin:

  1. import the python module salome_pluginsmanager (in your
  salome_plugins.py or <module>_plugins.py)

  2. write a function with one argument context (it's an object with 3
  attributes)

  3. register the function with a call to AddFunction (entry in menu plugins,
  tooltip, function)

context attributes:

  - sg : the SALOME Swig interface
  - studyId : the SALOME studyId that must be used to execute the plugin
  - study : the SALOME study object that must be used to execute the plugin

"""

import os,sys,traceback
from qtsalome import *

import salome

SEP=":"
if sys.platform == "win32":
  SEP = ";"

# Get SALOME PyQt interface
import SalomePyQt
sgPyQt = SalomePyQt.SalomePyQt()

# Get SALOME Swig interface
import libSALOME_Swig
sg = libSALOME_Swig.SALOMEGUI_Swig()

plugins={}
current_plugins_manager=None

def initialize(module,name,basemenuname,menuname):
  if not plugins.has_key(name):
    if module:
      plugins[name]={}
    else:
      plugins[name]=[]
  if module:
    d=sgPyQt.getDesktop()
    if plugins[name].has_key(d):return
    plugins[name][d]=PluginsManager(module,name,basemenuname,menuname)
  else:
    plugins[name].append(PluginsManager(module,name,basemenuname,menuname))

class Context:
    def __init__(self,sgpyqt):
        self.sg=sgpyqt
        self.studyId=salome.sg.getActiveStudyId()
        self.study= salome.myStudyManager.GetStudyByID(self.studyId)

def find_menu(smenu):
  lmenus=smenu.split("|")
  # Take first element from the list
  main=lmenus.pop(0).strip()
  menu=sgPyQt.getPopupMenu(main)
  return findMenu(lmenus,menu)

def findMenu(lmenu,menu):
  if not lmenu:return menu
  # Take first element from the list
  m=lmenu.pop(0).strip()
  for a in menu.actions():
    if a.menu():
      if a.text() == m:
        return findMenu(lmenu,a.menu())

PLUGIN_PATH_PATTERN="share/salome/plugins"
MATCH_ENDING_PATTERN="_plugins.py"
from salome.kernel.syshelper import walktree
from salome.kernel.logger import Logger
#from salome.kernel.termcolor import GREEN
logger=Logger("PluginsManager") #,color=GREEN)
# VSR 21/11/2011 : do not show infos in the debug mode
#logger.showDebug()

class PluginsManager:
    def __init__(self,module,name,basemenuname,menuname):
        self.name=name
        self.basemenuname=unicode(basemenuname, "utf-8")
        self.menuname=unicode(menuname, "utf-8")
        self.module=module
        self.registry={}
        self.handlers={}
        self.entries=[]
        self.lasttime=0
        self.plugindirs=[]
        self.plugins_files=[]

        # MODULES plugins directory.
        # The SALOME modules may provides natively some plugins. These
        # MODULES plugins are supposed to be located in the
        # installation folder of the module, in the subdirectory
        # "share/salome/plugins". We first look for these directories.
        for key in os.environ.keys():
          if key.endswith("_ROOT_DIR"):
            rootpath=os.environ[key]
            dirpath=os.path.join(rootpath,PLUGIN_PATH_PATTERN)
            if os.path.isdir(dirpath) and dirpath not in self.plugindirs:
              logger.debug("Looking for plugins in the directory %s ..."%dirpath)
              walktree(dirpath,self.analyseFile)

        # USER plugins directory
        user_dir = os.path.expanduser("~/.config/salome/Plugins")
        self.plugindirs.append(user_dir)
        logger.info("The user directory %s has been added to plugin paths"%user_dir)
        # obsolete: USER plugins directory
        # (for compatibility reasons only; new plugins should be stored in ~/.config/salome/Plugins)
        user_obsolete_dir = os.path.expanduser("~/.salome/Plugins")
        self.plugindirs.append(user_obsolete_dir)
        logger.info("The user directory %s has been added to plugin paths (deprecated)"%user_obsolete_dir)

        # APPLI plugins directory
        appli=os.getenv("APPLI")
        if appli:
          appli_dir=os.path.join(os.path.expanduser("~"),appli,"Plugins")
          self.plugindirs.append(appli_dir)
          logger.info("The APPLI directory %s has been added to plugin paths"%appli_dir)

        #SALOME_PLUGINS_PATH environment variable (list of directories separated by ":")
        pluginspath=os.getenv("SALOME_PLUGINS_PATH")
        if pluginspath:
          for directory in pluginspath.split(SEP):
            self.plugindirs.append(directory)
            logger.info("The directory %s has been added to plugin paths"%directory)

        self.basemenu = find_menu(self.basemenuname)

        if self.module:
          self.menu=QMenu(self.menuname)
          mid=sgPyQt.createMenu(self.menu.menuAction(),self.basemenuname)
        else:
          self.menu=QMenu(self.menuname,self.basemenu)
          self.basemenu.addMenu(self.menu)

        self.menu.menuAction().setVisible(False)

        self.basemenu.aboutToShow.connect(self.importPlugins)

    def analyseFile(self,filename):
      """
      This function checks if the specified file is a plugins python
      module and add the directory name of this file to the list of
      plugin paths. This function is aimed to be used as the callback
      function of the walktree algorithm.
      """
      if str(filename).endswith(MATCH_ENDING_PATTERN):
        dirpath=os.path.dirname(filename)
        if dirpath not in self.plugindirs:
          self.plugindirs.append(dirpath)
          logger.debug("The directory %s has been added to plugin paths"%dirpath)
        
    def AddFunction(self,name,description,script):
        """ Add a plugin function
        """
        self.registry[name]=script,description
        self.entries.append(name)

        def handler(obj=self,script=script):
          try:
            script(Context(sgPyQt))
          except:
            s=traceback.format_exc()
            QMessageBox.warning(None,"Exception occured",s)

        self.handlers[name]=handler

    def importPlugins(self):
        """Execute the salome_plugins file that contains plugins definition """
        studyId=sg.getActiveStudyId()
        if studyId == 0:
          self.menu.clear()
          self.menu.menuAction().setVisible(False)
          return
        elif self.lasttime ==0 or salome.myStudy == None:
          salome.salome_init(embedded=1)

        lasttime=0

        plugins_files=[]
        plugins_file_name=self.name+MATCH_ENDING_PATTERN
        for directory in self.plugindirs:
          plugins_file = os.path.join(directory,plugins_file_name)
          if os.path.isfile(plugins_file):
            plugins_files.append((directory,plugins_file))
            lasttime=max(lasttime,os.path.getmtime(plugins_file))

        plugins_files.sort()

        if not plugins_files:
          self.registry.clear()
          self.handlers.clear()
          self.entries=[]
          self.lasttime=0
          self.menu.clear()
          self.menu.menuAction().setVisible(False)
          return

        if self.plugins_files != plugins_files or lasttime > self.lasttime:
          global current_plugins_manager
          current_plugins_manager=self
          self.registry.clear()
          self.handlers.clear()
          self.entries=[]
          self.lasttime=lasttime
          for directory,plugins_file in plugins_files:
            logger.debug("look for python path: %s"%directory)
            if directory not in sys.path:
              sys.path.insert(0,directory)
              logger.debug("The directory %s has been added to PYTHONPATH"%directory)
            try:
              execfile(plugins_file,globals(),{})
            except:
              logger.fatal("Error while loading plugins from file %s"%plugins_file)
              traceback.print_exc()

          self.updateMenu()

    def updateMenu(self):
        """Update the Plugins menu"""
        self.menu.clear()
        for entry in self.entries:
          names=entry.split("/")
          if len(names) < 1:continue
          parentMenu=self.menu

          if len(names) > 1:
            #create or get submenus
            submenus={}
            for action in parentMenu.actions():
              menu=action.menu()
              if menu:
                submenus[str(menu.title())]=menu
            while len(names) > 1:
              name=names.pop(0)
              if submenus.has_key(name):
                amenu=submenus[name]
              else:
                amenu=QMenu(name,parentMenu)
                parentMenu.addMenu(amenu)
                submenus[name]=amenu
              parentMenu=amenu

          name=names.pop(0)
          act=parentMenu.addAction(name,self.handlers[entry])
          act.setStatusTip(self.registry[entry][1])

        self.menu.menuAction().setVisible(True)

def AddFunction(name,description,script):
   """ Add a plugin function
       Called by a user to register a function (script)
   """
   return current_plugins_manager.AddFunction(name,description,script)

def entries():
  """ Return the list of entries in menu: can be sorted or modified in place to customize menu content """
  return current_plugins_manager.entries

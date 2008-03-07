#!/bin/csh -f

# this variable necessary for loading .ini or .xml file
if ( ! $?LightAppConfig )    setenv LightAppConfig ${GUI_ROOT_DIR}/share/salome/resources/gui
if ( ! $?LightAppResources ) setenv LightAppResources ${GUI_ROOT_DIR}/share/salome/resources/gui

SUITApp LightApp -style salome $* &

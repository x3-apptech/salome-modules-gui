#!/bin/csh -f

if ( ! $?SUITRoot )          setenv SUITRoot ${GUI_ROOT_DIR}/share/salome

# this variable necessary for loading .ini or .xml file
if ( ! $?LightAppConfig )    setenv LightAppConfig ${GUI_ROOT_DIR}/share/salome/resources
if ( ! $?LightAppResources ) setenv LightAppResources ${GUI_ROOT_DIR}/share/salome/resources

SUITApp LightApp $1

#!/bin/bash -f

if [ -z "$SUITRoot" ] ; then          
  export SUITRoot=${GUI_ROOT_DIR}/share/salome
fi
# this variable necessary for loading .ini or .xml file
if [ -z "$LightAppConfig" ] ; then
  export LightAppConfig=${GUI_ROOT_DIR}/share/salome/resources
fi
if [ -z "$LightAppResources" ] ; then
  export LightAppResources=${GUI_ROOT_DIR}/share/salome/resources
fi

SUITApp LightApp -style salome $* &

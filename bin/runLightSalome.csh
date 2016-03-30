#!/bin/csh -f
# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
#  File   : runLightSalome.csh
#  Author : Vadim SANDLER, Open CASCADE S.A.S, vadim.sandler@opencascade.com
#

###
# set default value for the LightAppConfig variable, which 
# is necessary for loading of .ini or .xml resources file
###

if ( ! ($?LightAppConfig) ) then
    setenv LightAppConfig ${GUI_ROOT_DIR}/share/salome/resources/gui
else
    setenv LightAppConfig ${LightAppConfig}:${GUI_ROOT_DIR}/share/salome/resources/gui
endif
if ( ! ($?LightAppResources) ) then
    setenv LightAppResources ${GUI_ROOT_DIR}/share/salome/resources/gui
else
    setenv LightAppResources ${LightAppResources}:${GUI_ROOT_DIR}/share/salome/resources/gui
endif

###
# default settings 
###

if (! ($?PATH) ) setenv PATH
if (! ($?LD_LIBRARY_PATH) ) setenv LD_LIBRARY_PATH
if (! ($?PYTHONPATH) ) setenv PYTHONPATH
if (! ($?SALOMEPATH) ) setenv SALOMEPATH

###
# process --modules=... command line option (get list of modules)
###

set modules=""

foreach arg ( X $* )
    if ( "$arg" != "X" ) then
	switch ( $arg )
	    case --modules=*:
		set modules=`echo $arg | awk -F= '{ print $2 }' | sed -e "s%,% %g"`
		breaksw
	    default:
		breaksw
	endsw
    endif
end

set modules="KERNEL GUI $modules"

###
# exclude modules duplication
###

set mods=""

foreach mod ( $modules )
    echo $mods | grep -E "\<$mod\>" >/dev/null
    if ( "$?" == "1" ) then
	set mods="$mods $mod"
    endif
end

###
# set additional environment
###

set python_version=`python -c "import sys; print sys.version[:3]"`

set MY_PATH=""
set MY_LD_LIBRARY_PATH=""
set MY_PYTHONPATH=""

foreach mod ( $mods )
    if ( "$arg" != "X" ) then
	set root_dir=`printenv ${mod}_ROOT_DIR`
	if ( "$root_dir" != "" ) then
	    set mod_lower=`echo $mod | tr "A-Z" "a-z"`
	    if ( -d ${root_dir}/bin/salome ) then
		if ( "${MY_PATH}" == "" ) then
		    set MY_PATH=${root_dir}/bin/salome
		else
		    set MY_PATH=${MY_PATH}:${root_dir}/bin/salome
		endif
	    endif
	    if ( -d ${root_dir}/lib/salome ) then
		if ( "${MY_LD_LIBRARY_PATH}" == "" ) then
		    set MY_LD_LIBRARY_PATH=${root_dir}/lib/salome
		else
		    set MY_LD_LIBRARY_PATH=${MY_LD_LIBRARY_PATH}:${root_dir}/lib/salome
		endif
	    endif
	    if ( "${python_version}" != "" ) then
		if ( -d ${root_dir}/bin/salome ) then
		    if ( "${MY_PYTHONPATH}" == "" ) then
			set MY_PYTHONPATH=${root_dir}/bin/salome
		    else
			set MY_PYTHONPATH=${MY_PYTHONPATH}:${root_dir}/bin/salome
		    endif
		endif
		if ( -d ${root_dir}/lib/salome ) then
		    if ( "${MY_PYTHONPATH}" == "" ) then
			set MY_PYTHONPATH=${root_dir}/lib/salome
		    else
			set MY_PYTHONPATH=${MY_PYTHONPATH}:${root_dir}/lib/salome
		    endif
		endif
		if ( -d ${root_dir}/lib/python${python_version}/site-packages/salome ) then 
		    if ( "${MY_PYTHONPATH}" == "" ) then
			set MY_PYTHONPATH=${root_dir}/lib/python${python_version}/site-packages/salome
		    else
			set MY_PYTHONPATH=${MY_PYTHONPATH}:${root_dir}/lib/python${python_version}/site-packages/salome
		    endif
		endif
	    endif
	    if ( "$mod" != "KERNEL" && "$mod" != "GUI" ) then
		setenv LightAppConfig ${LightAppConfig}:${root_dir}/share/salome/resources/${mod_lower}
	    endif
	    if ( "${SALOMEPATH}" == "" ) then
		setenv SALOMEPATH ${root_dir}
	    else
		setenv SALOMEPATH ${SALOMEPATH}:${root_dir}
	    endif
	endif
    endif
end

if ( "${MY_PATH}" != "" ) setenv PATH ${MY_PATH}:${PATH}
if ( "${MY_LD_LIBRARY_PATH}" != "" ) setenv LD_LIBRARY_PATH ${MY_LD_LIBRARY_PATH}:${LD_LIBRARY_PATH}
if ( "${PYTHONPATH}" != "" ) setenv PYTHONPATH ${MY_PYTHONPATH}:${PYTHONPATH}

###
# start application
###

suitexe LightApp $* &

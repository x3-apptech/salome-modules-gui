#!/bin/bash -f
#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
# File   : runLightSalome.sh
# Author : Vadim SANDLER, Open CASCADE S.A.S, vadim.sandler@opencascade.com
###
# set default value for the LightAppConfig variable, which 
# is necessary for loading of .ini or .xml resources file
###
#
if [ -z "$LightAppConfig" ] ; then
  export LightAppConfig=${GUI_ROOT_DIR}/share/salome/resources/gui
else
  export LightAppConfig=${LightAppConfig}:${GUI_ROOT_DIR}/share/salome/resources/gui
fi
if [ -z "$LightAppResources" ] ; then
  export LightAppResources=${GUI_ROOT_DIR}/share/salome/resources/gui
else
  export LightAppResources=${LightAppResources}:${GUI_ROOT_DIR}/share/salome/resources/gui
fi

###
# process --modules=... command line option (get list of modules)
###

modules=""

for arg in X $* ; do
    if [ "$arg" != "X" ] ; then
	case $arg in
	    --modules=* )  modules=`echo $arg | awk -F= '{ print $2 }' | sed -e "s%,% %g"` ;;
            *)             ;;
        esac
    fi
done

modules="KERNEL GUI $modules"

###
# exclude modules duplication
###

mods=""

for mod in $modules ; do
    echo $mods | grep -E "\<$mod\>" >/dev/null 2>&1
    if [ "$?" == "1" ] ; then
	mods="$mods $mod"
    fi
done

###
# set additional environment
###

python_version=`python -c "import sys; print sys.version[:3]" 2>/dev/null`

MY_PATH=""
MY_LD_LIBRARY_PATH=""
MY_PYTHONPATH=""

for mod in $mods ; do
    if [ "$arg" != "X" ] ; then
	root_dir=`printenv ${mod}_ROOT_DIR`
	if [ "$root_dir" != "" ] ; then
	    mod_lower=`echo $mod | tr "A-Z" "a-z"`
	    if [ -d ${root_dir}/bin/salome ] ; then
		if [ "${MY_PATH}" == "" ] ; then
		    MY_PATH=${root_dir}/bin/salome
		else
		    MY_PATH=${MY_PATH}:${root_dir}/bin/salome
		fi
	    fi
	    if [ -d ${root_dir}/lib/salome ] ; then
		if [ "${MY_LD_LIBRARY_PATH}" == "" ] ; then
		    MY_LD_LIBRARY_PATH=${root_dir}/lib/salome
		else
		    MY_LD_LIBRARY_PATH=${MY_LD_LIBRARY_PATH}:${root_dir}/lib/salome
		fi
	    fi
	    if [ "${python_version}" != "" ] ; then
		if [ -d ${root_dir}/bin/salome ] ; then
		    if [ "${MY_PYTHONPATH}" == "" ] ; then
			MY_PYTHONPATH=${root_dir}/bin/salome
		    else
			MY_PYTHONPATH=${MY_PYTHONPATH}:${root_dir}/bin/salome
		    fi
		fi
		if [ -d ${root_dir}/lib/salome ] ; then
		    if [ "${MY_PYTHONPATH}" == "" ] ; then
			MY_PYTHONPATH=${root_dir}/lib/salome
		    else
			MY_PYTHONPATH=${MY_PYTHONPATH}:${root_dir}/lib/salome
		    fi
		fi
		if [ -d ${root_dir}/lib/python${python_version}/site-packages/salome ] ; then 
		    if [ "${MY_PYTHONPATH}" == "" ] ; then
			MY_PYTHONPATH=${root_dir}/lib/python${python_version}/site-packages/salome
		    else
			MY_PYTHONPATH=${MY_PYTHONPATH}:${root_dir}/lib/python${python_version}/site-packages/salome
		    fi
		fi
	    fi
	    if [ "$mod" != "KERNEL" ] && [ "$mod" != "GUI" ] ; then
		export LightAppConfig=${LightAppConfig}:${root_dir}/share/salome/resources/${mod_lower}
	    fi
	    if [ "${SALOMEPATH}" == "" ] ; then
		export SALOMEPATH=${root_dir}
	    else
		export SALOMEPATH=${SALOMEPATH}:${root_dir}
	    fi
	fi
    fi
done

if [ "${MY_PATH}" != "" ] ; then
    export PATH=${MY_PATH}:${PATH}
fi
if [ "${MY_LD_LIBRARY_PATH}" != "" ] ; then
    export LD_LIBRARY_PATH=${MY_LD_LIBRARY_PATH}:${LD_LIBRARY_PATH}
fi
if [ "${PYTHONPATH}" != "" ] ; then
    export PYTHONPATH=${MY_PYTHONPATH}:${PYTHONPATH}
fi

###
# start application
###

SUITApp LightApp $* &

#!/bin/bash -f

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

# File   : runLightSalome.sh
# Author : Vadim SANDLER, Open CASCADE S.A.S, vadim.sandler@opencascade.com


###
# function show_usage() : print help an exit
###

show_usage() {
    local RET=0
    if [ $# -gt 0 ] ; then RET=$1 ; fi
    local MOVE1="echo -en \\033[35G"
    local MOVE2="echo -en \\033[22G"
    echo
    echo "Run standalone SALOME desktop".
    echo
    echo "Usage: $(basename $0) [options]"
    echo
    echo "Options:"
    echo "-m MODULES, --modules=MODULES   List of modules, separated by comma, to be used within SALOME session."
    echo "-r RCFILE, --resources=RCFILE   Preferences file to be used within SALOME session."
    echo "-v, --version                   Print SALOME version and exit."
    echo "-h, --help                      Print this help and exit."
    echo
    echo "Example:"
    echo "  $(basename $0) --modules=LIGHT,PYLIGHT"
    echo
   exit ${RET}
}

###
# function show_version() : print SALOME version an exit
###

show_version() {
    local RET=0
    local DIR=$(dirname $0)
    if [ -z "${DIR}" ] ; then DIR=. ; fi
    if [ -f ${DIR}/VERSION ] ; then
	cat ${DIR}/VERSION
    else
        echo
	echo "Error: can't find VERSION file"  > /dev/stderr
        echo
	RET=1
    fi
    exit ${RET}
}

###
# function option_modules() : process --modules / -m command line option
###

option_modules() {
    local MODS
    case $1 in
	modules\=* )
	    MODS=`echo $1 | awk -F "=" '{ if(NF>1) print $2 ; else print "" }'`
	    ;;
	modules* )
            echo
            echo "Error: Invalid option format, please use --modules=MODULES" > /dev/stderr
            echo
            exit 1
	    ;;
	* )
	    MODS=$1
	    ;;
    esac
    if [ "X${MODS}" = "X" ] ; then
        echo
        echo "Error: Please, specify list of modules" > /dev/stderr
        echo
        exit 1
    fi
    MODULES=`echo ${MODS} | sed -e "s%,% %g"`
    return
}

###
# function option_resources() : process --resources / -r command line option
###

option_resources() {
    local RCF
    case $1 in
	resources\=* )
	    RCF=`echo $1 | awk -F "=" '{ if(NF>1) print $2 ; else print "" }'` ;;
	resources* )
            echo
            echo "Error: Invalid option format, please use --resources=RCFILE" > /dev/stderr
            echo
            exit 1
	    ;;
	* ) RCF=$1
    esac
    if [ "X${RCF}" = "X" ] ; then
        echo
        echo "Error: Please, specify resource file" > /dev/stderr
        echo
        exit 1
    fi
    RCFILE="--resources=${RCF}"
    return
}

###
# function run_light_salome(): run SALOME
###

run_light_salome(){

    local MODULES
    local RCFILE

    ###
    # process command line options
    ###

    local OPTION
    while getopts ":-:hvm:r:" OPTION "$@" ; do
	if [ "${OPTION}" = "-" ] ; then
            case ${OPTARG} in
		help       )  show_usage                          ;;
		version    )  show_version                        ;;
		modules*   )  option_modules  "${OPTARG}"         ;;
		resources* )  option_resources  "${OPTARG}"       ;;
		*          )  echo "!!!Wrong option!!!" ; exit 1  ;;
            esac
	else
	    case ${OPTION} in
		h  )  show_usage                                 ;;
		v  )  show_version                               ;;
		m* )  option_modules "${OPTARG}"                 ;;
		r* )  option_resources "${OPTARG}"               ;;
		?  )  echo "!!!Wrong option!!!" ; exit 1         ;;
	    esac
	fi
    done
    shift $((OPTIND - 1))

    ###
    # by default try to detect all available modules
    ###

    if [ -z "${MODULES}" ] ; then
	local ENVVAR
	local ROOTDIR
	for ENVVAR in `env | awk -F= '{print $1}' | grep _ROOT_DIR` ; do
	    local MOD=`echo $ENVVAR | awk -F_ '{print $1}'`
	    local LMOD=`echo ${MOD} | tr 'A-Z' 'a-z'`
	    ROOTDIR=`printenv ${ENVVAR}` 
	    if [ -f ${ROOTDIR}/share/salome/resources/${LMOD}/LightApp.xml ] || [ -f ${ROOTDIR}/share/salome/resources/LightApp.xml ] ; then
		MODULES="${MODULES} ${MOD}"
	    fi
	done
    fi

    ###
    #  initial environment
    ###

    if [ -z "${LightAppResources}" ] ; then
	export LightAppResources=${GUI_ROOT_DIR}/share/salome/resources/gui
    else
	export LightAppResources=${LightAppResources}:${GUI_ROOT_DIR}/share/salome/resources/gui
    fi

    ###
    # exclude modules duplication
    ###

    local MODS=""
    local MOD
    for MOD in ${MODULES} ; do
	case ${MOD} in
	    KERNEL | GUI )
		;;
	    * ) 
		echo ${MODS} | grep -E "\<${MOD}\>" >/dev/null 2>&1
		if [ "$?" == "1" ] ; then
		    MODS="${MODS} ${MOD}"
		fi
		;;
	esac
    done
    MODULES=${MODS}

    ###
    # set additional environment
    ###

    local PVERSION=`python -c "import sys; print sys.version[:3]" 2>/dev/null`
    
    local MY_PATH=""
    local MY_LD_LIBRARY_PATH=""
    local MY_PYTHONPATH=""

    for MOD in KERNEL GUI ${MODULES} ; do
	local ROOTDIR=`printenv ${MOD}_ROOT_DIR`
	if [ -z "${ROOTDIR}" ] ; then continue ; fi
	local LMOD=`echo ${MOD} | tr 'A-Z' 'a-z'`
	if [ -d ${ROOTDIR}/bin/salome ] ; then
	    MY_PATH=${MY_PATH}:${ROOTDIR}/bin/salome
	fi
	if [ -d ${ROOTDIR}/lib/salome ] ; then
	    MY_LD_LIBRARY_PATH=${MY_LD_LIBRARY_PATH}:${ROOTDIR}/lib/salome
	fi
	if [ "${PVERSION}" != "" ] ; then
	    if [ -d ${ROOTDIR}/bin/salome ] ; then
		MY_PYTHONPATH=${MY_PYTHONPATH}:${ROOTDIR}/bin/salome
	    fi
	    if [ -d ${ROOTDIR}/lib/salome ] ; then
		MY_PYTHONPATH=${MY_PYTHONPATH}:${ROOTDIR}/lib/salome
	    fi
	    if [ -d ${ROOTDIR}/lib/python${PVERSION}/site-packages/salome ] ; then 
		MY_PYTHONPATH=${MY_PYTHONPATH}:${ROOTDIR}/lib/python${PVERSION}/site-packages/salome
	    fi
	fi
	if [ -f ${ROOTDIR}/share/salome/resources/${LMOD}/LightApp.xml ] ; then
	    export LightAppConfig=${LightAppConfig}:${ROOTDIR}/share/salome/resources/${LMOD}
	elif [ -f ${ROOTDIR}/share/salome/resources/LightApp.xml ] ; then
	    export LightAppConfig=${LightAppConfig}:${ROOTDIR}/share/salome/resources
	fi
	export SALOMEPATH=${SALOMEPATH}:${ROOTDIR}
    done

    PATH=${MY_PATH}:${PATH}
    PATH=`echo ${PATH} | sed -e "s,^:,,;s,:$,,;s,::\+,:,g"`
    export PATH
    LD_LIBRARY_PATH=${MY_LD_LIBRARY_PATH}:${LD_LIBRARY_PATH}
    LD_LIBRARY_PATH=`echo ${LD_LIBRARY_PATH} | sed -e "s,^:,,;s,:$,,;s,::\+,:,g"`
    export LD_LIBRARY_PATH
    PYTHONPATH=${MY_PYTHONPATH}:${PYTHONPATH}
    PYTHONPATH=`echo ${PYTHONPATH} | sed -e "s,^:,,;s,:$,,;s,::\+,:,g"`
    export PYTHONPATH
    LightAppConfig=`echo ${LightAppConfig} | sed -e "s,^:,,;s,:$,,;s,::\+,:,g"`
    export LightAppConfig
    SALOMEPATH=`echo ${SALOMEPATH} | sed -e "s,^:,,;s,:$,,;s,::\+,:,g"`
    export SALOMEPATH

    ###
    # start application
    ###

    suitexe LightApp --modules=`echo $MODULES | tr " " ","` "${RCFILE}" "$@" &
}

###
# call wrapper function (entry point)
###

run_light_salome "$@"
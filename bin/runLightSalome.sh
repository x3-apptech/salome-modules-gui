#!/bin/bash -f

# Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

###
# function show_usage() : print help an exit
###

show_usage()
{
    echo
    echo "Run standalone SALOME desktop".
    echo
    echo "Usage: $(basename ${0}) [options]"
    echo
    echo "Options:"
    echo "-h, --help                       Print this information and exit."
    echo "-v, --version                    Print SALOME version and exit."
    echo "-m MODULES, --modules=MODULES    Comma-separated list of modules to be used within SALOME session."
    echo "-r RCFILE, --resources=RCFILE    User preferences file to be used within SALOME session, instead of default one."
    echo "-a LANGUAGE, --language=LANGUAGE Language to be used within SALOME session, instead of default one."
    echo "-z, --no-splash                  Do not display splash screeen."
    echo "-c, --no-exception-handler       Do not install exception handler (allows debugging exceptions)."
    echo "-l, --show-license               Show license dialog at start-up."
    echo
    echo "Example:"
    echo "  $(basename ${0}) --modules=LIGHT,PYLIGHT"
    echo
    exit 0
}

###
# function show_version() : print SALOME version an exit
###

show_version()
{
    local where=$(readlink -f $(dirname "${0}"))
    if [ -f ${where}/VERSION ]
    then
        cat ${where}/VERSION
    else
        echo
        echo "Error: can't find VERSION file"  > /dev/stderr
        echo
        exit 1
    fi
    exit 0
}

###
# function long_option() : extract value from long command line option
###

long_option()
{
    local option=${1}
    local value=${2}

    if [ $(echo ${value} | grep -e "^${option}=") ]
    then
        value=$(echo ${value} | sed -e "s%^${option}=%%")
    else
        value=
    fi

    if [ "${value}" = "" ]
    then
        echo "Error: please, specify value for option '--${option}'" > /dev/stderr
        exit 1
    fi

    echo ${value}
}

###
# function remove_duplications() : remove duplications
###

remove_duplications()
{
    local unique
    local m
    for m in ${@}
    do
        case ${m} in
            KERNEL | GUI )
                ;;
            * )
                echo ${unique} | grep -qvE "\<${m}\>" && unique+=" ${m}"
                ;;
        esac
    done
    echo ${unique}
}


###
# function run_light_salome(): run SALOME
###

run_light_salome()
{
    local modules
    local rcfile
    local language
    local no_splash=0
    local debug_exceptions=0
    local show_license=0

    ###
    # process command line options
    ###

    local option
    while getopts ":-:hvzclm:r:a:" option "${@}"
    do
        if [ "${option}" = "-" ]
        then
            case ${OPTARG} in
                help ) show_usage ;;
                version ) show_version ;;    
                no-splash ) no_splash=1 ;;
                no-exception-handler ) debug_exceptions=1 ;;
                show-license ) show_license=1 ;;
                modules* ) modules=$(long_option modules ${OPTARG}) ;;
                resources* ) rcfile=$(long_option resources ${OPTARG}) ;;
                language* ) language=$(long_option language ${OPTARG}) ;;
                * ) echo "Wrong option: --${OPTARG}" ; return 1 ;;
            esac
        else
            case ${option} in
                h ) show_usage ;;
                v ) show_version ;;
                z ) no_splash=1 ;;
                c ) debug_exceptions=1 ;;
                l ) show_license=1 ;;
                m* ) modules=${OPTARG} ;;
                r* ) rcfile=${OPTARG} ;;
                a* ) language=${OPTARG} ;;
                ? ) echo "Wrong option" ; return 1 ;;
            esac
        fi
    done
    shift $((OPTIND - 1))

    modules=$(echo ${modules} | sed -e "s%,% %g")

    ###
    # if modules aren't given in command line option, try to detect all available modules
    ###

    if [ "${modules}" = "" ]
    then
        local envvar
        for envvar in $(env | awk -F= '{print $1}' | grep _ROOT_DIR)
        do
            local mdir=${!envvar}
            local mname=$(echo ${envvar} | awk -F_ '{print $1}')
            local mname_lc=$(echo ${mname} | tr 'A-Z' 'a-z')
            if [ -f ${mdir}/share/salome/resources/${mname_lc}/LightApp.xml ] || [ -f ${mdir}/share/salome/resources/LightApp.xml ]
            then
                modules+=" ${mname}"
            fi
        done
    fi

    ###
    # remove duplications from modules list
    ###

    modules=$(remove_duplications ${modules})

    ###
    # set-up environment
    ###

    if [ "${LightAppResources}" = "" ]
    then
        export LightAppResources=${GUI_ROOT_DIR}/share/salome/resources/gui
    else
        export LightAppResources=${LightAppResources}:${GUI_ROOT_DIR}/share/salome/resources/gui
    fi

    local pyversion=$(python3 -c "import sys; print(sys.version[:3])" 2>/dev/null)
    local my_path
    local my_ld_library_path
    local my_pythonpath

    local m
    for m in KERNEL GUI ${modules}
    do
        local root=${m}_ROOT_DIR
        root=${!root}
        if [ "${root}" != "" ]
        then
            local m_lc=$(echo ${m} | tr 'A-Z' 'a-z')
            test -d ${root}/bin/salome && my_path+=:${root}/bin/salome
            test -d ${root}/lib/salome && my_ld_library_path+=:${root}/lib/salome
            if [ "${pyversion}" != "" ]
            then
                test -d ${root}/bin/salome && my_pythonpath+=:${root}/bin/salome
                test -d ${root}/lib/salome && my_pythonpath+=:${root}/lib/salome
                test -d ${root}/lib/python${pyversion}/site-packages/salome && my_pythonpath+=:${root}/lib/python${pyversion}/site-packages/salome
            fi
            if [ -f ${root}/share/salome/resources/${m_lc}/LightApp.xml ]
            then
                export LightAppConfig+=:${root}/share/salome/resources/${m_lc}
            elif [ -f ${root}/share/salome/resources/LightApp.xml ]
            then
                export LightAppConfig+=:${root}/share/salome/resources
            fi
            export SALOMEPATH+=:${root}
        fi
    done

    PATH=${my_path}:${PATH}
    PATH=$(echo ${PATH} | sed -e "s,^:,,;s,:$,,;s,::\+,:,g")
    export PATH
    LD_LIBRARY_PATH=${my_ld_library_path}:${LD_LIBRARY_PATH}
    LD_LIBRARY_PATH=$(echo ${LD_LIBRARY_PATH} | sed -e "s,^:,,;s,:$,,;s,::\+,:,g")
    export LD_LIBRARY_PATH
    PYTHONPATH=${my_pythonpath}:${PYTHONPATH}
    PYTHONPATH=$(echo ${PYTHONPATH} | sed -e "s,^:,,;s,:$,,;s,::\+,:,g")
    export PYTHONPATH
    LightAppConfig=$(echo ${LightAppConfig} | sed -e "s,^:,,;s,:$,,;s,::\+,:,g")
    export LightAppConfig
    SALOMEPATH=$(echo ${SALOMEPATH} | sed -e "s,^:,,;s,:$,,;s,::\+,:,g")
    export SALOMEPATH

    ###
    # start application
    ###

    local options="--modules=$(echo ${modules} | tr ' ' ',')"
    test "${rcfile}" != "" && options+=" --resources=${rcfile}"
    test "${language}" != "" && options+=" --language=${language}"
    test "${no_splash}" = "1" && options+=" --no-splash"
    test "${debug_exceptions}" = "1" && options+=" --no-exception-handler"
    test "${show_license}" = "1" && options+=" --show-license"
    suitexe LightApp ${options} "${@}"
}

###
# call wrapper function (entry point)
###

run_light_salome "${@}"

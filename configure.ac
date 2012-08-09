# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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

# Author : Marc Tajchman (CEA)
# Date : 28/06/2001
# Modified by : Patrick GOLDBRONN (CEA)
# Modified by : Marc Tajchman (CEA)
# Modified by : Mikhail PONIKAROV (OCN) - autotools usage
#
AC_INIT([Salome2 Project GUI module], [6.5.0], [webmaster.salome@opencascade.com], [SalomeGUI])
AC_CONFIG_AUX_DIR(adm_local/unix/config_files)
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([-Wno-portability])

XVERSION=`echo $VERSION | awk -F. '{printf("0x%02x%02x%02x",$1,$2,$3)}'`
AC_SUBST(XVERSION)
VERSION_DEV=1
AC_SUBST(VERSION_DEV)

# set up MODULE_NAME variable for dynamic construction of directories (resources, etc.)
MODULE_NAME=gui
AC_SUBST(MODULE_NAME)

echo
echo ---------------------------------------------
echo Initialize source and build root directories
echo ---------------------------------------------
echo

########################################################################
# Check --disable-corba-gen and other --disable-* options

if test "x${enable_corba_gen}" != "xno" ; then
    GUI_DISABLE_CORBA="no"
else 
    GUI_DISABLE_CORBA="yes"
fi

if test "x${enable_pyConsole}" != "xno" ; then
    DISABLE_PYCONSOLE="no"
else 
    DISABLE_PYCONSOLE="yes"
fi

if test "x${enable_glViewer}" != "xno" ; then
    DISABLE_GLVIEWER="no"
else 
    DISABLE_GLVIEWER="yes"
fi

if test "x${enable_plot2dViewer}" != "xno" ; then
    DISABLE_PLOT2DVIEWER="no"
else 
    DISABLE_PLOT2DVIEWER="yes"
fi

if test "x${enable_supervGraphViewer}" != "xno" ; then
    DISABLE_SUPERVGRAPHVIEWER="no"
else 
    DISABLE_SUPERVGRAPHVIEWER="yes"
fi

if test "x${enable_occViewer}" != "xno" ; then
    DISABLE_OCCVIEWER="no"
else 
    DISABLE_OCCVIEWER="yes"
fi

if test "x${enable_vtkViewer}" != "xno" ; then
    DISABLE_VTKVIEWER="no"
else 
    DISABLE_VTKVIEWER="yes"
fi

if test "x${enable_salomeObject}" != "xno" ; then
    DISABLE_SALOMEOBJECT="no"
else 
    DISABLE_SALOMEOBJECT="yes"
fi

if test "x${enable_qxGraphViewer}" != "xno" ; then
    DISABLE_QXGRAPHVIEWER="no"
else 
    DISABLE_QXGRAPHVIEWER="yes"
fi

dnl
dnl Initialize source and build root directories
dnl

ROOT_BUILDDIR=`pwd`
ROOT_SRCDIR=`echo $0 | sed -e "s,[[^/]]*$,,;s,/$,,;s,^$,.,"`
cd $ROOT_SRCDIR
ROOT_SRCDIR=`pwd`
cd $ROOT_BUILDDIR

AC_SUBST(ROOT_SRCDIR)
AC_SUBST(ROOT_BUILDDIR)

echo
echo Source root directory : $ROOT_SRCDIR
echo Build  root directory : $ROOT_BUILDDIR
echo
echo

if test -z "$AR"; then
   AC_CHECK_PROGS(AR,ar xar,:,$PATH)
fi
AC_SUBST(AR)

dnl Export the AR macro so that it will be placed in the libtool file
dnl correctly.
export AR

echo
echo ---------------------------------------------
echo testing make
echo ---------------------------------------------
echo

AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_LOCAL_INSTALL
dnl
dnl libtool macro check for CC, LD, NM, LN_S, RANLIB, STRIP + pour les librairies dynamiques !

echo
echo ---------------------------------------------
echo Coniguring production
echo ---------------------------------------------
echo
AC_ENABLE_DEBUG(yes)
AC_DISABLE_PRODUCTION

echo ---------------------------------------------
echo testing libtool
echo ---------------------------------------------

dnl first, we set static to no!
dnl if we want it, use --enable-static
AC_ENABLE_STATIC(no)

AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

dnl Fix up the INSTALL macro if it s a relative path. We want the
dnl full-path to the binary instead.
case "$INSTALL" in
   *install-sh*)
      INSTALL='\${ROOT_BUILDDIR}'/adm_local/unix/config_files/install-sh
      ;;
esac

echo
echo ---------------------------------------------
echo testing C/C++
echo ---------------------------------------------
echo

cc_ok=no
dnl inutil car libtool
dnl AC_PROG_CC
AC_PROG_CXX
AC_CXX_WARNINGS
AC_CXX_TEMPLATE_OPTIONS
AC_DEPEND_FLAG
# AC_CC_WARNINGS([ansi])
cc_ok=yes

dnl Library libdl :
AC_CHECK_LIB(dl,dlopen)

dnl add library libm :
AC_CHECK_LIB(m,ceil)

dnl
dnl Well we use sstream which is not in gcc pre-2.95.3
dnl We must test if it exists. If not, add it in include !
dnl

AC_CXX_HAVE_SSTREAM

echo
echo ---------------------------------------------
echo testing MPI
echo ---------------------------------------------
echo

CHECK_MPI


echo
echo ---------------------------------------------
echo testing LEX \& YACC
echo ---------------------------------------------
echo

lex_yacc_ok=no
AC_PROG_YACC
AC_PROG_LEX
lex_yacc_ok=yes

echo
echo ---------------------------------------------
echo testing python
echo ---------------------------------------------
echo

CHECK_PYTHON

echo
echo ---------------------------------------------
echo testing swig
echo ---------------------------------------------
echo

AM_PATH_PYTHON(2.3)
CHECK_SWIG


dnl echo
dnl echo ---------------------------------------------
dnl echo testing java
dnl echo ---------------------------------------------
dnl echo

dnl CHECK_JAVA

echo
echo ---------------------------------------------
echo Testing Disable Corba
echo ---------------------------------------------
echo

CHECK_DISABLE_CORBA
echo "GUI_DISABLE_CORBA = "$GUI_DISABLE_CORBA

echo
echo ---------------------------------------------
echo testing threads
echo ---------------------------------------------
echo

ENABLE_PTHREADS

if test "x${GUI_DISABLE_CORBA}" != "xyes" ; then
echo
echo ---------------------------------------------
echo testing omniORB
echo ---------------------------------------------
echo

CHECK_OMNIORB

dnl echo
dnl echo ---------------------------------------------
dnl echo testing mico
dnl echo ---------------------------------------------
dnl echo

dnl CHECK_MICO

echo
echo ---------------------------------------------
echo default ORB : omniORB
echo ---------------------------------------------
echo

DEFAULT_ORB=omniORB

echo
echo ---------------------------------------------
echo testing Corba
echo ---------------------------------------------
echo

CHECK_CORBA

AC_SUBST_FILE(CORBA)
corba=make_$ORB
CORBA=adm_local/unix/$corba

fi
echo
echo ---------------------------------------------
echo testing openGL
echo ---------------------------------------------
echo

CHECK_OPENGL

echo
echo ---------------------------------------------
echo testing QT
echo ---------------------------------------------
echo

CHECK_QT

echo
echo ---------------------------------------------
echo testing VTK
echo ---------------------------------------------
echo

CHECK_VTK

echo
echo ---------------------------------------------
echo testing HDF5
echo ---------------------------------------------
echo

CHECK_HDF5

echo
echo ---------------------------------------------
echo BOOST Library
echo ---------------------------------------------
echo

CHECK_BOOST

echo
echo ---------------------------------------------
echo Testing OpenCascade
echo ---------------------------------------------
echo

CHECK_CAS

echo
echo ---------------------------------------------
echo Testing sip
echo ---------------------------------------------
echo

CHECK_SIP

echo
echo ---------------------------------------------
echo Testing pyqt
echo ---------------------------------------------
echo

CHECK_PYQT

echo
echo ---------------------------------------------
echo Testing qwt
echo ---------------------------------------------
echo

CHECK_QWT

CHECK_TESTRECORDER

echo
echo ---------------------------------------------
echo Testing PyConsole
echo ---------------------------------------------
echo

CHECK_PYCONSOLE
echo "DISABLE_PYCONSOLE = "$DISABLE_PYCONSOLE

echo
echo ---------------------------------------------
echo Testing GlViewer
echo ---------------------------------------------
echo

CHECK_GLVIEWER
echo "DISABLE_GLVIEWER = "$DISABLE_GLVIEWER

echo
echo ---------------------------------------------
echo Testing Plot2dViewer
echo ---------------------------------------------
echo

CHECK_PLOT2DVIEWER
echo "DISABLE_PLOT2DVIEWER = "$DISABLE_PLOT2DVIEWER

echo
echo ---------------------------------------------
echo Testing SupervGraphViewer
echo ---------------------------------------------
echo

CHECK_SUPERVGRAPHVIEWER
echo "DISABLE_SUPERVGRAPHVIEWER = "$DISABLE_SUPERVGRAPHVIEWER

echo
echo ---------------------------------------------
echo Testing OCCViewer
echo ---------------------------------------------
echo

CHECK_OCCVIEWER
echo "DISABLE_OCCVIEWER = "$DISABLE_OCCVIEWER

echo
echo ---------------------------------------------
echo Testing VTKViewer
echo ---------------------------------------------
echo

CHECK_VTKVIEWER
echo "DISABLE_VTKVIEWER = "$DISABLE_VTKVIEWER

echo
echo ---------------------------------------------
echo Testing SalomeObject
echo ---------------------------------------------
echo

CHECK_SALOMEOBJECT
echo "DISABLE_SALOMEOBJECT = "$DISABLE_SALOMEOBJECT

echo
echo ---------------------------------------------
echo Testing QxGraphViewer
echo ---------------------------------------------
echo

CHECK_QXGRAPHVIEWER
echo "DISABLE_QXGRAPHVIEWER = "$DISABLE_QXGRAPHVIEWER

if test "x${GUI_DISABLE_CORBA}" != "xyes" ; then
  if test "x${DISABLE_PYCONSOLE}" == "xyes" ; then
    echo "failed : for full configuration of GUI module necessary enable PyConsole !"
    exit
  fi
  if test "x${DISABLE_GLVIEWER}" == "xyes" ; then
    echo "failed : for full configuration of GUI module necessary enable GlViewer !"
    exit
  fi
  if test "x${DISABLE_PLOT2DVIEWER}" == "xyes" ; then
    echo "failed : for full configuration of GUI module necessary enable Plot2dViewer !"
    exit
  fi
  if test "x${DISABLE_SUPERVGRAPHVIEWER}" == "xyes" ; then
    echo "failed : for full configuration of GUI module necessary enable SupervGraphViewer !"
    exit
  fi
  if test "x${DISABLE_OCCVIEWER}" == "xyes" ; then
    echo "failed : for full configuration of GUI module necessary enable OCCViewer !"
    exit
  fi
  if test "x${DISABLE_VTKVIEWER}" == "xyes" ; then
    echo "failed : for full configuration of GUI module necessary enable VTKViewer !"
    exit
  fi
  if test "x${DISABLE_SALOMEOBJECT}" == "xyes" ; then
    echo "failed : for full configuration of GUI module necessary enable SalomeObject !"
    exit
  fi
  if test "x${DISABLE_QXGRAPHVIEWER}" == "xyes" ; then
    echo "failed : for full configuration of GUI module necessary enable QxGraphViewer !"
    exit
  fi
fi

echo
echo ---------------------------------------------
echo Testing html generators
echo ---------------------------------------------
echo

CHECK_HTML_GENERATORS

echo
echo ---------------------------------------------
echo Testing Kernel
echo ---------------------------------------------
echo
CHECK_KERNEL

echo
echo ---------------------------------------------
echo Testing CPPUNIT only required for unit testing
echo ---------------------------------------------
echo

CHECK_CPPUNIT

echo
echo ---------------------------------------------
echo Testing libxml2
echo ---------------------------------------------
echo

CHECK_LIBXML

echo
echo ---------------------------------------------
echo Summary
echo ---------------------------------------------
echo

if test "$GUI_DISABLE_CORBA" == "yes"; then
   echo "Configure (without Corba)"
else
   echo Configure
fi


if test "$GUI_DISABLE_CORBA" != "yes"; then
variables="cc_ok lex_yacc_ok python_ok swig_ok threads_ok OpenGL_ok qt_ok vtk_ok hdf5_ok
omniORB_ok boost_ok occ_ok sip_ok pyqt_ok qwt_ok doxygen_ok graphviz_ok libxml_ok Kernel_ok"
else
variables="cc_ok lex_yacc_ok python_ok threads_ok OpenGL_ok qt_ok vtk_ok hdf5_ok boost_ok
occ_ok sip_ok pyqt_ok qwt_ok doxygen_ok graphviz_ok Kernel_ok "
fi

for var in $variables
do
   printf "   %10s : " `echo \$var | sed -e "s,_ok,,"`
   eval echo \$$var
done

echo "---Optional:"
variables="cppunit_ok TestRecorder_ok"

for var in $variables
do
   eval toto=\$$var
   if test x$toto != "x"; then
     printf "   %10s : " `echo \$var | sed -e "s,_ok,,"`
     eval echo \$$var
   fi
done

if test "$GUI_DISABLE_CORBA" != "yes"; then
echo
echo "Default ORB   : $DEFAULT_ORB"
echo
fi

dnl We don t need to say when we re entering directories if we re using
dnl GNU make becuase make does it for us.
if test "X$GMAKE" = "Xyes"; then
   AC_SUBST(SETX) SETX=":"
else
   AC_SUBST(SETX) SETX="set -x"
fi

AM_CONDITIONAL(GUI_ENABLE_CORBA, [test "$GUI_DISABLE_CORBA" = no])
AM_CONDITIONAL(ENABLE_PYCONSOLE, [test "$DISABLE_PYCONSOLE" = no])
AM_CONDITIONAL(ENABLE_GLVIEWER, [test "$DISABLE_GLVIEWER" = no])
AM_CONDITIONAL(ENABLE_PLOT2DVIEWER, [test "$DISABLE_PLOT2DVIEWER" = no])
AM_CONDITIONAL(ENABLE_SUPERVGRAPHVIEWER, [test "$DISABLE_SUPERVGRAPHVIEWER" = no])
AM_CONDITIONAL(ENABLE_OCCVIEWER, [test "$DISABLE_OCCVIEWER" = no])
AM_CONDITIONAL(ENABLE_VTKVIEWER, [test "$DISABLE_VTKVIEWER" = no])
AM_CONDITIONAL(ENABLE_SALOMEOBJECT, [test "$DISABLE_SALOMEOBJECT" = no])
AM_CONDITIONAL(ENABLE_QXGRAPHVIEWER, [test "$DISABLE_QXGRAPHVIEWER" = no])
AM_CONDITIONAL(ENABLE_TESTRECORDER, [test "$ENABLE_TESTRECORDER" = yes])

if test "$DISABLE_PYCONSOLE" = "no" ; then
  AC_DEFINE(SUIT_ENABLE_PYTHON)
fi

dnl Build with GUI observers feature
AC_DEFINE(WITH_SALOMEDS_OBSERVER)

echo
echo ---------------------------------------------
echo generating Makefiles and configure files
echo ---------------------------------------------
echo

#AC_OUTPUT_COMMANDS([ \
#  chmod +x ./bin/*; \
#  chmod +x ./bin/salome/*; \
#])

AC_HACK_LIBTOOL
AC_CONFIG_COMMANDS([hack_libtool],[
sed -i "s%^CC=\"\(.*\)\"%hack_libtool (){ \n\
  $(pwd)/hack_libtool \1 \"\$[@]\" \n\
}\n\
CC=\"hack_libtool\"%g" libtool
sed -i "s%\(\s*\)for searchdir in \$newlib_search_path \$lib_search_path \$sys_lib_search_path \$shlib_search_path; do%\1searchdirs=\"\$newlib_search_path \$lib_search_path \$sys_lib_search_path \$shlib_search_path\"\n\1for searchdir in \$searchdirs; do%g" libtool
sed -i "s%\(\s*\)searchdirs=\"\$newlib_search_path \$lib_search_path \(.*\)\"%\1searchdirs=\"\$newlib_search_path \$lib_search_path\"\n\1sss_beg=\"\"\n\1sss_end=\"\2\"%g" libtool
sed -i "s%\(\s*\)\(for searchdir in \$searchdirs; do\)%\1for sss in \$searchdirs; do\n\1  if ! test -d \$sss; then continue; fi\n\1  ssss=\$(cd \$sss; pwd)\n\1  if test \"\$ssss\" != \"\" \&\& test -d \$ssss; then\n\1    case \$ssss in\n\1      /usr/lib | /usr/lib64 ) ;;\n\1      * ) sss_beg=\"\$sss_beg \$ssss\" ;;\n\1    esac\n\1  fi\n\1done\n\1searchdirs=\"\$sss_beg \$sss_end\"\n\1\2%g" libtool
],[])

# This list is initiated using autoscan and must be updated manually
# when adding a new file <filename>.in to manage. When you execute
# autoscan, the Makefile list is generated in the output file configure.scan.
# This could be helpfull to update de configuration.
AC_OUTPUT([ \
  adm_local/Makefile \
  adm_local/cmake_files/Makefile \
  adm_local/unix/Makefile \
  adm_local/unix/config_files/Makefile \
  bin/VERSION \
  bin/Makefile \
  GUI_version.h \
  doc/Makefile \
  doc/salome/Makefile \
  doc/salome/gui/Makefile \
  doc/salome/gui/doxyfile \
  doc/salome/gui/static/header.html \
  doc/salome/tui/Makefile \
  doc/salome/tui/doxyfile \
  doc/salome/tui/static/header.html \
  src/Makefile \
  src/CASCatch/Makefile \
  src/Qtx/Makefile \
  src/Style/Makefile \
  src/DDS/Makefile \
  src/QDS/Makefile \
  src/SUIT/Makefile \
  src/STD/Makefile \
  src/CAF/Makefile \
  src/CAM/Makefile \
  src/SUITApp/Makefile \
  src/LogWindow/Makefile \
  src/ObjBrowser/Makefile \
  src/Prs/Makefile \
  src/OBJECT/Makefile \
  src/ViewerTools/Makefile \
  src/GLViewer/Makefile \
  src/VTKViewer/Makefile \
  src/SVTK/Makefile \
  src/OCCViewer/Makefile \
  src/SOCC/Makefile \
  src/PyInterp/Makefile \
  src/PyConsole/Makefile \
  src/Plot2d/Makefile \
  src/SPlot2d/Makefile \
  src/SUPERVGraph/Makefile \
  src/QxGraph/Makefile \
  src/QxScene/Makefile \
  src/LightApp/Makefile \
  src/ResExporter/Makefile \
  src/TOOLSGUI/Makefile \
  src/Event/Makefile \
  src/OpenGLUtils/Makefile \
  src/Session/Makefile \
  src/SalomeApp/Makefile \
  src/SalomeApp/Test/Makefile \
  src/SalomeApp/pluginsdemo/Makefile \
  src/GuiHelpers/Makefile \
  src/TreeData/Makefile \
  src/TreeData/Test/Makefile \
  src/SALOME_SWIG/Makefile \
  src/SALOME_SWIG/supervisionexample.py \
  src/SALOME_SWIG/supervisiongeomexample.py \
  src/SALOME_PY/Makefile \
  src/SALOME_PYQT/Makefile \
  src/SALOME_PYQT/SALOME_PYQT_GUI/Makefile \
  src/SALOME_PYQT/SALOME_PYQT_GUILight/Makefile \
  src/SALOME_PYQT/SalomePyQt/Makefile \
  src/GUI_PY/Makefile \
  resources/Makefile \
  idl/Makefile \
  tools/Makefile \
  tools/dlgfactory/Makefile \
  tools/vtkEDFOverloads/Makefile \
  Makefile
])

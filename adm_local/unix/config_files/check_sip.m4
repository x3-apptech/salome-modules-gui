dnl Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
dnl
dnl Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
dnl CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2.1 of the License, or (at your option) any later version.
dnl
dnl This library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl Lesser General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public
dnl License along with this library; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
dnl
dnl See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
dnl

AC_DEFUN([CHECK_SIP],[
AC_REQUIRE([CHECK_PYTHON])dnl
AC_REQUIRE([CHECK_QT])dnl
AC_REQUIRE([AC_LINKER_OPTIONS])dnl

sip_ok=yes

AC_ARG_WITH(sip,
    [  --with-sip=EXEC sip executable ],
    [SIP="$withval"
      AC_MSG_RESULT([Try $withval as sip executable])
    ], [
      AC_PATH_PROG(SIP, sip)
    ])

if test "x$SIP" = "x" ; then
    sip_ok=no
    AC_MSG_RESULT(sip is not in the PATH variable)
else
    AC_MSG_CHECKING(whether sip version >= 4.6)
    SIP_VERSION=`$SIP -V | awk '{print $[1]}'`
    SIP_VERSION_ID=`echo $SIP_VERSION | awk -F. '{v=$[1]*10000+$[2]*100+$[3];print v}'`
    if test $SIP_VERSION_ID -ge 40600 ; then 
        AC_MSG_RESULT(yes)
        AC_MSG_RESULT(sip version is $SIP_VERSION)
    else
        AC_MSG_RESULT(no)
        AC_MSG_RESULT(WARNING! sip version $SIP_VERSION is not supported (at least 4.6 is required)!)
        sip_ok=no
    fi
fi

if test "x$sip_ok" = "xyes" ; then
    if test "x$SIPDIR" = "x" ; then
        SIPDIR=`dirname $SIP`
        SIPDIR=`dirname $SIPDIR`
    fi

    TEST_INC_DIRS=""
    TEST_LIB_DIRS=""
    if test "x${SIPDIR}" != "x" ; then
        TEST_INC_DIRS="${TEST_INC_DIRS} ${SIPDIR} ${SIPDIR}/include"
	TEST_INC_DIRS="${TEST_INC_DIRS} ${SIPDIR}/include/python${PYTHON_VERSION}"
        TEST_LIB_DIRS="${TEST_LIB_DIRS} ${SIPDIR} ${SIPDIR}/lib"
	TEST_LIB_DIRS="${TEST_LIB_DIRS} ${SIPDIR}/lib${LIB_LOCATION_SUFFIX}/python${PYTHON_VERSION}/site-packages"
    fi
    if test "x${PYTHONHOME}" != "x" ; then
	TEST_INC_DIRS="${TEST_INC_DIRS} ${PYTHONHOME}/include/python${PYTHON_VERSION}"
	TEST_LIB_DIRS="${TEST_LIB_DIRS} ${PYTHONHOME}/lib${LIB_LOCATION_SUFFIX}/python${PYTHON_VERSION}/site-packages"
    fi
    TEST_INC_DIRS="${TEST_INC_DIRS} /usr/include /usr/include/python${PYTHON_VERSION}"
    TEST_LIB_DIRS="${TEST_LIB_DIRS} /usr/lib${LIB_LOCATION_SUFFIX} /usr/lib${LIB_LOCATION_SUFFIX}/python${PYTHON_VERSION}/site-packages"
    TEST_LIB_DIRS="${TEST_LIB_DIRS} /usr/lib${LIB_LOCATION_SUFFIX}/python${PYTHON_VERSION}/dist-packages"
    TEST_LIB_DIRS="${TEST_LIB_DIRS} /usr/lib${LIB_LOCATION_SUFFIX}/python${PYTHON_VERSION}/dist-packages/PyQt4"
    TEST_LIB_DIRS="${TEST_LIB_DIRS} /usr/lib${LIB_LOCATION_SUFFIX}/pymodules/python${PYTHON_VERSION}"
    if test "${build_cpu::6}" = "x86_64" ; then
      TEST_LIB_DIRS="${TEST_LIB_DIRS} /usr/lib64/python${PYTHON_VERSION}/site-packages"
    fi
    dnl Search sip.h file
    sip_ok=no
    for d in ${TEST_INC_DIRS} ; do
        if test -d $d ; then
            AC_CHECK_FILE(${d}/sip.h,sip_ok=yes,sip_ok=no)
      	    if test "x$sip_ok" == "xyes" ; then
	        SIP_INCLUDES="-I${d}"
                break
            fi
        fi
    done

    if test "x$sip_ok" == "xyes" ; then
        dnl Search sip.so file
        sip_ok=no
        for d in ${TEST_LIB_DIRS} ; do
            if test -d $d ; then
                AC_CHECK_FILE(${d}/sip.so,sip_ok=yes,sip_ok=no)
      	        if test "x$sip_ok" == "xyes" ; then
                    if test "x${d}" = "x/usr/lib${LIB_LOCATION_SUFFIX}" ; then
                        SIP_LIBS=""
                    else
                        SIP_LIBS="-L${d}"
                    fi
                    break
                fi
            fi
        done
    fi
fi

AC_SUBST(SIP)
AC_SUBST(SIP_INCLUDES)
AC_SUBST(SIP_LIBS)
AC_SUBST(SIP_VERSION)
AC_SUBST(SIP_VERSION_ID)

AC_MSG_RESULT(for sip: $sip_ok)

])dnl
dnl

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

AC_DEFUN([CHECK_PYQT],[
AC_REQUIRE([CHECK_PYTHON])dnl
AC_REQUIRE([CHECK_QT])dnl
AC_REQUIRE([CHECK_SIP])dnl
AC_REQUIRE([AC_LINKER_OPTIONS])dnl

dnl AC_LANG_SAVE
dnl AC_LANG_CPLUSPLUS

AC_ARG_WITH(pyqt,
    [  --with-pyqt=DIR      root directory path to PyQt installation ],
    [PYQTDIR="$withval"
      AC_MSG_RESULT([Try $withval as path to the PyQt])
    ])

AC_ARG_WITH(pyqt_sips,
    [  --with-pyqt_sips=DIR      a directory path to PyQt sips installation ],
    [PYQT_SIPS="$withval"
      AC_MSG_RESULT([Try $withval as path to the PyQt sip files])
    ])

AC_ARG_WITH(pyuic4,
    [  --with-pyuic4=EXEC pyuic4 executable ],
    [PYUIC="$withval"
      AC_MSG_RESULT([Try $withval as pyuic4 executable])
    ])

AC_ARG_WITH(pyrcc4,
    [  --with-pyrcc4=EXEC pyrcc4 executable ],
    [PYRCC="$withval"
      AC_MSG_RESULT([Try $withval as pyrcc4 executable])
    ])

AC_CHECKING(for pyqt)

pyqt_ok=no

TEST_BIN_DIRS=""
if test "x${PYQTDIR}" != "x" ; then
    TEST_BIN_DIRS="${TEST_BIN_DIRS} ${PYQTDIR} ${PYQTDIR}/bin"
fi
TEST_BIN_DIRS="${TEST_BIN_DIRS} __CHECK__PATH__"
if test "x${SIPDIR}" != "x" ; then
    TEST_BIN_DIRS="${TEST_BIN_DIRS} ${SIPDIR} ${SIPDIR}/bin"
fi
if test "x${PYTHONHOME}" != "x" ; then
    TEST_BIN_DIRS="${TEST_BIN_DIRS} ${PYTHONHOME}/bin"
fi
TEST_BIN_DIRS="${TEST_BIN_DIRS} /usr/bin"

dnl check pyuic4
if test "x$PYUIC" != "x" ; then
    dnl try $withval value
    AC_CHECK_FILE($PYUIC,pyqt_ok=yes,pyqt_ok=no)
else
    dnl search pyuic4
    pyqt_ok=no
    for d in ${TEST_BIN_DIRS} ; do
        if test "x${d}" = "x__CHECK__PATH__" ; then
            AC_PATH_PROG(TEMP, pyuic4)
            if test "x${TEMP}" != "x" ; then
                PYUIC=${TEMP}
                if test "x$PYQTDIR" = "x" ; then
                    PYQTDIR=`dirname ${PYUIC}`
                    PYQTDIR=`dirname ${PYQTDIR}`
                fi
                pyqt_ok=yes
                break
            fi
        else
            if test -d $d ; then
                AC_CHECK_FILE(${d}/pyuic4,pyqt_ok=yes,pyqt_ok=no)
      	        if test "x$pyqt_ok" == "xyes" ; then
                    PYUIC=${d}/pyuic4
                    break
                fi
            fi
        fi
    done
fi

dnl check PyQt version
if test "x$pyqt_ok" == "xyes" ; then
    AC_MSG_CHECKING(whether PyQt version >= 4.2)
    PYQT_VERSION=`${PYUIC} --version 2>&1 | grep "Python User Interface Compiler"`
    if test "$?" != "0" ; then
        PYQT_VERSION=`${PYUIC} -version 2>&1 | grep "Python User Interface Compiler"`
    fi
    if test "x${PYQT_VERSION}" != "x" ; then
        PYQT_VERSION=`echo $PYQT_VERSION | sed -e 's%[[[:alpha:][:space:]]]*\([[[:digit:].]]*\).*%\1%g'`
        PYQT_VERSION_ID=`echo $PYQT_VERSION | awk -F. '{v=$[1]*10000+$[2]*100+$[3];print v}'`
    else
        PYQT_VERSION="<unknown>"
        PYQT_VERSION_ID=0
    fi
    if test $PYQT_VERSION_ID -ge 40200 ; then 
        AC_MSG_RESULT(yes)
        AC_MSG_RESULT(PyQt version is $PYQT_VERSION)
    else
        AC_MSG_RESULT(no)
        AC_MSG_RESULT(WARNING! PyQt version $PYQT_VERSION is not supported (at least 4.2 is required)!)
        pyqt_ok=no
    fi
else
    AC_MSG_RESULT(Warning! pyuic4 is not found!)
fi

dnl check pyrcc4
if test "x$PYRCC" != "x" ; then
    dnl try $withval value
    AC_CHECK_FILE($PYRCC,pyqt_ok=yes,pyqt_ok=no)
else
    dnl search pyrcc4
    pyqt_ok=no
    for d in ${TEST_BIN_DIRS} ; do
        if test "x${d}" = "x__CHECK__PATH__" ; then
            AC_PATH_PROG(TEMP, pyrcc4)
            if test "x${TEMP}" != "x" ; then
                PYRCC=${TEMP}
                if test "x$PYQTDIR" = "x" ; then
                    PYQTDIR=`dirname ${PYRCC}`
                    PYQTDIR=`dirname ${PYQTDIR}`
                fi
                pyqt_ok=yes
                break
            fi
        else
            if test -d $d ; then
                AC_CHECK_FILE(${d}/pyrcc4,pyqt_ok=yes,pyqt_ok=no)
      	        if test "x$pyqt_ok" == "xyes" ; then
                    PYRCC=${d}/pyrcc4
                    break
                fi
            fi
        fi
    done
fi

if test "x$pyqt_ok" == "xyes" ; then
    TESTLIBFILE=QtCore.so
    TESTSIPFILE=QtCore/QtCoremod.sip
    TEST_LIB_DIRS=""
    TEST_SIPS_DIRS=""
    if test "x${PYQT_SIPS}" != "x" ; then
        TEST_SIPS_DIRS="${TEST_SIPS_DIRS} ${PYQT_SIPS}"
    fi
    if test "x${PYQTDIR}" != "x" ; then
        TEST_LIB_DIRS="${TEST_LIB_DIRS} ${PYQTDIR} ${PYQTDIR}/lib ${PYQTDIR}/PyQt4"
        TEST_LIB_DIRS="${TEST_LIB_DIRS} ${PYQTDIR}/lib${LIB_LOCATION_SUFFIX}/python${PYTHON_VERSION}/site-packages"
        TEST_LIB_DIRS="${TEST_LIB_DIRS} ${PYQTDIR}/lib${LIB_LOCATION_SUFFIX}/python${PYTHON_VERSION}/site-packages/PyQt4"
        TEST_SIPS_DIRS="${TEST_SIPS_DIRS} ${PYQTDIR} ${PYQTDIR}/sip"
	TEST_SIPS_DIRS="${TEST_SIPS_DIRS} ${PYQTDIR}/share ${PYQTDIR}/share/sip"
    fi
    if test "x${SIPDIR}" != "x" ; then
        TEST_LIB_DIRS="${TEST_LIB_DIRS} ${SIPDIR} ${SIPDIR}/lib ${SIPDIR}/PyQt4"
	TEST_LIB_DIRS="${TEST_LIB_DIRS} ${SIPDIR}/lib${LIB_LOCATION_SUFFIX}/python${PYTHON_VERSION}/site-packages"
	TEST_LIB_DIRS="${TEST_LIB_DIRS} ${SIPDIR}/lib${LIB_LOCATION_SUFFIX}/python${PYTHON_VERSION}/site-packages/PyQt4"
        TEST_SIPS_DIRS="${TEST_SIPS_DIRS} ${SIPDIR} ${SIPDIR}/sip"
        TEST_SIPS_DIRS="${TEST_SIPS_DIRS} ${SIPDIR}/share ${SIPDIR}/share/sip ${SIPDIR}/share/sip/PyQt4"
    fi
    if test "x${PYTHONHOME}" != "x" ; then
        TEST_LIB_DIRS="${TEST_LIB_DIRS} ${PYTHONHOME}/lib${LIB_LOCATION_SUFFIX}/python${PYTHON_VERSION}/site-packages"
        TEST_LIB_DIRS="${TEST_LIB_DIRS} ${PYTHONHOME}/lib${LIB_LOCATION_SUFFIX}/python${PYTHON_VERSION}/site-packages/PyQt4"
    fi
    TEST_LIB_DIRS="${TEST_LIB_DIRS} /usr/lib${LIB_LOCATION_SUFFIX} /usr/lib${LIB_LOCATION_SUFFIX}/python${PYTHON_VERSION}/site-packages"
    TEST_LIB_DIRS="${TEST_LIB_DIRS} /usr/lib${LIB_LOCATION_SUFFIX}/python${PYTHON_VERSION}/site-packages/PyQt4"
    TEST_LIB_DIRS="${TEST_LIB_DIRS} /usr/lib${LIB_LOCATION_SUFFIX}/python${PYTHON_VERSION}/dist-packages/PyQt4"
    TEST_LIB_DIRS="${TEST_LIB_DIRS} /usr/lib${LIB_LOCATION_SUFFIX}/pymodules/python${PYTHON_VERSION}/PyQt4"
    if test "${build_cpu::6}" = "x86_64" ; then
      TEST_LIB_DIRS="${TEST_LIB_DIRS} /usr/lib64/python2.6/site-packages/PyQt4"
    fi
    TEST_SIPS_DIRS="${TEST_SIPS_DIRS} /usr/share/sip"
    TEST_SIPS_DIRS="${TEST_SIPS_DIRS} /usr/share/sip/PyQt4"

    dnl check PyQt libs
    pyqt_ok=no
    for d in ${TEST_LIB_DIRS} ; do
        if test -d $d ; then
            AC_CHECK_FILE(${d}/${TESTLIBFILE},pyqt_ok=yes,pyqt_ok=no)
            if test "x$pyqt_ok" == "xyes" ; then
                if test "x${d}" = "x/usr/lib${LIB_LOCATION_SUFFIX}" ; then
                    PYQT_LIBS=""
                else
                    PYQT_LIBS="-L${d}"
                fi
                break
            fi
        fi
    done

    dnl check PyQt sips
    if test "x$pyqt_ok" == "xyes" ; then
        pyqt_ok=no
        for d in ${TEST_SIPS_DIRS} ; do
            if test -d $d ; then
                AC_CHECK_FILE(${d}/${TESTSIPFILE},pyqt_ok=yes,pyqt_ok=no)
                if test "x$pyqt_ok" == "xyes" ; then
                    PYQT_SIPS=${d}
                    PYQT_INCLUDES="-I ${d}"
                    PYQT_INCLUDES="${PYQT_INCLUDES} -I ${d}/QtCore -I ${d}/QtGui"
                    PYQT_INCLUDES="${PYQT_INCLUDES} -I ${d}/QtXml -I ${d}/QtOpenGL"
                    PYQT_INCLUDES="${PYQT_INCLUDES} -I ${d}/QtAssistant -I ${d}/QtDesigner"
                    PYQT_INCLUDES="${PYQT_INCLUDES} -I ${d}/QtNetwork -I ${d}/QtSql"
                    PYQT_INCLUDES="${PYQT_INCLUDES} -I ${d}/QtSvg -I ${d}/QtTest"

                    # check compatibility with Qt
                    #SUPPORTED=`grep -e "[[[:space:]]]*Qt_[[[:digit:]_]]\+}" ${PYQT_SIPS}/QtCore/QtCoremod.sip | sed -e "s/\(.*\)[[[:space:]]]*\(Qt_[[[:digit:]_]]\+\)}/\2/g"`
                    #SUPPORTED=`echo $SUPPORTED | sed -e "s/Qt_//g" -e "s/_/./g"`
                    #SUPPORTED_ID=`echo $SUPPORTED | awk -F. '{v=$[1]*10000+$[2]*100+$[3];print v}'`
                    #if test $SUPPORTED_ID -lt $QT_VERSION_ID ; then 
                    #    AC_MSG_RESULT(Warning! Used Qt version ($QT_VERSION) is not supported by PyQt)
                    #    AC_MSG_RESULT(Latest supported Qt version is ${SUPPORTED})
                    #else
                    #    SUPPORTED=${QT_VERSION}
                    #fi
                    #SUPPORTED="Qt_`echo ${SUPPORTED} | sed -e 's/\./_/g'`"
		    PYQT_SIPFLAGS=`python -c "from PyQt4 import pyqtconfig; print pyqtconfig.Configuration().pyqt_sip_flags"`
                    PYQT_SIPFLAGS="${PYQT_SIPFLAGS} -s .cc -c . ${PYQT_INCLUDES}"
                    break
                fi
            fi
        done
    fi
fi

AC_SUBST(PYQT_INCLUDES)
AC_SUBST(PYQT_LIBS)
AC_SUBST(PYQT_SIPS)
AC_SUBST(PYUIC)
AC_SUBST(PYRCC)
AC_SUBST(PYQT_SIPFLAGS)

dnl AC_LANG_RESTORE

AC_MSG_RESULT(for pyqt: $pyqt_ok)

])dnl
dnl


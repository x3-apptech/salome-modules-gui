#  Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
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
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#  File   : Makefile.in
#  Author : Vladimir Klyachin (OCN)
#  Module : Plot2d
#  $Header$

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES= libPlot2d.la

# header files 
salomeinclude_HEADERS= \
	Plot2d.h \
	Plot2d_Curve.h \
	Plot2d_FitDataDlg.h \
	Plot2d_Prs.h \
	Plot2d_SetupViewDlg.h \
	Plot2d_ViewFrame.h \
	Plot2d_ViewManager.h \
	Plot2d_ViewModel.h \
	Plot2d_ViewWindow.h \
	Plot2d_SetupCurveDlg.h \
	Plot2d_ToolTip.h

dist_libPlot2d_la_SOURCES= \
	Plot2d_Curve.cxx \
	Plot2d_FitDataDlg.cxx \
	Plot2d_Prs.cxx \
	Plot2d_SetupViewDlg.cxx \
	Plot2d_ViewFrame.cxx \
	Plot2d_ViewManager.cxx \
	Plot2d_ViewModel.cxx \
	Plot2d_ViewWindow.cxx \
	Plot2d_SetupCurveDlg.cxx \
	Plot2d_ToolTip.cxx

MOC_FILES = \
	Plot2d_FitDataDlg_moc.cxx \
	Plot2d_SetupViewDlg_moc.cxx \
	Plot2d_ViewFrame_moc.cxx \
	Plot2d_ViewManager_moc.cxx \
	Plot2d_ViewModel_moc.cxx \
	Plot2d_ViewWindow_moc.cxx \
	Plot2d_SetupCurveDlg_moc.cxx \
	Plot2d_ToolTip_moc.cxx
nodist_libPlot2d_la_SOURCES= $(MOC_FILES)

dist_salomeres_DATA = \
	resources/plot2d_clone.png \
	resources/plot2d_camera_dump.png \
	resources/plot2d_fitall.png \
	resources/plot2d_fitarea.png \
	resources/plot2d_glpan.png \
	resources/plot2d_legend.png \
	resources/plot2d_linear.png \
	resources/plot2d_linear_y.png \
	resources/plot2d_lines.png \
	resources/plot2d_log.png \
	resources/plot2d_log_y.png \
	resources/plot2d_pan.png \
	resources/plot2d_points.png \
	resources/plot2d_settings.png \
	resources/plot2d_splines.png \
	resources/plot2d_zoom.png

nodist_salomeres_DATA= \
	Plot2d_images.qm \
	Plot2d_msg_en.qm 

libPlot2d_la_CPPFLAGS=$(QT_INCLUDES) $(PYTHON_INCLUDES) $(QWT_INCLUDES) \
	-I$(srcdir)/../Qtx -I$(srcdir)/../SUIT

libPlot2d_la_LDFLAGS=$(QWT_LIBS) $(QT_MT_LIBS) ../SUIT/libsuit.la



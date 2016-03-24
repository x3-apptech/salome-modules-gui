// Copyright (C) 2015-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef OCCVIEWER_RAYTRACINGDLG_H
#define OCCVIEWER_RAYTRACINGDLG_H

#include "OCCViewer.h"
#include <QDialog>
#include <V3d_View.hxx>

class OCCViewer_ViewWindow;
class OCCViewer_ViewFrame;
class QCheckBox;
class QtxIntSpinBox;
class QGroupBox;


class OCCVIEWER_EXPORT OCCViewer_RayTracingDlg : public QDialog
{
  Q_OBJECT

  public:
  OCCViewer_RayTracingDlg( OCCViewer_ViewWindow* );
  ~OCCViewer_RayTracingDlg();

  static QString getName();

private slots:

  void onValueChanged();
  void onRayTracing(bool);

  void ClickOnHelp();

private:
  void initParam();

  OCCViewer_ViewFrame* myViewFrame;
  Handle(V3d_View)     myView3d;

  QGroupBox*     myRayTracingGroup;
  QtxIntSpinBox* myDepth;
  QCheckBox*     myReflection;
  QCheckBox*     myAntialiasing;
  QCheckBox*     myShadow;
  QCheckBox*     myTransparentShadow;
};

#endif // OCCVIEWER_RAYTRACINGDLG_H

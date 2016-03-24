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

#ifndef OCCVIEWER_LIGHTSOURCEDLG_H
#define OCCVIEWER_LIGHTSOURCEDLG_H

#include "OCCViewer.h"
#include <QDialog>
#include <V3d_DirectionalLight.hxx>
#include <V3d_PositionalLight.hxx>
#include <V3d_TypeOfLight.hxx>

class OCCViewer_ViewWindow;
class OCCViewer_Viewer;
class QtxDoubleSpinBox;
class QtxColorButton;
class QPushButton;
class QCheckBox;
class QRadioButton;
class QStackedLayout;

class OCCVIEWER_EXPORT OCCViewer_LightSourceDlg: public QDialog
{
  Q_OBJECT

public:
  OCCViewer_LightSourceDlg( OCCViewer_ViewWindow*, OCCViewer_Viewer* );
  ~OCCViewer_LightSourceDlg();

  static QString getName();

protected slots:

private slots:
  void onTypeChanged();
  void onDirChanged();
  void onPosChanged();

  void ClickOnOk();
  void ClickOnDefault();
  void ClickOnClose();
  void ClickOnHelp();

private:
  void initParam( bool theIsDefault = false );

  OCCViewer_Viewer* myModel;

  Handle(V3d_DirectionalLight) myDirLight;
  Handle(V3d_PositionalLight) myPosLight;

  QRadioButton* myDirType;
  QRadioButton* myPosType;

  QStackedLayout* myStackedLayout;

  QtxDoubleSpinBox* myDx;
  QtxDoubleSpinBox* myDy;
  QtxDoubleSpinBox* myDz;
  QCheckBox* myDirHeadLight;
  QtxColorButton* myDirColor;

  QtxDoubleSpinBox* myX;
  QtxDoubleSpinBox* myY;
  QtxDoubleSpinBox* myZ;
  QCheckBox* myPosHeadLight;
  QtxColorButton* myPosColor;

  double myInX, myInY, myInZ;
  bool myInHeadLight;
  Quantity_Color myInColor;
  V3d_TypeOfLight myInType;

  bool isBusy;
};

#endif // OCCVIEWER_LIGHTSOURCEDLG_H

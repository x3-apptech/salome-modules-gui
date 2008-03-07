// Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful
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
#ifndef OCCVIEWER_SETROTATIONPOINTDLG_H
#define OCCVIEWER_SETROTATIONPOINTDLG_H

#include "OCCViewer.h"

#include <qdialog.h>

class QtxAction;

class QLineEdit;
class QPushButton;
class QGroupBox;
class QButtonGroup;
class QCheckBox;

class OCCViewer_ViewWindow;

class OCCVIEWER_EXPORT OCCViewer_SetRotationPointDlg : public QDialog
{
  Q_OBJECT

public:
  OCCViewer_SetRotationPointDlg(OCCViewer_ViewWindow* , QWidget* parent = 0, 
				const char* name = 0, bool modal = FALSE, WFlags fl = 0);
  ~OCCViewer_SetRotationPointDlg();

  void SetAction( QtxAction* theAction ) { myAction = theAction; }
  bool IsFirstShown();

  void setCoords(double theX=0., double theY=0., double theZ=0.);
  void toggleChange();

protected:
  OCCViewer_ViewWindow* myView;
  QtxAction* myAction;

  QCheckBox*    myIsBBCenter;

  QButtonGroup* myGroupSelButton;
  QPushButton*  myToOrigin;
  QPushButton*  mySelectPoint;

  QGroupBox* myGroupBoxCoord;
  QLineEdit* myX;
  QLineEdit* myY;
  QLineEdit* myZ;

  void setEnabled(QGroupBox* theGrp, const bool theState);

protected slots:
  void onBBCenterChecked();
  
  void onToOrigin();
  void onSelectPoint();

  void onCoordChanged();

  void onClickClose();

  void onViewShow();
  void onViewHide();
};

#endif // OCCVIEWER_SETROTATIONPOINTDLG_H

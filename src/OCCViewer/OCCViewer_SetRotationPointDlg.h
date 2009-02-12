//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef OCCVIEWER_SETROTATIONPOINTDLG_H
#define OCCVIEWER_SETROTATIONPOINTDLG_H

#include "OCCViewer.h"

#include <QDialog>

class QtxAction;

class QLineEdit;
class QPushButton;
class QGroupBox;
class QCheckBox;

class OCCViewer_ViewWindow;

class OCCVIEWER_EXPORT OCCViewer_SetRotationPointDlg : public QDialog
{
  Q_OBJECT

public:
  OCCViewer_SetRotationPointDlg(OCCViewer_ViewWindow* , QWidget* parent = 0, 
				const char* name = 0, bool modal = FALSE, Qt::WindowFlags fl = 0);
  ~OCCViewer_SetRotationPointDlg();

  void SetAction( QtxAction* theAction ) { myAction = theAction; }
  bool IsFirstShown();

  void setCoords(double theX=0., double theY=0., double theZ=0.);
  void toggleChange();

protected:
  OCCViewer_ViewWindow* myView;
  QtxAction* myAction;

  QCheckBox*    myIsBBCenter;

  QGroupBox   * myGroupBoxSel;
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

protected:
  void closeEvent( QCloseEvent* );
};

#endif // OCCVIEWER_SETROTATIONPOINTDLG_H

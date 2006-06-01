//  SALOME VTKViewer : build VTK viewer into Salome desktop
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : 
//  Author : 
//  Module : SALOME
//  $Header$

#ifndef SVTK_NONISOMETRICDLG_H
#define SVTK_NONISOMETRICDLG_H

#include "SVTK_DialogBase.h"

class SVTK_MainWindow;

class QtxDblSpinBox;
class QtxAction;

class QPushButton;


class SVTK_NonIsometricDlg : public SVTK_DialogBase
{
  Q_OBJECT;

public:
  SVTK_NonIsometricDlg(QtxAction* theAction,
		       SVTK_MainWindow* theParent,
		       const char* theName);

  ~SVTK_NonIsometricDlg();

  void Update();

protected:
  SVTK_MainWindow *m_MainWindow;

  QtxDblSpinBox* m_sbXcoeff;
  QtxDblSpinBox* m_sbYcoeff;
  QtxDblSpinBox* m_sbZcoeff;
  QPushButton* m_bReset;

protected slots:
  void onClickApply();
  void onClickReset();
  void onClickOk();
  void onClickClose();
};

#endif // SVTK_NONISOMETRICDLG_H

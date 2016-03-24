// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

//  SALOME VTKViewer : build VTK viewer into Salome desktop
//  File   : 
//  Author : 

#ifndef SVTK_NONISOMETRICDLG_H
#define SVTK_NONISOMETRICDLG_H

#include <ViewerTools_DialogBase.h>

class SVTK_ViewWindow;

class QtxDoubleSpinBox;
class QtxAction;

class QPushButton;


class SVTK_NonIsometricDlg : public ViewerTools_DialogBase
{
  Q_OBJECT;

public:
  SVTK_NonIsometricDlg(QtxAction* theAction,
                       SVTK_ViewWindow* theParent,
                       const char* theName);

  ~SVTK_NonIsometricDlg();

  void Update();

protected:
  SVTK_ViewWindow *m_MainWindow;

  QtxDoubleSpinBox* m_sbXcoeff;
  QtxDoubleSpinBox* m_sbYcoeff;
  QtxDoubleSpinBox* m_sbZcoeff;
  QPushButton* m_bReset;

protected slots:
  void onClickApply();
  void onClickReset();
  void onClickOk();
  void onClickClose();
};

#endif // SVTK_NONISOMETRICDLG_H

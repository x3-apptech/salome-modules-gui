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

//  File   : SVTK_CubeAxesDlg.h
//  Author : Sergey LITONIN
//  Module : VISU
//
#ifndef SVTK_CubeAxesDlg_H
#define SVTK_CubeAxesDlg_H

#include "SVTK.h"

#include <ViewerTools_CubeAxesDlgBase.h>

class SVTK_ViewWindow;
class SVTK_CubeAxesActor2D;

class vtkAxisActor2D;

/*!
 * Class       : SVTK_AxisWidget
 * Description : Axis tab widget of the "Graduated axis" dialog box
*/
class SVTK_EXPORT SVTK_AxisWidget : public ViewerTools_AxisWidgetBase
{
public:
  SVTK_AxisWidget( QWidget* );
  ~SVTK_AxisWidget();

public:
  bool             ReadData( vtkAxisActor2D* );
  bool             Apply( vtkAxisActor2D* );

protected:
  virtual ViewerTools_FontWidgetBase* createFontWidget( QWidget* );
};

/*!
 * Class       : SVTK_CubeAxesDlg
 * Description : Dialog for specifynig cube axes properties
 */
class SVTK_EXPORT SVTK_CubeAxesDlg : public ViewerTools_CubeAxesDlgBase
{
  Q_OBJECT

public:
                  SVTK_CubeAxesDlg(QtxAction* theAction,
                                   SVTK_ViewWindow* theParent,
                                   const char* theName);
  virtual         ~SVTK_CubeAxesDlg();

  virtual void    Update();

private slots:
  virtual bool    onApply();

private:
  virtual ViewerTools_AxisWidgetBase* createAxisWidget( QWidget* );

private:
  SVTK_ViewWindow*      myMainWindow;
  SVTK_CubeAxesActor2D* myActor;
};

#endif

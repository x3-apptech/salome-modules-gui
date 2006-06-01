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

#ifndef SVTK_DIALOGBASE_H
#define SVTK_DIALOGBASE_H

#include "SVTK.h"

#include <qdialog.h>

class QtxAction;

class SVTK_EXPORT SVTK_DialogBase : public QDialog
{
  Q_OBJECT;

public:
  SVTK_DialogBase(QtxAction* theAction,
		  QWidget* theParent,
		  const char* theName = "", 
		  bool theModal = FALSE, 
		  WFlags theWFalgs = 0);

  ~SVTK_DialogBase();

protected slots:
  void onParentShow();
  void onParentHide();
  virtual void done( int );

protected:
  QtxAction* myAction;
};

#endif // SVTK_DIALOGBASE_H

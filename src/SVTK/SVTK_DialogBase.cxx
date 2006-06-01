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

#include "SVTK_DialogBase.h"

#include "QtxAction.h"

/*!
  Constructor
*/
SVTK_DialogBase
::SVTK_DialogBase(QtxAction* theAction,
		  QWidget* theParent,
		  const char* theName, 
		  bool theModal, 
		  WFlags theWFalgs):
  QDialog(theParent, 
	  theName, 
	  theModal, 
	  theWFalgs | WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu),
  myAction(theAction)
{
  connect(theParent, SIGNAL(Show( QShowEvent * )), this, SLOT(onParentShow()));
  connect(theParent, SIGNAL(Hide( QHideEvent * )), this, SLOT(onParentHide()));
}

/*
 *  Destroys the object and frees any allocated resources
 */
SVTK_DialogBase
::~SVTK_DialogBase()
{
  // no need to delete child widgets, Qt does it all for us
}

void 
SVTK_DialogBase
::onParentShow()
{
  if(myAction->isOn())
    show();
  else
    hide();
}

void 
SVTK_DialogBase
::onParentHide()
{
  hide();
}

void 
SVTK_DialogBase
::done( int r )
{
  myAction->setOn( false );
  QDialog::done( r );
}

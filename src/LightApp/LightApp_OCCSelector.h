// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
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
#ifndef LIGHTAPP_OCCSELECTOR_H
#define LIGHTAPP_OCCSELECTOR_H

#include "LightApp.h"

#include <SUIT_Selector.h>
#include <SUIT_DataOwner.h>

#include <OCCViewer_ViewModel.h>

class Handle_AIS_InteractiveObject;

/*!
  \class LightApp_OCCSelector
  Custom selector to get/set selection from OCC viewer
*/
class LIGHTAPP_EXPORT LightApp_OCCSelector : public SUIT_Selector
{
  Q_OBJECT

public:
  LightApp_OCCSelector( OCCViewer_Viewer*, SUIT_SelectionMgr* );
  virtual ~LightApp_OCCSelector();

  OCCViewer_Viewer* viewer() const;

  virtual QString   type() const { return OCCViewer_Viewer::Type(); }

private slots:
  virtual void      onSelectionChanged();
  virtual void      onDeselection();

protected:
  virtual void      getSelection( SUIT_DataOwnerPtrList& ) const;
  virtual void      setSelection( const SUIT_DataOwnerPtrList& );

  QString           entry( const Handle_AIS_InteractiveObject& ) const;

  SUIT_DataOwnerPtrList mySelectedExternals;

private:
  OCCViewer_Viewer*     myViewer;
};

#endif

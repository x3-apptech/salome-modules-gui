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

#ifndef LIGHTAPP_OCCSELECTOR_H
#define LIGHTAPP_OCCSELECTOR_H

#include "LightApp.h"

#include <SUIT_Selector.h>
#include <SUIT_DataOwner.h>

#include <QObject>

#ifndef DISABLE_OCCVIEWER
#include <OCCViewer_ViewModel.h>

#include <AIS_InteractiveObject.hxx>
#endif

/*!
  \class LightApp_OCCSelector
  Custom selector to get/set selection from OCC viewer
*/
class LIGHTAPP_EXPORT LightApp_OCCSelector : public QObject, public SUIT_Selector
{
  Q_OBJECT

public:
#ifndef DISABLE_OCCVIEWER
  LightApp_OCCSelector( OCCViewer_Viewer*, SUIT_SelectionMgr* );
#else
  LightApp_OCCSelector( SUIT_SelectionMgr* );
#endif
  virtual ~LightApp_OCCSelector();

#ifndef DISABLE_OCCVIEWER
  OCCViewer_Viewer* viewer() const;

  virtual QString   type() const { return OCCViewer_Viewer::Type(); }
#endif

private slots:
  virtual void      onSelectionChanged();
  virtual void      onDeselection();

protected:
  virtual void      getSelection( SUIT_DataOwnerPtrList& ) const;
  virtual void      setSelection( const SUIT_DataOwnerPtrList& );

#ifndef DISABLE_OCCVIEWER
  virtual QString   entry( const Handle_AIS_InteractiveObject& ) const;
#endif

  SUIT_DataOwnerPtrList mySelectedExternals;

private:
#ifndef DISABLE_OCCVIEWER
  OCCViewer_Viewer*     myViewer;
#endif
};

#endif

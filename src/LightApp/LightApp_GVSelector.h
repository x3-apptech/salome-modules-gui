// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef LIGHTAPP_GVSELECTOR_H
#define LIGHTAPP_GVSELECTOR_H

#include "LightApp.h"

#include <QObject>
#include <SUIT_Selector.h>
#include <SUIT_DataOwner.h>
#include <GraphicsView_Defs.h>

class GraphicsView_Viewer;

class LIGHTAPP_EXPORT LightApp_GVSelector : public QObject, public SUIT_Selector
{
  Q_OBJECT

public:
  LightApp_GVSelector( GraphicsView_Viewer*, SUIT_SelectionMgr* );
  virtual ~LightApp_GVSelector();

  virtual QString type() const;

protected:
  virtual void getSelection( SUIT_DataOwnerPtrList& ) const;
  virtual void setSelection( const SUIT_DataOwnerPtrList& );

protected slots:
  void onSelectionChanged( GV_SelectionChangeStatus );

protected:
  GraphicsView_Viewer* myViewer;
};

#endif

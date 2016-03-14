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

#ifndef LIGHTAPP_GLSELECTOR_H
#define LIGHTAPP_GLSELECTOR_H

#include "LightApp.h"

#include <SUIT_Selector.h>

#include <GLViewer_Viewer2d.h>

#include <QObject>

class LightApp_DataObject;

/*!
  \class LightApp_GLSelector
  Custom selector to get/set selection from GL viewer
*/
class LIGHTAPP_EXPORT LightApp_GLSelector : public QObject, public SUIT_Selector
{
  Q_OBJECT

public:
  LightApp_GLSelector( GLViewer_Viewer2d*, SUIT_SelectionMgr* );
  virtual ~LightApp_GLSelector();

  GLViewer_Viewer2d* viewer() const;

  virtual QString type() const { return GLViewer_Viewer2d::Type(); }

private slots:
  void                onSelectionChanged();

protected:
  virtual void        getSelection( SUIT_DataOwnerPtrList& ) const;
  virtual void        setSelection( const SUIT_DataOwnerPtrList& );

private:
  GLViewer_Viewer2d*  myViewer;
};


#endif

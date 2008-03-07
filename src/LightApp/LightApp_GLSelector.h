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
#ifndef LIGHTAPP_GLSELECTOR_H
#define LIGHTAPP_GLSELECTOR_H

#include "LightApp.h"

#include <SUIT_Selector.h>

#include <GLViewer_Viewer2d.h>

#include <string>
#include <GLViewer_Object.h>

class LightApp_DataObject;

/*!
  \class LightApp_GLSelector
  Custom selector to get/set selection from GL viewer
*/
class LIGHTAPP_EXPORT LightApp_GLSelector : public SUIT_Selector
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

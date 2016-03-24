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

#ifndef LIGHTAPP_VTKSELECTOR_H
#define LIGHTAPP_VTKSELECTOR_H

#include "SUIT_Selector.h"
#include "LightApp.h"
#include "LightApp_DataOwner.h"

#include <QObject>

class SUIT_Desktop;

#ifndef DISABLE_VTKVIEWER
  #include <TColStd_IndexedMapOfInteger.hxx>
  #include "SVTK_Selection.h"
#ifndef DISABLE_SALOMEOBJECT
  #include "SALOME_InteractiveObject.hxx"
#endif
  class SALOME_Actor;
  class SVTK_ViewWindow;
  class SVTK_ViewModelBase;
#endif

/*!
  Provide salome vtk data owner list.
*/
class LIGHTAPP_EXPORT LightApp_SVTKDataOwner : public LightApp_DataOwner
{
 public:
   virtual ~LightApp_SVTKDataOwner() {};
#ifndef DISABLE_VTKVIEWER
   #ifndef DISABLE_SALOMEOBJECT
    LightApp_SVTKDataOwner( const Handle(SALOME_InteractiveObject)&, SUIT_Desktop* );
   #else
    LightApp_SVTKDataOwner( const QString& );
   #endif

    /*!Gets dataowners ids list.*/
    const TColStd_IndexedMapOfInteger& GetIds() const;

    /*!Gets selection mode.*/
    Selection_Mode GetMode() const;

    /*!Finds corresponding actor in the active viewer.*/
    SALOME_Actor* GetActor() const;

 protected:
    mutable TColStd_IndexedMapOfInteger myIds;

    SVTK_ViewWindow* GetActiveViewWindow() const;
    SUIT_Desktop* myDesktop;
#endif
};


/*!
  \class LightApp_VTKSelector
  Custom selector to get/set selection from object browser
*/
class LIGHTAPP_EXPORT LightApp_VTKSelector : public QObject, public SUIT_Selector
{
  Q_OBJECT;

public:
  virtual ~LightApp_VTKSelector() {};
#ifndef DISABLE_VTKVIEWER
  LightApp_VTKSelector( SVTK_ViewModelBase*, SUIT_SelectionMgr* );

  SVTK_ViewModelBase* viewer() const;

  virtual QString type() const;
#else
  LightApp_VTKSelector( SUIT_SelectionMgr* );
#endif

private slots:
  void              onSelectionChanged();

#ifndef DISABLE_VTKVIEWER
protected:
  virtual void      getSelection( SUIT_DataOwnerPtrList& ) const;
  virtual void      setSelection( const SUIT_DataOwnerPtrList& );

private:
  SVTK_ViewModelBase* myViewer;

#endif
};

#endif

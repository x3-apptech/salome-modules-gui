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
// See http://www.salome-platform.org/
//
#ifndef LIGHTAPP_VTKSELECTOR_H
#define LIGHTAPP_VTKSELECTOR_H

#include <vtkSmartPointer.h>

#include <TColStd_IndexedMapOfInteger.hxx>

#include "SUIT_Selector.h"

#include "LightApp.h"
#include "LightApp_DataOwner.h"

#include "SVTK_Selection.h"
#include "SALOME_InteractiveObject.hxx"

class SALOME_Actor;
class SVTK_ViewModelBase;

/*!
  Provide salome vtk data owner list.
*/
class LIGHTAPP_EXPORT LightApp_SVTKDataOwner : public LightApp_DataOwner
{
 public:
    LightApp_SVTKDataOwner( const Handle(SALOME_InteractiveObject)& theIO,
                            const TColStd_IndexedMapOfInteger& theIds,
                            Selection_Mode theMode = ActorSelection,
                            SALOME_Actor* theActor = NULL);
    virtual ~LightApp_SVTKDataOwner();

    /*!Gets dataowners ids list.*/
    const TColStd_IndexedMapOfInteger& GetIds() const
    {
      return myIds;
    }

    /*!Gets selection mode.*/
    Selection_Mode GetMode() const 
    { 
      return mySelectionMode; 
    }

    SALOME_Actor* GetActor() const;

 protected:
    TColStd_IndexedMapOfInteger myIds;
    Selection_Mode mySelectionMode;
    vtkSmartPointer<SALOME_Actor> myActor;
};


/*!
  Provide vtk selection of data owners.
*/
class LIGHTAPP_EXPORT LightApp_VTKSelector : public SUIT_Selector
{
  Q_OBJECT;

public:
  LightApp_VTKSelector( SVTK_ViewModelBase*, SUIT_SelectionMgr* );
  virtual ~LightApp_VTKSelector();

  SVTK_ViewModelBase* viewer() const;

  virtual QString type() const;

private slots:
  void              onSelectionChanged();

protected:
  virtual void      getSelection( SUIT_DataOwnerPtrList& ) const;
  virtual void      setSelection( const SUIT_DataOwnerPtrList& );

private:
  SVTK_ViewModelBase* myViewer;
};

#endif

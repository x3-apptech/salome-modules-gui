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
class SVTK_Viewer;

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
  LightApp_VTKSelector( SVTK_Viewer*, SUIT_SelectionMgr* );
  virtual ~LightApp_VTKSelector();

  SVTK_Viewer* viewer() const;

  virtual QString type() const;

private slots:
  void              onSelectionChanged();

protected:
  virtual void      getSelection( SUIT_DataOwnerPtrList& ) const;
  virtual void      setSelection( const SUIT_DataOwnerPtrList& );

private:
  SVTK_Viewer* myViewer;
};

#endif

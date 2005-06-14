#ifndef SALOMEAPP_VTKSELECTOR_H
#define SALOMEAPP_VTKSELECTOR_H

#include <vtkSmartPointer.h>

#include <TColStd_IndexedMapOfInteger.hxx>

#include "SUIT_Selector.h"

#include "SalomeApp.h"
#include "SalomeApp_DataOwner.h"

#include "SVTK_Selection.h"
#include "SALOME_InteractiveObject.hxx"

class SALOME_Actor;
class SVTK_Viewer;

class SalomeApp_SVTKDataOwner : public SalomeApp_DataOwner
{
 public:
    SalomeApp_SVTKDataOwner( const Handle(SALOME_InteractiveObject)& theIO,
			     const TColStd_IndexedMapOfInteger& theIds,
			     Selection_Mode theMode = ActorSelection,
			     SALOME_Actor* theActor = NULL);
    virtual ~SalomeApp_SVTKDataOwner();

    const TColStd_IndexedMapOfInteger& GetIds() const
    {
      return myIds;
    }

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


class SALOMEAPP_EXPORT SalomeApp_VTKSelector : public SUIT_Selector
{
  Q_OBJECT;

public:
  SalomeApp_VTKSelector( SVTK_Viewer*, SUIT_SelectionMgr* );
  virtual ~SalomeApp_VTKSelector();

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

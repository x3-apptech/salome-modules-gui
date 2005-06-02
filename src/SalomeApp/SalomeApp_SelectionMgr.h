#ifndef SALOMEAPP_SELECTIONMGR_H
#define SALOMEAPP_SELECTIONMGR_H

#include "SalomeApp.h"

#include <SUIT_SelectionMgr.h>
#include <SALOME_InteractiveObject.hxx>

#include <qmap.h>

class SALOME_ListIO;
class SalomeApp_Application;
class TColStd_IndexedMapOfInteger;
class TColStd_MapOfInteger;

class SALOMEAPP_EXPORT SalomeApp_SelectionMgr : public SUIT_SelectionMgr
{
  Q_OBJECT

public:
  SalomeApp_SelectionMgr( SalomeApp_Application*, const bool = true );
  virtual ~SalomeApp_SelectionMgr();

  typedef QMap< Handle(SALOME_InteractiveObject), TColStd_IndexedMapOfInteger > MapIOOfMapOfInteger;
  typedef QMap< QString, TColStd_IndexedMapOfInteger > MapEntryOfMapOfInteger;

  SalomeApp_Application* application() const;

  void                   selectedObjects( SALOME_ListIO&, const QString& = QString::null ) const;
  void                   setSelectedObjects( const SALOME_ListIO&, const bool = false );

  void                   GetIndexes( const Handle(SALOME_InteractiveObject)& IObject, 
				     TColStd_IndexedMapOfInteger& theIndex );
  void                   GetIndexes( const QString& theEntry, 
				     TColStd_IndexedMapOfInteger& theIndex );

  bool                   AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& IObject, 
					   const TColStd_MapOfInteger& theIndices, 
					   bool modeShift );

  void                   selectObjects( const Handle(SALOME_InteractiveObject)& IObject, 
					TColStd_IndexedMapOfInteger theIndex, bool append );
  void                   selectObjects( MapIOOfMapOfInteger theMapIO, bool append );

  void                   selectedSubOwners( MapEntryOfMapOfInteger& theMap );

signals:
  void                   currentSelectionChanged();

private:
  virtual void           selectionChanged( SUIT_Selector* );

private:
  SalomeApp_Application* myApp;
};

#endif

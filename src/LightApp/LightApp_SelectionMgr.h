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

#ifndef LIGHTAPP_SELECTIONMGR_H
#define LIGHTAPP_SELECTIONMGR_H

#include "LightApp.h"

#include <SUIT_SelectionMgr.h>

#include <QtCore/QTime>

#ifndef DISABLE_SALOMEOBJECT
  #include <SALOME_InteractiveObject.hxx>
  #include <QMap>
  #include <NCollection_DataMap.hxx>

  #include <TColStd_IndexedMapOfInteger.hxx>
  #include <TColStd_MapOfInteger.hxx>

  class SALOME_ListIO;
  class TCollection_AsciiString;
#else
#include <QStringList>
#endif

class LightApp_Application;

/*!
  Custom selection manager, allowing to work with object selection
  (additionally to data owners) and to access to sub-selection of objects
*/
class LIGHTAPP_EXPORT LightApp_SelectionMgr : public SUIT_SelectionMgr
{
  Q_OBJECT

public:
  LightApp_SelectionMgr( LightApp_Application*, const bool = true );
  virtual ~LightApp_SelectionMgr();

  LightApp_Application* application() const;

  virtual void           setSelected( const SUIT_DataOwnerPtrList&, const bool = false );

#ifndef DISABLE_SALOMEOBJECT
  typedef NCollection_DataMap< Handle(SALOME_InteractiveObject), TColStd_IndexedMapOfInteger > MapIOOfMapOfInteger;
  typedef NCollection_DataMap< TCollection_AsciiString, TColStd_IndexedMapOfInteger > MapEntryOfMapOfInteger;

  void                   selectedObjects( SALOME_ListIO&, const QString& = QString(), const bool = true ) const;
  void                   setSelectedObjects( const SALOME_ListIO&, const bool = false );

  void                   GetIndexes( const Handle(SALOME_InteractiveObject)& IObject, 
                                     TColStd_IndexedMapOfInteger& theIndex );
  void                   GetIndexes( const QString& theEntry, 
                                     TColStd_IndexedMapOfInteger& theIndex );

  //bool                   AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& IObject, 
  void                   AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& IObject, 
                                           const TColStd_MapOfInteger& theIndices, 
                                           bool modeShift );

  void                   selectObjects( const Handle(SALOME_InteractiveObject)& IObject, 
                                        TColStd_IndexedMapOfInteger theIndex, bool append );
  void                   selectObjects( MapIOOfMapOfInteger theMapIO, bool append );

  void                   selectedSubOwners( MapEntryOfMapOfInteger& theMap );
#else
  void                   selectedObjects( QStringList&, const QString& = QString(), const bool = true ) const;
  void                   setSelectedObjects( const QStringList&, const bool = false );
#endif

  void                   clearSelectionCache();
  bool                   isSelectionCacheEnabled() const;
  void                   setSelectionCacheEnabled( bool );

signals:
  void                   currentSelectionChanged();

private:
  virtual void           selectionChanged( SUIT_Selector* );

#ifndef DISABLE_SALOMEOBJECT
  QList<Handle_SALOME_InteractiveObject> selectionCache( const QString& = QString() ) const;
#else
  QStringList                            selectionCache( const QString& = QString() ) const;
#endif
  bool                   isActualSelectionCache( const QString& = QString() ) const;

  QStringList            selectorTypes() const;

private:
#ifndef DISABLE_SALOMEOBJECT
  typedef Handle_SALOME_InteractiveObject SelObject;
#else
  typedef QString                         SelObject;
#endif
  typedef QList<SelObject>             SelList;
  typedef QMap<QString, QTime>         TimeMap;
  typedef QMap<QString, SelList>       CacheMap;

private:
  LightApp_Application* myApp;

  QTime                 myTimeStamp;

  bool                  myCacheState;
  TimeMap               myCacheTimes;
  CacheMap              myCacheSelection;
};

#endif

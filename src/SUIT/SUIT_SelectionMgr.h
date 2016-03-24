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

#ifndef SUIT_SELECTIONMGR_H
#define SUIT_SELECTIONMGR_H

#include "SUIT_DataOwner.h"

#include <QList>
#include <QObject>

class SUIT_Selector;
class SUIT_SelectionFilter;

#ifdef WIN32
#pragma warning ( disable : 4251 )
#endif

class SUIT_EXPORT SUIT_SelectionMgr : public QObject
{
  Q_OBJECT

public:
  SUIT_SelectionMgr( const bool = true, QObject* = 0 );
  virtual ~SUIT_SelectionMgr();

  void            clearSelected();
  virtual void    selected( SUIT_DataOwnerPtrList&, const QString& = QString() ) const;
  virtual void    setSelected( const SUIT_DataOwnerPtrList&, const bool = false );

  void            selectors( QList<SUIT_Selector*>& ) const;
  void            selectors( const QString&, QList<SUIT_Selector*>& ) const;


  void            setEnabled( const bool, const QString& = QString() );


  bool            hasSelectionMode( const int ) const;
  void            selectionModes( QList<int>& ) const;

  void            setSelectionModes( const int );
  virtual void    setSelectionModes( const QList<int>& );

  void            appendSelectionModes( const int );
  virtual void    appendSelectionModes( const QList<int>& );

  void            removeSelectionModes( const int );
  virtual void    removeSelectionModes( const QList<int>& );


  bool            isOk( const SUIT_DataOwner* ) const;
  bool            isOk( const SUIT_DataOwnerPtr& ) const;

  bool            hasFilter( SUIT_SelectionFilter* ) const;

  virtual void    installFilter( SUIT_SelectionFilter*, const bool = true );
  virtual void    removeFilter( SUIT_SelectionFilter* );
  virtual void    clearFilters();

  bool            autoDeleteFilter() const;
  void            setAutoDeleteFilter( const bool );

  bool            isSynchronizing() const;

signals:
  void            selectionChanged();

protected:
  virtual void    selectionChanged( SUIT_Selector* );

  virtual void    installSelector( SUIT_Selector* );
  virtual void    removeSelector( SUIT_Selector* );

private:
  void            filterOwners( const SUIT_DataOwnerPtrList&, SUIT_DataOwnerPtrList& ) const;

  typedef QList<SUIT_Selector*>        SelectorList;
  typedef QList<SUIT_SelectionFilter*> SelFilterList;

private:
  SelFilterList   myFilters;
  QList<int>      mySelModes;
  SelectorList    mySelectors;
  int             myIterations;
  bool            myAutoDelFilter;
  bool            myIsSelChangeEnabled;

  friend class SUIT_Selector;
};

#ifdef WIN32
#pragma warning ( default : 4251 )
#endif

#endif

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
#ifndef SUIT_SELECTIONMGR_H
#define SUIT_SELECTIONMGR_H

#include "SUIT_Selector.h"
#include "SUIT_DataOwner.h"
#include "SUIT_SelectionFilter.h"

#include <qobject.h>
#include <qptrlist.h>
#include <qvaluelist.h>

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
  virtual void    selected( SUIT_DataOwnerPtrList&, const QString& = QString::null ) const;
  virtual void    setSelected( const SUIT_DataOwnerPtrList&, const bool = false );

  void            selectors( QPtrList<SUIT_Selector>& ) const;
  void            selectors( const QString&, QPtrList<SUIT_Selector>& ) const;


  void            setEnabled( const bool, const QString& = QString::null );


  bool            hasSelectionMode( const int ) const;
  void            selectionModes( QValueList<int>& ) const;

  void            setSelectionModes( const int );
  virtual void    setSelectionModes( const QValueList<int>& );

  void            appendSelectionModes( const int );
  virtual void    appendSelectionModes( const QValueList<int>& );

  void            removeSelectionModes( const int );
  virtual void    removeSelectionModes( const QValueList<int>& );


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

  typedef QPtrListIterator<SUIT_Selector>        SelectorListIterator;

  virtual void    installSelector( SUIT_Selector* );
  virtual void    removeSelector( SUIT_Selector* );

private:
  void            filterOwners( const SUIT_DataOwnerPtrList&, SUIT_DataOwnerPtrList& ) const;

  typedef QPtrList<SUIT_Selector>                SelectorList;
  typedef QPtrList<SUIT_SelectionFilter>         SelFilterList;
  typedef QPtrListIterator<SUIT_SelectionFilter> SelFilterListIterator;

protected:
  SelectorList    mySelectors;

private:
  SelFilterList   myFilters;
  QValueList<int> mySelModes;
  int             myIterations;
  bool            myIsSelChangeEnabled;

  friend class SUIT_Selector;
};

#ifdef WIN32
#pragma warning ( default : 4251 )
#endif

#endif

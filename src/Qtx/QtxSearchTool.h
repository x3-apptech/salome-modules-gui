// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File   : QtxSearchTool.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef QTXSEARCHTOOL_H
#define QTXSEARCHTOOL_H

#include "Qtx.h"
#include <QFrame>
#include <QList>
#include <QMap>
#include <QPersistentModelIndex>
#include <QPointer>

class QCheckBox;
class QLineEdit;
class QShortcut;
class QTimer;
class QToolButton;
class QTreeView;

class QTX_EXPORT QtxSearchTool : public QFrame
{
  Q_OBJECT

  //! Find operation type
  enum {
    fAny,             //!< find any appropriate
    fNext,            //!< find next appropriate
    fPrevious,        //!< find previous appropriate
    fFirst,           //!< find first appropriate
    fLast             //!< find last appropriate
  };

public:
  class Searcher;

  //! Search tool controls
  typedef enum { 
    Search    = 0x00000001,                          //!< line edit field for searched text
    Next      = 0x00000002,                          //!< 'find next' button
    Prev      = 0x00000004,                          //!< 'find previous' button
    First     = 0x00000008,                          //!< 'find first' button
    Last      = 0x00000010,                          //!< 'find last' button
    Close     = 0x00000020,                          //!< 'close' button
    Case      = 0x00000040,                          //!< 'case sensitive search' check box
    RegExp    = 0x00000080,                          //!< 'regular expression' check box
    Wrap      = 0x00000100,                          //!< 'wrap search' check box
    Basic     = Search   | Next   | Prev | Close,    //!< basic controls: text field, "next", "prev" and "close" buttons
    Standard  = Basic    | First  | Last,            //!< standard controls: same as Basic plus "first" and "last" buttons
    Modifiers = Case     | RegExp | Wrap,            //!< search modifiers check boxes 
    All       = Standard | Modifiers                 //!< all controls
  } Controls;

  typedef enum {
    None        = 0x00000000,                                 //!< search tool can be activated programmatically only
    HotKey      = 0x00000001,                                 //!< search tool is activated by hot key
    SlashKey    = 0x00000002,                                 //!< search tool is activated by splash key ( / )
    StandardKey = 0x00000004,                                 //!< search tool is activated by standard key combination (e.g. F3)
    PrintKey    = 0x00000008,                                 //!< search tool is activated when user types any print key
    Any         = HotKey | SlashKey | StandardKey | PrintKey  //!< search tool is activated by any of above mentioned ways
  } Activator;

  QtxSearchTool( QWidget*, QWidget* = 0, int = All, Qt::Orientation = Qt::Vertical );
  QtxSearchTool( QWidget*, int = All, Qt::Orientation = Qt::Vertical );
  virtual ~QtxSearchTool();

  QWidget*            watchedWidget() const;
  void                setWatchedWidget( QWidget* );

  Searcher*           searcher() const;
  void                setSearcher( Searcher* );

  int                 activators() const;
  void                setActivators( const int );

  int                 controls() const;
  void                setControls( const int );

  QList<QKeySequence> shortcuts() const;
  void                setShortcuts( const QKeySequence& );
  void                setShortcuts( const QList<QKeySequence>& );

  int                 addCustomWidget( QWidget*, int = -1 );
  QWidget*            customWidget( int ) const;
  int                 customWidgetId( QWidget* ) const;

  bool                isAutoHideEnabled() const;
  void                enableAutoHide( bool );

  bool                isCaseSensitive() const;
  bool                isRegExpSearch() const;
  bool                isSearchWrapped() const;

  void                setCaseSensitive( bool );
  void                setRegExpSearch( bool );
  void                setSearchWrapped( bool );

  virtual bool        event( QEvent* );
  virtual bool        eventFilter( QObject*, QEvent* );

public slots:
  virtual void        find();
  virtual void        findNext();
  virtual void        findPrevious();
  virtual void        findFirst();
  virtual void        findLast();

private slots:
  void                find( const QString&, int = fAny );
  void                modifierSwitched();

private:
  void                init( Qt::Orientation );
  bool                focused() const;
  void                clearShortcuts();
  void                initShortcuts( const QList<QKeySequence>& );
  void                updateShortcuts();
  void                updateControls();

private:
  typedef QPointer<QShortcut> ShortcutPtr;
  typedef QList<ShortcutPtr>  ShortcutList;

private:
  QWidget*            myBtnWidget;
  QWidget*            myModWidget;
  QLineEdit*          myData;
  QToolButton*        myToFirst;
  QToolButton*        myToLast;
  QToolButton*        myNext;
  QToolButton*        myPrev;
  QToolButton*        myClose;
  QCheckBox*          myIsCaseSens;
  QCheckBox*          myIsRegExp;
  QCheckBox*          myWrap;
  QWidget*            myWatched;
  Searcher*           mySearcher;
  int                 myControls;
  int                 myActivators;
  ShortcutList        myShortcuts;
  QTimer*             myAutoHideTimer;
  bool                myAutoHideEnabled;
  QMap<int, QWidget*> myWidgets;
};

class QTX_EXPORT QtxSearchTool::Searcher
{
public:
  Searcher();
  virtual ~Searcher();

  virtual bool find( const QString&, QtxSearchTool* ) = 0;
  virtual bool findNext( const QString&, QtxSearchTool* ) = 0;
  virtual bool findPrevious( const QString&, QtxSearchTool* ) = 0;
  virtual bool findFirst( const QString&, QtxSearchTool* ) = 0;
  virtual bool findLast( const QString&, QtxSearchTool* ) = 0;
};

class QTX_EXPORT QtxTreeViewSearcher : public QtxSearchTool::Searcher
{
public:
  QtxTreeViewSearcher( QTreeView*, int = 0 );
  virtual ~QtxTreeViewSearcher();

  int                    searchColumn() const;
  void                   setSearchColumn( int );
  
  virtual bool           find( const QString&, QtxSearchTool* );
  virtual bool           findNext( const QString&, QtxSearchTool* );
  virtual bool           findPrevious( const QString&, QtxSearchTool* );
  virtual bool           findFirst( const QString&, QtxSearchTool* );
  virtual bool           findLast( const QString&, QtxSearchTool* );

protected:
  virtual Qt::MatchFlags matchFlags( QtxSearchTool* ) const;
  
private:
  QModelIndexList        findItems( const QString&, QtxSearchTool* );
  QModelIndex            findNearest( const QModelIndex&, const QModelIndexList&, bool );
  void                   showItem( const QModelIndex& );
  QString                getId( const QModelIndex& );
  int                    compareIndices( const QModelIndex&, const QModelIndex& );

private:
  QTreeView*             myView;
  int                    myColumn;
  QPersistentModelIndex  myIndex;
};

#endif // QTXSEARCHTOOL_H

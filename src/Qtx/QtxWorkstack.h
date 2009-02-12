//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File:      QtxWorkstack.h
// Author:    Sergey TELKOV
//
#ifndef QTXWORKSTACK_H
#define QTXWORKSTACK_H

#include "Qtx.h"

#include <QWidget>
#include <QFrame>
#include <QTabBar>
#include <QEvent>
#include <QMap>

class QAction;
class QSplitter;
class QStackedWidget;
class QRubberBand;
class QAbstractButton;

class QtxWorkstackArea;
class QtxWorkstackDrag;
class QtxWorkstackChild;
class QtxWorkstackTabBar;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxWorkstack : public QWidget
{
  Q_OBJECT

public:
  //! Workstack actions (context menu items)
  enum { SplitVertical    = 0x01,  //!< "Split vertically" menu item
	 SplitHorizontal  = 0x02,  //!< "Split horizontally" menu item
	 Close            = 0x04,  //!< "Close" menu item
	 Rename           = 0x08,  //!< "Rename" menu item
	 All = SplitVertical | SplitHorizontal | 
	       Close | Rename      //!< all menu items
  };
    
  //! Workstack splitting type
  enum SplitType
  {
    SplitStay,  //!< selected widget stays in current workarea, others widgets are moved into a new workarea
    SplitAt,    //!< all widgets before selected widget stay in current workarea, other widgess are moved into a new workarea
    SplitMove   //!< selected widget is moved into a new workarea, all other widgets stay in an old workarea
  };

public:
  QtxWorkstack( QWidget* = 0 );
  virtual ~QtxWorkstack();

  QWidgetList         windowList( QWidget* = 0 ) const;
  QWidgetList         splitWindowList() const;

  QWidget*            activeWindow() const;

  int                 accel( const int ) const;
  void                setAccel( const int, const int );

  QIcon               icon( const int ) const;
  void                setIcon( const int, const QIcon& );

  void                setMenuActions( const int );
  int                 menuActions() const;

  void                split( const int );
  bool                move( QWidget* wid, QWidget* wid_to, const bool before );
  void                stack();

  QWidget*            addWindow( QWidget*, Qt::WindowFlags = 0 );

  void Split( QWidget* wid, const Qt::Orientation o, const SplitType type );
  void Attract( QWidget* wid1, QWidget* wid2, const bool all );
  void SetRelativePosition( QWidget* wid, const Qt::Orientation o, const double pos );
  void SetRelativePositionInSplitter( QWidget* wid, const double pos );

  // asv: Store/Restore visual parameters - geometry of inner windows
  QtxWorkstack& operator<<( const QString& );
  QtxWorkstack& operator>>( QString& );

signals:
  void                windowActivated( QWidget* );

public slots:
  void                splitVertical();
  void                splitHorizontal();
  
private slots:
  void                onRename();
  void                onCloseWindow();
  void                onDestroyed( QObject* );
  void                onWindowActivated( QWidget* );
  void                onContextMenuRequested( QWidget*, QPoint );
  void                onDeactivated( QtxWorkstackArea* );

protected:
  virtual void        customEvent( QEvent* );

private:
  QSplitter*          splitter( QtxWorkstackArea* ) const;
  void                splitters( QSplitter*, QList<QSplitter*>&, const bool = false ) const;
  void                areas( QSplitter*, QList<QtxWorkstackArea*>&, const bool = false ) const;

  QSplitter*          wrapSplitter( QtxWorkstackArea* );
  void                insertWidget( QWidget*, QWidget*, QWidget* );

  QtxWorkstackArea*   areaAt( const QPoint& ) const;
  QtxWorkstackArea*   wgArea( QWidget* ) const;

  QtxWorkstackArea*   targetArea();
  QtxWorkstackArea*   activeArea() const;
  QtxWorkstackArea*   currentArea() const;

  void                setActiveArea( QtxWorkstackArea* );
  QtxWorkstackArea*   neighbourArea( QtxWorkstackArea* ) const;

  QtxWorkstackArea*   createArea( QWidget* ) const;

  void                updateState();
  void                updateState( QSplitter* );

  void                distributeSpace( QSplitter* ) const;
  int                 setPosition( QWidget* wid, QSplitter* split, const Qt::Orientation o,
				                           const int need_pos, const int splitter_pos );
  
  void                splitterInfo( QSplitter*, QString& ) const;
  void                setSplitter( QSplitter*, const QString&, QMap< QSplitter*, QList<int> >& );
  
private:
  QWidget*            myWin;        //!< active widget
  QtxWorkstackArea*   myArea;       //!< active workarea
  QSplitter*          mySplit;      //!< tol-level splitter
  QWidget*            myWorkWin;    //!< widget where popup menu is invoked (used internally)
  QtxWorkstackArea*   myWorkArea;   //!< workarea where popup menu is invoked (used internally)

  QMap<int, QAction*> myActionsMap; //!< actions map

  friend class QtxWorkstackArea;
  friend class QtxWorkstackDrag;
};

class QtxWorkstackArea : public QFrame
{
  Q_OBJECT

  class WidgetEvent;

public:
  QtxWorkstackArea( QWidget* );
  virtual ~QtxWorkstackArea();

  bool                isNull() const;
  bool                isEmpty() const;

  QWidget*            insertWidget( QWidget*, const int = -1, Qt::WindowFlags = 0 );
  void                removeWidget( QWidget*, const bool = true );

  QWidget*            activeWidget() const;
  void                setActiveWidget( QWidget* );

  bool                contains( QWidget* ) const;

  QWidgetList         widgetList() const;

  bool                isActive() const;
  void                updateActiveState();

  QtxWorkstack*       workstack() const;

  virtual bool        eventFilter( QObject*, QEvent* );

  QRect               floatRect() const;
  QRect               floatTab( const int ) const;

  int                 tabAt( const QPoint& ) const;

signals:
  void                activated( QWidget* );
  void                contextMenuRequested( QWidget*, QPoint );
  void                deactivated( QtxWorkstackArea* );

public slots:
  virtual void        setVisible( bool );

private slots:
  void                onClose();
  void                onCurrentChanged( int );

  void                onWidgetDestroyed();

  void                onChildDestroyed( QObject* );
  void                onChildShown( QtxWorkstackChild* );
  void                onChildHidden( QtxWorkstackChild* );
  void                onChildActivated( QtxWorkstackChild* );
  void                onChildCaptionChanged( QtxWorkstackChild* );

  void                onDragActiveTab();
  void                onContextMenuRequested( QPoint );

protected:
  virtual void        customEvent( QEvent* );
  virtual void        focusInEvent( QFocusEvent* );
  virtual void        mousePressEvent( QMouseEvent* );

private:
  //! Custom events
  enum { ActivateWidget = QEvent::User,   //!< activate widget event
	 FocusWidget,                     //!< focus receiving widget event
	 RemoveWidget                     //!< widget removing event
  };

private:
  void                updateState();
  void                updateCurrent();
  void                updateTab( QWidget* );

  QWidget*            widget( const int ) const;
  int                 widgetId( QWidget* ) const;
  bool                widgetVisibility( QWidget* ) const;

  void                setWidgetActive( QWidget* );
  void                setWidgetShown( QWidget*, const bool );

  int                 generateId() const;

  bool                isBlocked( QWidget* ) const;
  void                setBlocked( QWidget*, const bool );

  QtxWorkstackChild*  child( QWidget* ) const;

private:
  struct WidgetInfo
  {
    WidgetInfo() : id( 0 ), vis( false ) {}
    int id; bool vis;
  };

  typedef QMap<QWidget*, bool>               BlockMap;
  typedef QMap<QWidget*, QtxWorkstackChild*> ChildMap;
  typedef QMap<QWidget*, WidgetInfo>         WidgetInfoMap;

private:
  QtxWorkstackTabBar* myBar;     //!< workarea tab bar header
  QAbstractButton*    myClose;   //!< close button
  QStackedWidget*     myStack;   //!< widget stack

  QWidgetList         myList;    //!< child widgets list
  WidgetInfoMap       myInfo;    //!< widgets states mp
  ChildMap            myChild;   //!< child widget containers map
  BlockMap            myBlock;   //!< blocked widgets
};

class QtxWorkstackChild : public QWidget
{
  Q_OBJECT

public:
  QtxWorkstackChild( QWidget*, QWidget* = 0, Qt::WindowFlags = 0 );
  virtual ~QtxWorkstackChild();

  QWidget*            widget() const;

  virtual bool        eventFilter( QObject*, QEvent* );

signals:
  void                shown( QtxWorkstackChild* );
  void                hidden( QtxWorkstackChild* );
  void                activated( QtxWorkstackChild* );
  void                captionChanged( QtxWorkstackChild* );

private slots:
  void                onDestroyed( QObject* );

protected:
  virtual void        childEvent( QChildEvent* );

private:
  QWidget*            myWidget;   //!< child widget
};

class QtxWorkstackTabBar : public QTabBar
{
  Q_OBJECT

public:
  QtxWorkstackTabBar( QWidget* = 0 );
  virtual ~QtxWorkstackTabBar();

  bool                isActive() const;
  void                setActive( const bool );

  int                 tabId( const int ) const;
  int                 indexOf( const int ) const;
  void                setTabId( const int, const int );

  void                updateActiveState();

signals:
  void                dragActiveTab();
  void                contextMenuRequested( QPoint );

private slots:
  void                onCurrentChanged( int );

protected:
  virtual void        mouseMoveEvent( QMouseEvent* );
  virtual void        mousePressEvent( QMouseEvent* );
  virtual void        mouseReleaseEvent( QMouseEvent* );
  virtual void        contextMenuEvent( QContextMenuEvent* );
  virtual void        changeEvent( QEvent* );

//  virtual void        paintLabel( QPainter*, const QRect&, QTab*, bool ) const;

private:
  int                 myId;         //!< current tab page index
  bool                myActive;     //!< "active" status
};

class QtxWorkstackDrag : public QObject
{
  Q_OBJECT

public:
  QtxWorkstackDrag( QtxWorkstack*, QtxWorkstackChild* );
  virtual ~QtxWorkstackDrag();

  virtual bool        eventFilter( QObject*, QEvent* );

private:
  void                dropWidget();

  void                updateTarget( const QPoint& );
  QtxWorkstackArea*   detectTarget( const QPoint&, int& ) const;
  void                setTarget( QtxWorkstackArea*, const int );

  void                drawRect();
  void                endDrawRect();
  void                startDrawRect();

private:
  QtxWorkstack*       myWS;          //!< parent workstack
  QtxWorkstackChild*  myChild;       //!< workstack child widget container

  int                 myTab;         //!< workarea tab page index
  QtxWorkstackArea*   myArea;        //!< workarea
  QRubberBand*        myTabRect;     //!< tab bar rubber band
  QRubberBand*        myAreaRect;    //!< workarea rubber band
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif   // QTXWORKSTACK_H

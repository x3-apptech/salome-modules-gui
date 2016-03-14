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

// File:      QtxWorkstack.h
// Author:    Sergey TELKOV
//
#ifndef QTXWORKSTACK_H
#define QTXWORKSTACK_H

#include "Qtx.h"

#include <QMap>
#include <QFrame>
#include <QEvent>
#include <QWidget>
#include <QTabBar>
#include <QPointer>
#include <QSplitter>
#include <QByteArray>

class QAction;
class QDataStream;
class QRubberBand;
class QStackedWidget;
class QAbstractButton;

class QtxWorkstackArea;
class QtxWorkstackDrag;
class QtxWorkstackChild;
class QtxWorkstackTabBar;
class QtxWorkstackSplitter;

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

  enum { VersionMarker = 0x01,
               SplitMarker   = 0x02,
               AreaMarker    = 0x04,
               WidgetMarker  = 0x08
  };

  enum { Horizontal = 0x01,
               Visible    = 0x02
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
  void                setActiveWindow( QWidget* );

  int                 accel( const int ) const;
  void                setAccel( const int, const int );

  QIcon               icon( const int ) const;
  void                setIcon( const int, const QIcon& );

  void                setMenuActions( const int );
  int                 menuActions() const;

  void                stack();
  void                split( const int );
  bool                move( QWidget* wid, QWidget* wid_to, const bool before );

  QWidget*            addWindow( QWidget*, Qt::WindowFlags = 0 );

  QByteArray          saveState( int ) const;
  bool                restoreState( const QByteArray&, int );
  
  void                setOpaqueResize( bool = true );
  bool                opaqueResize() const;

  void                splittersVisible( QWidget*, bool = true );

  void Split( QWidget* wid, const Qt::Orientation o, const SplitType type );
  void Attract( QWidget* wid1, QWidget* wid2, const bool all );
  void SetRelativePosition( QWidget* wid, const Qt::Orientation o, const double pos );
  void SetRelativePositionInSplitter( QWidget* wid, const double pos );

  void                updateState();

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

  QAction*            action( const int ) const;

  void                saveState( QDataStream& ) const;
  bool                restoreState( QDataStream& );

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

  void                updateState( QSplitter* );

  void                splitterVisible(QWidget*, QList<QSplitter*>&, QSplitter*, bool );

  void                distributeSpace( QSplitter* ) const;

  int                 setPosition( QWidget* wid, QSplitter* split, const Qt::Orientation o,
                                                           const int need_pos, const int splitter_pos );

private:
  QPointer<QWidget>          myWin;        //!< active widget
  QPointer<QtxWorkstackArea> myArea;       //!< active workarea
  QtxWorkstackSplitter*      mySplit;      //!< tol-level splitter
  QPointer<QWidget>          myWorkWin;    //!< widget where popup menu is invoked (used internally)
  QPointer<QtxWorkstackArea> myWorkArea;   //!< workarea where popup menu is invoked (used internally)

  QMap<int, QAction*> myActionsMap; //!< actions map

  friend class QtxWorkstackArea;
  friend class QtxWorkstackDrag;
  friend class QtxWorkstackAction;
  friend class QtxWorkstackSplitter;
};

class QtxWorkstackSplitter : public QSplitter
{
  Q_OBJECT

public:
  QtxWorkstackSplitter( QWidget* = 0 );
  virtual ~QtxWorkstackSplitter();

  QtxWorkstack*       workstack() const;

  void                saveState( QDataStream& ) const;
  bool                restoreState( QDataStream&, QMap<QString, QtxWorkstackChild*>& );
};

class QTX_EXPORT QtxWorkstackArea : public QFrame
{
  Q_OBJECT

  class WidgetEvent;
  class RestoreEvent;

public:
  QtxWorkstackArea( QWidget* );
  virtual ~QtxWorkstackArea();

  bool                isNull() const;
  bool                isEmpty() const;

  QtxWorkstackChild*  insertWidget( QWidget*, const int = -1, Qt::WindowFlags = 0 );
  void                removeWidget( QWidget*, const bool = true );

  void                insertChild( QtxWorkstackChild*, const int = -1 );
  void                removeChild( QtxWorkstackChild*, const bool = true );

  QWidget*            activeWidget() const;
  void                setActiveWidget( QWidget* );

  bool                contains( QWidget* ) const;

  QWidgetList         widgetList() const;
  QList<QtxWorkstackChild*> childList() const;

  bool                isActive() const;
  void                updateActiveState();

  QtxWorkstack*       workstack() const;

  virtual bool        eventFilter( QObject*, QEvent* );

  QRect               floatRect() const;
  QRect               floatTab( const int ) const;

  int                 tabAt( const QPoint& ) const;

  void                saveState( QDataStream& ) const;
  bool                restoreState( QDataStream&, QMap<QString, QtxWorkstackChild*>& );

  void                showTabBar( bool = true);

signals:
  void                activated( QWidget* );
  void                contextMenuRequested( QWidget*, QPoint );
  void                deactivated( QtxWorkstackArea* );

private slots:
  void                onClose();
  void                onCurrentChanged( int );

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
         MakeCurrent,
         RestoreWidget
  };

private:
  void                updateState();
  void                updateCurrent();
  void                updateTab( QWidget* );

  QWidget*            widget( const int ) const;
  int                 widgetId( QWidget* ) const;

  QtxWorkstackChild*  child( QWidget* ) const;
  QtxWorkstackChild*  child( const int ) const;

  void                setWidgetActive( QWidget* );

  int                 generateId() const;

private:
  typedef QList<QtxWorkstackChild*> ChildList;

private:
  QWidget*            myTop;     //!< workarea top widget
  QtxWorkstackTabBar* myBar;     //!< workarea tab bar header
  ChildList           myList;    //!< child widgets list
  QAbstractButton*    myClose;   //!< close button
  QStackedWidget*     myStack;   //!< widget stack
};

class QtxWorkstackChild : public QWidget
{
  Q_OBJECT

public:
  QtxWorkstackChild( QWidget*, QWidget* = 0, Qt::WindowFlags = 0 );
  virtual ~QtxWorkstackChild();

  QWidget*            widget() const
;

  int                 id() const;
  void                setId( const int );

  bool                visibility();

  QtxWorkstackArea*   area() const;

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
  int                 myId;       //!< id
  QPointer<QWidget>   myWidget;   //!< child widget
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
  virtual void        changeEvent( QEvent* );
  virtual void        mouseMoveEvent( QMouseEvent* );
  virtual void        mousePressEvent( QMouseEvent* );
  virtual void        mouseReleaseEvent( QMouseEvent* );
  virtual void        contextMenuEvent( QContextMenuEvent* );

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

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
// File:      QtxWorkstack.h
// Author:    Sergey TELKOV

#ifndef QTXWORKSTACK_H
#define QTXWORKSTACK_H

#include "Qtx.h"

#include <qhbox.h>
#include <qwidget.h>
#include <qtabbar.h>
#include <qwidgetlist.h>

class QAction;
class QTabBar;
class QPainter;
class QSplitter;
class QPushButton;
class QWidgetStack;

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
  enum { SplitVertical, SplitHorizontal, Close, Rename };
    
  enum SplitType
  {
    SPLIT_STAY, //!< given widget stays in its workarea, others are moved into a new one
    SPLIT_AT,   //!< widgets before a given widget stays in they workarea, others are moved into a new one
    SPLIT_MOVE  //!< given widget is moved into a new workarea, others stay in an old one
  };

public:
  QtxWorkstack( QWidget* = 0 );
  virtual ~QtxWorkstack();

  QWidgetList         windowList() const;
  QWidgetList         splitWindowList() const;

  QWidget*            activeWindow() const;

  int                 accel( const int ) const;
  void                setAccel( const int, const int );

  void                split( const int );

  // STV: Useless function. wid->setFocus() should be used instead.
  // void OnTop( QWidget* wid);

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
  virtual void        childEvent( QChildEvent* );
  virtual void        customEvent( QCustomEvent* );

private:
  QSplitter*          splitter( QtxWorkstackArea* ) const;
  void                splitters( QSplitter*, QPtrList<QSplitter>&, const bool = false ) const;
  void                areas( QSplitter*, QPtrList<QtxWorkstackArea>&, const bool = false ) const;

  QSplitter*          wrapSplitter( QtxWorkstackArea* );
  void                insertWidget( QWidget*, QWidget*, QWidget* );

  QtxWorkstackArea*   areaAt( const QPoint& ) const;

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
  void                setSplitter( QSplitter*, const QString&, QMap< QSplitter*,QValueList<int> >& );
  
private:
  QWidget*            myWin;
  QtxWorkstackArea*   myArea;
  QSplitter*          mySplit;
  QWidget*            myWorkWin;
  QtxWorkstackArea*   myWorkArea;

  QMap<int, QAction*> myActionsMap; //!< The map of the actions. Allows to get the QAction object by the key.

  friend class QtxWorkstackArea;
  friend class QtxWorkstackDrag;
};

class QtxWorkstackArea : public QWidget
{
  Q_OBJECT

public:
  QtxWorkstackArea( QWidget* );
  virtual ~QtxWorkstackArea();

  bool                isEmpty() const;

  void                insertWidget( QWidget*, const int = -1 );
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
  virtual void        show();
  virtual void        hide();

private slots:
  void                onClose();
  void                onSelected( int );

  void                onWidgetDestroyed();

  void                onChildDestroyed( QObject* );
  void                onChildShown( QtxWorkstackChild* );
  void                onChildHided( QtxWorkstackChild* );
  void                onChildActivated( QtxWorkstackChild* );
  void                onChildCaptionChanged( QtxWorkstackChild* );

  void                onDragActiveTab();
  void                onContextMenuRequested( QPoint );

protected:
  virtual void        customEvent( QCustomEvent* );
  virtual void        focusInEvent( QFocusEvent* );
  virtual void        mousePressEvent( QMouseEvent* );

private:
  enum { ActivateWidget = QEvent::User, FocusWidget, RemoveWidget };

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
  QtxWorkstackTabBar* myBar;
  QPushButton*        myClose;
  QWidgetStack*       myStack;

  QWidgetList         myList;
  WidgetInfoMap       myInfo;
  ChildMap            myChild;
  BlockMap            myBlock;
};

class QtxWorkstackChild : public QHBox
{
  Q_OBJECT

public:
  QtxWorkstackChild( QWidget*, QWidget* = 0 );
  virtual ~QtxWorkstackChild();

  QWidget*            widget() const;

  virtual bool        eventFilter( QObject*, QEvent* );

signals:
  void                shown( QtxWorkstackChild* );
  void                hided( QtxWorkstackChild* );
  void                activated( QtxWorkstackChild* );
  void                captionChanged( QtxWorkstackChild* );

private slots:
  void                onDestroyed( QObject* );

protected:
  virtual void        childEvent( QChildEvent* );

private:
  QWidget*            myWidget;
};

class QtxWorkstackTabBar : public QTabBar
{
  Q_OBJECT

public:
  QtxWorkstackTabBar( QWidget* = 0 );
  virtual ~QtxWorkstackTabBar();

  QRect               tabRect( const int ) const;

  void                setActive( const bool );

signals:
  void                dragActiveTab();
  void                contextMenuRequested( QPoint );

protected:
  virtual void        mouseMoveEvent( QMouseEvent* );
  virtual void        mousePressEvent( QMouseEvent* );
  virtual void        mouseReleaseEvent( QMouseEvent* );
  virtual void        contextMenuEvent( QContextMenuEvent* );

  virtual void        paintLabel( QPainter*, const QRect&, QTab*, bool ) const;

private:
  int                 myId;
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
  QtxWorkstack*       myWS;
  QtxWorkstackChild*  myChild;

  int                 myTab;
  QtxWorkstackArea*   myArea;
  QPainter*           myPainter;
  
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif

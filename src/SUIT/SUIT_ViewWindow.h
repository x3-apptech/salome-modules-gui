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

// SUIT_ViewWindow.h: interface for the SUIT_ViewWindow class.
//
#ifndef SUIT_VIEWWINDOW_H
#define SUIT_VIEWWINDOW_H

#include "SUIT.h"
#include "SUIT_CameraProperties.h"

#include <QMainWindow>
#include <QList>
#include <QMap>

class SUIT_Desktop;
class SUIT_ViewManager;
class QtxActionToolMgr;
class QtxMultiAction;
class QImage;

class SUIT_EXPORT SUIT_ViewWindow: public QMainWindow 
{
  Q_OBJECT

public:

  SUIT_ViewWindow( SUIT_Desktop* );
  virtual ~SUIT_ViewWindow();

  virtual void      setViewManager( SUIT_ViewManager* );
  SUIT_ViewManager* getViewManager() const;

  bool              event(QEvent*);

  virtual QImage    dumpView();
  bool              dumpViewToFormat( const QString& fileName, const QString& format );

  bool              onAccelAction( int );

  bool              closable() const;
  bool              setClosable( const bool );

  virtual QString   getVisualParameters();
  virtual void      setVisualParameters( const QString& parameters );

  void              setDestructiveClose( const bool );
  
  int               getId() const;

  QtxActionToolMgr* toolMgr() const;

  virtual void      setDropDownButtons( bool );
  bool              dropDownButtons() const;

  virtual SUIT_CameraProperties cameraProperties();

public slots:
  virtual void      onDumpView();

signals:
  void              tryClosing( SUIT_ViewWindow* );
  void              closing( SUIT_ViewWindow* );
  void              mousePressed( SUIT_ViewWindow*, QMouseEvent* );
  void              mouseReleased( SUIT_ViewWindow*, QMouseEvent* );
  void              mouseDoubleClicked( SUIT_ViewWindow*, QMouseEvent* );
  void              mouseMoving( SUIT_ViewWindow*, QMouseEvent* );
  void              wheeling( SUIT_ViewWindow*, QWheelEvent* );
  void              keyPressed( SUIT_ViewWindow*, QKeyEvent* );
  void              keyReleased( SUIT_ViewWindow*, QKeyEvent* );
  void              contextMenuRequested( QContextMenuEvent *e );
  void              viewModified( SUIT_ViewWindow* );
  
protected:
  void              closeEvent( QCloseEvent* );
  virtual void      contextMenuEvent( QContextMenuEvent* );
  virtual QString   filter() const;
  virtual bool      action( const int );
  virtual bool      dumpViewToFormat( const QImage&, const QString& fileName, const QString& format );

  virtual QAction*  synchronizeAction();
  static void       synchronizeView( SUIT_ViewWindow* viewWindow, int id );
  
  SUIT_Desktop*     myDesktop;
  SUIT_ViewManager* myManager;

protected slots:
  void              updateSyncViews();
  void              onSynchronizeView(bool);
  virtual void      synchronize( SUIT_ViewWindow* );
  void              emitViewModified();
  
private:
  typedef QMap< int, QList<QtxMultiAction*> > ActionsMap;

  QtxActionToolMgr* myToolMgr;
  bool              myIsDropDown;
  ActionsMap        myMultiActions;
  QAction*          mySyncAction;
};

#endif // SUIT_VIEWWINDOW_H

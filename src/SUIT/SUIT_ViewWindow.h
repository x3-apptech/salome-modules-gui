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
// SUIT_ViewWindow.h: interface for the SUIT_ViewWindow class.
//
#ifndef SUIT_VIEWWINDOW_H
#define SUIT_VIEWWINDOW_H

#include "SUIT.h"

#include <QMainWindow>

class SUIT_Desktop;
class SUIT_ViewManager;
class QtxActionToolMgr;
class QImage;

class SUIT_EXPORT SUIT_ViewWindow: public QMainWindow 
{
  Q_OBJECT
public:
  SUIT_ViewWindow( SUIT_Desktop* );
  virtual ~SUIT_ViewWindow();

  void              setViewManager( SUIT_ViewManager* );
  SUIT_ViewManager* getViewManager() const;

  bool              event(QEvent*);

  virtual QImage    dumpView();
  bool              dumpViewToFormat( const QString& fileName, const QString& format );

  bool              onAccelAction( int );

  virtual QString   getVisualParameters();
  virtual void      setVisualParameters( const QString& parameters );

  void              setDestructiveClose( const bool );
  
  int               getId() const;

  QtxActionToolMgr* toolMgr() const;

public slots:
  virtual void      onDumpView();

signals:
  void              closing( SUIT_ViewWindow* );
  void              mousePressed( SUIT_ViewWindow*, QMouseEvent* );
  void              mouseReleased( SUIT_ViewWindow*, QMouseEvent* );
  void              mouseDoubleClicked( SUIT_ViewWindow*, QMouseEvent* );
  void              mouseMoving( SUIT_ViewWindow*, QMouseEvent* );
  void              wheeling( SUIT_ViewWindow*, QWheelEvent* );
  void              keyPressed( SUIT_ViewWindow*, QKeyEvent* );
  void              keyReleased( SUIT_ViewWindow*, QKeyEvent* );
  void              contextMenuRequested( QContextMenuEvent *e );

protected:
  void              closeEvent( QCloseEvent* );
  virtual void      contextMenuEvent( QContextMenuEvent* );
  virtual QString   filter() const;
  virtual bool      action( const int );
  virtual bool      dumpViewToFormat( const QImage&, const QString& fileName, const QString& format );

  SUIT_Desktop*     myDesktop;
  SUIT_ViewManager* myManager;

private:
  QtxActionToolMgr* myToolMgr;
};

#endif // !defined(AFX_SUIT_VIEWWINDOW_H__82C3D51A_6F10_45B0_BCFE_3CB3EF596A4D__INCLUDED_)

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
// File:      QtxDockAction.h
// Author:    Sergey TELKOV

#ifndef QTX_DOCKACTION_H
#define QTX_DOCKACTION_H

#include "QtxAction.h"

#include <qevent.h>
#include <qptrlist.h>
#include <qpopupmenu.h>

class QAction;
class QDockArea;
class QDockWindow;
class QMainWindow;
class QtxResourceMgr;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxDockAction : public QtxAction
{
  Q_OBJECT

  Q_PROPERTY( bool autoAdd   READ isAutoAdd   WRITE setAutoAdd )
  Q_PROPERTY( bool autoPlace READ isAutoPlace WRITE setAutoPlace )
  Q_PROPERTY( bool separate  READ isSeparate  WRITE setSeparate )

public:
  QtxDockAction( QMainWindow*, const char* = 0 );
  QtxDockAction( const QString&, const QString&, QMainWindow*, const char* = 0 );
  QtxDockAction( const QString&, const QIconSet&, const QString&, QMainWindow*, const char* = 0 );
  virtual ~QtxDockAction();

  QMainWindow* mainWindow() const;

  bool         isAutoAdd() const;
  void         setAutoAdd( const bool );

  bool         isAutoPlace() const;
  void         setAutoPlace( const bool );

  bool         isSeparate() const;
  void         setSeparate( const bool );

  virtual bool addTo( QWidget* );
  virtual bool addTo( QWidget*, const int );
  virtual bool removeFrom( QWidget* );
  virtual void setMenuText( const QString& );

  bool         addDockWindow( QDockWindow* );
  bool         removeDockWindow( QDockWindow* );

  virtual bool eventFilter( QObject*, QEvent* );

  virtual void storeGeometry( QDockWindow* = 0 );
  virtual void restoreGeometry( QDockWindow* = 0 ) const;

  virtual void loadGeometry( QtxResourceMgr*, const QString&, const bool = true );
  virtual void saveGeometry( QtxResourceMgr*, const QString&, const bool = true ) const;

private slots:
  void         onAboutToShow();
  void         onToggled( bool );
  void         onVisibilityChanged( bool );
  void         onPopupDestroyed( QObject* );
  void         onWindowDestroyed( QObject* );
  void         onDockWindowPositionChanged( QDockWindow* );

protected:
  virtual bool event( QEvent* );

private:
  QAction*     action( QDockWindow* ) const;
  QDockWindow* dockWindow( const QAction* ) const;

  void         checkPopup( QPopupMenu* );
  void         fillPopup( QPopupMenu* ) const;
  int          findId( QPopupMenu*, QPopupMenu* ) const;

  void         initialize( QMainWindow* );
  void         updateInfo( QDockWindow* );
  bool         isToolBar( QDockWindow* ) const;
  void         dockWindows( QPtrList<QDockWindow>&, QMainWindow* = 0 ) const;

  QString      windowName( QDockWindow* ) const;
  void         savePlaceInfo( QDockWindow* );
  void         loadPlaceInfo( QDockWindow* ) const;
  void         loadPlaceInfo() const;

  bool         autoAddDockWindow( QDockWindow* );
  void         autoLoadPlaceInfo( QDockWindow* );

  void         splitMenuText( QString&, QString& ) const;
  QStringList  splitText( const QString&, const QString& ) const;

  QDockArea*   dockArea( const int ) const;
  int          dockPlace( const QString& ) const;

  void         collectNames( const int, QStringList& ) const;

  void         updateMenus();

  bool         dockMainWindow( QMainWindow*, QObject* ) const;

private:
  enum { AutoAdd = QEvent::User, LoadArea };

  typedef struct { bool vis, newLine;
                   int place, index, offset;
                   int x, y, w, h, fixW, fixH; } GeomInfo;
  typedef struct { QString name; QAction* a; }   DockInfo;
  typedef struct { QPopupMenu *dock, *tool; }    MenuInfo;

  typedef QMap<QDockWindow*, DockInfo>   InfoMap;
  typedef QMap<QString, GeomInfo>        GeomMap;
  typedef QMap<QPopupMenu*, MenuInfo>    MenuMap;

private:
  bool         loadGeometry( QtxResourceMgr*, const QString&,
                             const QString&, GeomInfo& ) const;
  bool         saveGeometry( QtxResourceMgr*, const QString&,
                             const QString&, const GeomInfo& ) const;
  void         loadPlaceArea( const int, QMainWindow*, QDockArea*,
                              const QPtrList<QDockWindow>&,
                              const QMap<QDockWindow*, GeomInfo*>& ) const;

private:
  InfoMap      myInfo;
  MenuMap      myMenu;
  GeomMap      myGeom;
  QMainWindow* myMain;
  QStringList  myNames;

  bool         myAutoAdd;
  bool         mySeparate;
  bool         myAutoPlace;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif

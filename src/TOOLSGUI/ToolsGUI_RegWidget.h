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
//  SALOME RegistryDisplay : GUI for Registry server implementation
//  File   : ToolsGUI_RegWidget.h
//  Author : Pascale NOYRET, EDF
//
#ifndef TOOLSGUI_REGWIDGET_H
#define TOOLSGUI_REGWIDGET_H

#include "ToolsGUI.h"

#include <QMainWindow>

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Registry)

class QTabWidget;
class QTreeWidget;
class QTreeWidgetItem;
class QWidget;
class QTimer;
class QCloseEvent;
class QAction;

class TOOLSGUI_EXPORT ToolsGUI_RegWidget : public QMainWindow
{
  Q_OBJECT

  class HelpWindow;
  class IntervalWindow;
  class InfoWindow;

  ToolsGUI_RegWidget( CORBA::ORB_var& orb, QWidget* parent = 0 );

public:
  ~ToolsGUI_RegWidget();

  void                       SetListe();
  void                       SetListeHistory();
  void                       InfoReg();
  void                       InfoHistory();

  bool                       eventFilter( QObject* object, QEvent* event );

  static ToolsGUI_RegWidget* GetRegWidget( CORBA::ORB_var& orb, 
					   QWidget* parent = 0 );

  virtual QMenu*             createPopupMenu();

public slots:
  void                       slotHelp();
  void                       slotListeSelect();
  void                       slotClientChanged( QTreeWidgetItem*, int );
  void                       slotHistoryChanged( QTreeWidgetItem*, int );
  void                       slotSelectRefresh();
  void                       slotIntervalOk();

protected:  
  static QString             setlongText( const Registry::Infos& c_info );
  int                        numitem( const QString& name, 
				      const QString& pid, 
				      const QString& machine, 
				      const Registry::AllInfos* clistclient );
  void                       closeEvent( QCloseEvent* e );
   
protected :
  QTreeWidget*         _clients;
  QTreeWidget*         _history;
  QWidget*             _parent;
  QTabWidget*          _tabWidget;
  QAction*             _refresh;
  QAction*             _interval;
  QAction*             _close;
  QTimer*              _counter;
  Registry::AllInfos*  _serverhistory;
  Registry::AllInfos*  _serverclients;
  InfoWindow*          myInfoWindow;
  HelpWindow*          myHelpWindow;
  IntervalWindow*      myIntervalWindow;
  int                  myRefreshInterval;

private:
  const Registry::Components_var _VarComponents;
  static ToolsGUI_RegWidget*     myRegWidgetPtr;
};

#endif // TOOLSGUI_REGWIDGET_H

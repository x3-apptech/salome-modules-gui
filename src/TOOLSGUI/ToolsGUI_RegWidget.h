//  SALOME RegistryDisplay : GUI for Registry server implementation
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : RegWidget.h
//  Author : Pascale NOYRET, EDF
//  Module : SALOME
//  $Header$

#ifndef __REGWIDGET_H__
#define __REGWIDGET_H__
#include <qapplication.h>
#include <qmainwindow.h>

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Registry)

class QTabWidget;
class QListView;
class QListViewItem;
class QWidget;
class QTimer;
class QCloseEvent;
class QTextView;
#if QT_VERSION > 300
class QPushButton;
#endif

#include <Standard_Macro.hxx>

class ToolsGUI_HelpWindow;
class ToolsGUI_IntervalWindow;

class Standard_EXPORT  ToolsGUI_InfoWindow : public QMainWindow
{
  Q_OBJECT

public:
  ToolsGUI_InfoWindow( QWidget* parent = 0, const char* name = 0);

  void setText( const QString& text );
  QTextView* textView() const { return myTextView; }

protected:
  void keyPressEvent( QKeyEvent * e );

private:
  QTextView* myTextView;
};

class Standard_EXPORT ToolsGUI_RegWidget : public QMainWindow
{
  Q_OBJECT

public:
  ToolsGUI_RegWidget( CORBA::ORB_var &orb , QWidget *parent = 0, const char *name = 0 );
  ~ToolsGUI_RegWidget();
  void       SetListe();
  void       SetListeHistory();
  void       InfoReg();
  void       InfoHistory();

  bool       eventFilter( QObject* object, QEvent* event );

  static ToolsGUI_RegWidget* GetRegWidget( CORBA::ORB_var &orb , QWidget *parent = 0, const char *name = 0 );

public slots:
  void       slotHelp();
  void       slotListeSelect();
  void       slotClientChanged( QListViewItem* );
  void       slotHistoryChanged( QListViewItem* );
  void       slotSelectRefresh();
  void       slotIntervalOk();
protected:  
  static QString setlongText( const Registry::Infos &c_info );
  int            numitem( const QString& name, const QString& pid, const QString& machine, const Registry::AllInfos* c_info );
  void           closeEvent( QCloseEvent *e);
   
protected :
  QListView*               _clients;
  QListView*               _history;
  QWidget*                 _parent;
  QTabWidget*              _tabWidget;
  QPushButton*             _refresh;
  QPushButton*             _interval;
  QPushButton*             _close;
  QTimer*                  _counter;
  Registry::AllInfos*      _serverhistory;
  Registry::AllInfos*      _serverclients;
  ToolsGUI_InfoWindow*     myInfoWindow;
  ToolsGUI_HelpWindow*     myHelpWindow;
  ToolsGUI_IntervalWindow* myIntervalWindow;
  int                      myRefreshInterval;

private:
  const      Registry::Components_var _VarComponents;
  static     ToolsGUI_RegWidget* myRegWidgetPtr;
};

#endif

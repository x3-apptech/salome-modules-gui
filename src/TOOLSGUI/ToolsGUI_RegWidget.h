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

#ifndef TOOLSGUI_REGWIDGET_H
#define TOOLSGUI_REGWIDGET_H

#include "ToolsGUI.h"

#include <QMainWindow>
#include <QMap>

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Registry)

class QTreeWidget;
class QTreeWidgetItem;
class QTimer;
class QAction;
class QTextEdit;

class TOOLSGUI_EXPORT ToolsGUI_RegWidget : public QMainWindow
{
  Q_OBJECT

  enum { Refresh, Interval, Close };
  enum { Clients, History };

  ToolsGUI_RegWidget( CORBA::ORB_ptr orb, QWidget* parent = 0 );

public:
  ~ToolsGUI_RegWidget();

  static ToolsGUI_RegWidget* GetRegWidget( CORBA::ORB_ptr orb, 
                                           QWidget* parent = 0 );

private slots:
  void                       refresh();
  void                       showDetails( QTreeWidgetItem*, int );
  void                       refreshInterval();

private:  
  static QString             getDetails( const Registry::Infos& c_info );
   
private:
  static ToolsGUI_RegWidget* myRegWidgetPtr;

  CORBA::ORB_var             myOrb;
  QMap<int, Registry::AllInfos*> myData;
  QMap<int, QAction*>        myActions;
  QMap<int, QTreeWidget*>    myViews;
  QTextEdit*                 myDetails;
  QTimer*                    myTimer;
};

#endif // TOOLSGUI_REGWIDGET_H

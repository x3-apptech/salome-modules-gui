// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

// File:      QtxWebBrowser.h
// Author:    Roman NIKOLAEV
//

#ifndef QTXWEBBROWSER_H
#define QTXWEBBROWSER_H

#include "Qtx.h"

#include <QMainWindow>
#include <QMap>

class QAction;
class QMenu;
class QToolBar;
class QWebView;
class QUrl;
class QtxSearchTool;

class QTX_EXPORT QtxWebBrowser : public QMainWindow
{
  Q_OBJECT

  enum { File };
  enum { Find, FindNext, FindPrev, Close };

private:
  QtxWebBrowser();
  
public:
  virtual ~QtxWebBrowser();
  
  static QtxWebBrowser*           webBrowser();
  static void                     loadUrl( const QString&, const QString& = QString() );
  static void                     setData( const QString&, const QVariant& );

private:
  static QString                  getStringValue( const QString& );
  static QIcon                    getIconValue( const QString& );
  void                            updateData();
  static void                     clearData();

protected slots:
  virtual void                    linkClicked( const QUrl& );
  virtual void                    linkHovered( const QString&, const QString&, const QString& );

private slots:
  void                            adjustTitle();    
  
private:
  static QMap<QString, QVariant>  myData;
  static QtxWebBrowser*           myBrowser;
  QWebView*                       myWebView;
  QToolBar*                       myToolbar;
  QMap<int, QMenu*>               myMenus;
  QMap<int, QAction*>             myActions;
  QtxSearchTool*                  myFindPanel;
};

#endif // QTXWEBBROWSER_H

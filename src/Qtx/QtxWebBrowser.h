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

// File:      QtxWebBrowser.h
// Author:    Roman NIKOLAEV
//

#ifndef QTXWEBBROWSER_H
#define QTXWEBBROWSER_H

#include "Qtx.h"

#include <QtGlobal>
#include <QDialog>
#include <QMainWindow>
#include <QUrl>

#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
// Web Kit
#define WebView QWebView
#define WebPage QWebPage
#else
// Web Engine
#define WebView QWebEngineView
#define WebPage QWebEnginePage
#endif

class QButtonGroup;
class QCheckBox;
class QLabel;
class QPushButton;
class WebView;
class QtxResourceMgr;
class QtxSearchTool;

class QTX_EXPORT QtxWebBrowser : public QMainWindow
{
  Q_OBJECT

  class Downloader;
  class Searcher;

public:
  virtual ~QtxWebBrowser();
  
  static QtxWebBrowser*           webBrowser();
  static void                     loadUrl( const QString&, const QString& = QString() );
  static void                     setResourceMgr( QtxResourceMgr* );
  static void                     shutdown();

protected:
  QtxWebBrowser();
  QtxResourceMgr*                 resourceMgr() const;

private:
  void                            saveLink( const QString& );
  void                            openLink( const QString&, bool = false );

public Q_SLOTS:
  virtual void                    load( const QString& );

protected Q_SLOTS:
  virtual void                    about();
  virtual void                    linkClicked( const QUrl& );
  virtual void                    linkHovered( const QString&, const QString&, const QString& );
  virtual void                    linkHovered( const QString&) ;

private Q_SLOTS:
  void                            open();
  void                            adjustTitle();
  void                            finished( bool );
  void                            linkAction();
  
private:
  static QtxWebBrowser*           myBrowser;
  static QtxResourceMgr*          myResourceMgr;
  WebView*                        myWebView;
  QtxSearchTool*                  myFindPanel;
  QUrl                            myLastUrl;
};

class QtxWebBrowser::Downloader: public QDialog
{
  Q_OBJECT;
  
  enum { mOpen, mSave };

public:
  Downloader( const QString&, int = mOpen, const QString& = QString(), QWidget* = 0 );
  ~Downloader();

  int          action() const;
  bool         isRepeatAction() const;
  QString      program() const;

private Q_SLOTS:
  void         setAction( int );
  void         browse();

private:
  QString       myProgram;
  QLabel*       myFileName;
  QButtonGroup* myAction;
  QPushButton*  myBrowse;
  QCheckBox*    myRepeat;

  friend class QtxWebBrowser;
};

#endif // QTXWEBBROWSER_H

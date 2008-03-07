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
#ifndef SUIT_VIEWMANAGER_H
#define SUIT_VIEWMANAGER_H

#include "SUIT.h"
#include "SUIT_ViewWindow.h"
#include "SUIT_PopupClient.h"

#include <qobject.h>
#include <qptrvector.h>

class SUIT_Study;
class SUIT_Desktop;
class SUIT_ViewModel;

class QPopupMenu;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class SUIT_EXPORT SUIT_ViewManager : public QObject, public SUIT_PopupClient
{
  Q_OBJECT
public:
  SUIT_ViewManager( SUIT_Study*,
                    SUIT_Desktop*,
                    SUIT_ViewModel* = 0 );
  virtual ~SUIT_ViewManager();

  virtual QString  popupClientType() const { return getType(); }
  virtual void     contextMenuPopup( QPopupMenu* );
  
  void             setViewModel(SUIT_ViewModel* theViewModel);
  SUIT_ViewModel*  getViewModel() { return myViewModel; }
  
  SUIT_Study*      study() const;

  QString          getType() const; 
  SUIT_ViewWindow* getActiveView() { return myActiveView; }
  
  int              getViewsCount() { return myViews.count(); }
  QPtrVector<SUIT_ViewWindow> getViews() { return myViews; }  

  QString          getTitle() const { return myTitle; }
  virtual void     setTitle( const QString& );

  SUIT_ViewWindow* createViewWindow();

  bool             isVisible() const;
  virtual void     setShown( const bool );
  virtual void     setDestructiveClose( const bool );

  int              getId() const { return myId; }

public slots:
  void             createView();
  void             closeAllViews();
  
signals:
  void             lastViewClosed(SUIT_ViewManager*);
  void             deleteView(SUIT_ViewWindow*);
  void             viewCreated(SUIT_ViewWindow*);
  void             mousePress(SUIT_ViewWindow*, QMouseEvent*);
  void             mouseRelease(SUIT_ViewWindow*, QMouseEvent*);
  void             mouseDoubleClick(SUIT_ViewWindow*, QMouseEvent*);
  void             mouseMove(SUIT_ViewWindow*, QMouseEvent*);
  void             wheel(SUIT_ViewWindow*, QWheelEvent*);
  void             keyPress(SUIT_ViewWindow*, QKeyEvent*);
  void             keyRelease(SUIT_ViewWindow*, QKeyEvent*);
  void             activated(SUIT_ViewManager*);
  
protected slots:
  void             onWindowActivated(SUIT_ViewWindow*);
  void             onClosingView( SUIT_ViewWindow* );
  void             onMousePressed(SUIT_ViewWindow*, QMouseEvent* );
  void             onDeleteStudy();

private slots:
  void             onContextMenuRequested( QContextMenuEvent* e );
  
protected:
  /*! Inserts the View into internal Views Vector.\n
   *  Returns true if view has been added successfully
   */
  virtual bool     insertView(SUIT_ViewWindow* theView);
  
  /*! Removes the View from internal Views Vector.*/
  virtual void     removeView(SUIT_ViewWindow* theView);
  
  /*! Close the specified View.*/
  virtual void     closeView(SUIT_ViewWindow* theView);
  
  /*! Used to set unique name for the view.*/
  virtual void     setViewName(SUIT_ViewWindow* theView);
  QString          prepareTitle( const QString&, const int, const int );

  static int       useNewId( const QString& );

protected:
  SUIT_Desktop*               myDesktop;
  SUIT_ViewModel*             myViewModel;
  QPtrVector<SUIT_ViewWindow> myViews;
  SUIT_ViewWindow*            myActiveView;

  int                         myId;
  QString                     myTitle;
  SUIT_Study*                 myStudy;

  static QMap<QString, int>   _ViewMgrId;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif

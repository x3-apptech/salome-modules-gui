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

  QString          getTitle() const { return myTitle;}
  void             setTitle(QString theTitle) { myTitle = theTitle; }

  /*! Creates View, adds it into list of views and returns just created view window*/
  SUIT_ViewWindow* createViewWindow();

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
  void             onDeleteView(SUIT_ViewWindow* theView);
  void             onMousePressed(SUIT_ViewWindow* theView, QMouseEvent* theEvent);
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
  
  /*! Used to set unique name for the view.*/
  virtual void     setViewName(SUIT_ViewWindow* theView);

protected:
  SUIT_Desktop*               myDesktop;
  SUIT_ViewModel*             myViewModel;
  QPtrVector<SUIT_ViewWindow> myViews;
  SUIT_ViewWindow*            myActiveView;

  QString                     myTitle;
  SUIT_Study*                 myStudy;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif

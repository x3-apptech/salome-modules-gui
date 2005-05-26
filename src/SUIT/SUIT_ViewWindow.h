// SUIT_ViewWindow.h: interface for the SUIT_ViewWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUIT_VIEWWINDOW_H__82C3D51A_6F10_45B0_BCFE_3CB3EF596A4D__INCLUDED_)
#define AFX_SUIT_VIEWWINDOW_H__82C3D51A_6F10_45B0_BCFE_3CB3EF596A4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SUIT.h"

#include <qmainwindow.h>

class SUIT_Desktop;
class SUIT_ViewManager;

class SUIT_EXPORT SUIT_ViewWindow: public QMainWindow 
{
  Q_OBJECT
public:
  SUIT_ViewWindow(SUIT_Desktop* theDesktop);
  virtual ~SUIT_ViewWindow();

  void              setViewManager(SUIT_ViewManager* theManager) { myManager = theManager;}
  SUIT_ViewManager* getViewManager() const { return myManager; }

signals:
  void              closing(SUIT_ViewWindow*);
  void              mousePressed(SUIT_ViewWindow*, QMouseEvent*);
  void              mouseReleased(SUIT_ViewWindow*, QMouseEvent*);
  void              mouseDoubleClicked(SUIT_ViewWindow*, QMouseEvent*);
  void              mouseMoving(SUIT_ViewWindow*, QMouseEvent*);
  void              wheeling(SUIT_ViewWindow*, QWheelEvent*);
  void              keyPressed(SUIT_ViewWindow*, QKeyEvent*);
  void              keyReleased(SUIT_ViewWindow*, QKeyEvent*);
  void              contextMenuRequested( QContextMenuEvent *e );

protected:
  void              closeEvent(QCloseEvent* theEvent);
  virtual void      contextMenuEvent( QContextMenuEvent * e );

  SUIT_Desktop*     myDesktop;
  SUIT_ViewManager* myManager;
};

#endif // !defined(AFX_SUIT_VIEWWINDOW_H__82C3D51A_6F10_45B0_BCFE_3CB3EF596A4D__INCLUDED_)

// PopupEvent.h: interface for the PopupEvent class.
//
//////////////////////////////////////////////////////////////////////
#ifndef SUIT_POPUPEVENT_H
#define SUIT_POPUPEVENT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qpopupmenu.h>
#include <qevent.h> 

class SUIT_PopupEvent :public QEvent
{
public:
  SUIT_PopupEvent()
  :QEvent( QEvent::User ){};

  void         setPopupMenu(QPopupMenu* theMenu){ myPopup = theMenu; };
  QPopupMenu*  getPopupMenu(){ return myPopup; };

  void         setMouseEvent(QMouseEvent* theEvent){ myMouseEvent = theEvent; };;
  QMouseEvent* getMouseEvent(){ return myMouseEvent; };

private:
  QPopupMenu*  myPopup;
  QMouseEvent* myMouseEvent;
};

#endif // !defined(SUIT_POPUPEVENT_H)


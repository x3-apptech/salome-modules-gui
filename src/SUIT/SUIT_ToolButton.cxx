
#include "SUIT_ToolButton.h"

#include <qpopupmenu.h>
#include <qstyle.h>

SUIT_ToolButton::SUIT_ToolButton( QWidget *parent, 
                                        const char *name,
                                        bool changeItemAfterClick)
 : QToolButton(  parent, name ),
   myChangeItemAfterClick( changeItemAfterClick )
{
  initialize();
}


SUIT_ToolButton::SUIT_ToolButton( const QPixmap & pm,
                                        const QString &textLabel,
                                        const QString& grouptext,
                                        QObject * receiver,
                                        const char* slot,
                                        QToolBar * parent,
                                        const char* name,
                                        bool changeItemAfterClick)
 :QToolButton(pm, textLabel, grouptext, receiver, slot, parent, name),
  myChangeItemAfterClick( changeItemAfterClick )
{
  initialize();
}


//********************************************************************************
void SUIT_ToolButton::initialize()
{
  mySignal = NULL;
  myPopup = new QPopupMenu( this );
  setPopup(myPopup);
  connect( myPopup, SIGNAL(activated(int)), SLOT(OnSelectAction(int)) );
  setPopupDelay(250);
}

//********************************************************************************
void SUIT_ToolButton::drawButton( QPainter * p )
{
  QToolButton::drawButton(p);

//draw DownArrow
  int x, y, w, h;
  QStyle::visualRect(QRect(0, 0, width(), height()), this).rect( &x, &y, &w, &h );
  style().drawPrimitive( QStyle::PE_ArrowDown,
    p, QRect(x+w/2+3, y+h/2+3, w/2, h/2),   //QRect(x+(w-x)/2, y+(h-y)/2, w, h)
    colorGroup(), isEnabled() );
}


//********************************************************************************
void SUIT_ToolButton::AddAction(QAction* theAction)
{
  bool aIsFirst = false;
  if ( myPopup->count() == 0 ) 
  {
    aIsFirst = true;
    setPixmap(theAction->iconSet().pixmap());
    setTextLabel(theAction->text());
    theAction->addTo( myPopup );
    QMenuItem* aItem = myPopup->findItem(myPopup->idAt(0));
    if (aItem != NULL) 
    {
      mySignal = aItem->signal();
    }
  }
  else
    theAction->addTo( myPopup );
}


//********************************************************************************
void SUIT_ToolButton::SetItem(int theIndex)
{
  int anId = myPopup->idAt(theIndex);
  if (anId != -1)
  {
    // Protection against unexpected null pointers returned
    if ( myPopup->iconSet(anId) )
      setPixmap(myPopup->iconSet(anId)->pixmap());
    setTextLabel(myPopup->text(anId));
    QMenuItem* aItem = myPopup->findItem(anId);
    if (aItem != NULL) 
    {
      mySignal = aItem->signal();
    }
  }
}

//********************************************************************************
void SUIT_ToolButton::OnSelectAction(int theItemID)
{
  if (myChangeItemAfterClick)
  {
    // Protection against unexpected null pointers returned
    if ( myPopup->iconSet(theItemID) )
      setPixmap(myPopup->iconSet(theItemID)->pixmap());
    setTextLabel(myPopup->text(theItemID));
    QMenuItem* aItem = myPopup->findItem(theItemID);
    if (aItem != NULL) 
    {
      mySignal = aItem->signal();
    }
  }
}



//********************************************************************************
void SUIT_ToolButton::mouseReleaseEvent ( QMouseEvent * theEvent)
{
  QToolButton::mouseReleaseEvent(theEvent);
  if (mySignal != NULL) 
  {
    mySignal->activate();
  }
}



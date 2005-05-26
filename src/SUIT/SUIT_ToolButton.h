#ifndef SUIT_TOOLBUTTON_H
#define SUIT_TOOLBUTTON_H

#include <qtoolbutton.h>
#include <qaction.h>

#include "SUIT.h"

// Purpose:   to draw down arrow on toolbutton.

class SUIT_EXPORT SUIT_ToolButton : public QToolButton
{
  Q_OBJECT

public:
//constructors
  SUIT_ToolButton( QWidget *parent = 0, 
                   const char *name = 0,
                   bool changeItemAfterClick = true );
  SUIT_ToolButton(  const QPixmap & pm, const QString &textLabel,
                    const QString& grouptext,
                    QObject * receiver, const char* slot,
                    QToolBar * parent, const char* name = 0,
                    bool changeItemAfterClick = true );

  //drawButton is redefined to draw DownArrow
  void drawButton( QPainter * pQPainter);

  // Add action into popup
  void AddAction(QAction* theAction);

  // Sets myPopup item with theIndex as current
  void SetItem(int theIndex);

public slots:
  void OnSelectAction(int theItemID);

protected:
  void  mouseReleaseEvent (QMouseEvent * theEvent);

private:
  void initialize();

  QPopupMenu* myPopup;
  QSignal* mySignal;
  bool myChangeItemAfterClick;

};

#endif


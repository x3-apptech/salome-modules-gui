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
#ifndef SUIT_TOOLBUTTON_H
#define SUIT_TOOLBUTTON_H

#include <qtoolbutton.h>
#include <qaction.h>

#include "SUIT.h"

/*! To draw down arrow on toolbutton.*/
class SUIT_EXPORT SUIT_ToolButton : public QToolButton
{
  Q_OBJECT

public:
  /*! @name constructors*/
  //@{
  SUIT_ToolButton( QWidget *parent = 0, 
                   const char *name = 0,
                   bool changeItemAfterClick = true );
  SUIT_ToolButton(  const QPixmap & pm, const QString &textLabel,
                    const QString& grouptext,
                    QObject * receiver, const char* slot,
                    QToolBar * parent, const char* name = 0,
                    bool changeItemAfterClick = true );
  //@}

  void drawButton( QPainter * pQPainter);

  void AddAction(QAction* theAction);

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


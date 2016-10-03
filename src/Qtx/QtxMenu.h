// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:      QtxMenu.h
// Author:    Sergey TELKOV
//
#ifndef QTXMENU_H
#define QTXMENU_H

#include "Qtx.h"

#include <QMenu>

class QWidgetAction;

class QTX_EXPORT QtxMenu : public QMenu
{
  Q_OBJECT

  class Title;

public:
  //! Popup menu title mode
  typedef enum { 
    TitleAuto,        //!< auto mode
    TitleOn,          //!< always on (display title)
    TitleOff          //!< always off (do not display title)
  } TitleMode;

public:
  QtxMenu( QWidget* = 0 );
  virtual ~QtxMenu();

  QIcon                  titleIcon() const;
  QString                titleText() const;

  TitleMode              titleMode() const;
  Qt::Alignment          titleAlignment() const;

  virtual void           setTitleIcon( const QIcon& );
  virtual void           setTitleText( const QString& );

  virtual void           setTitleMode( const TitleMode );
  virtual void           setTitleAlignment( const Qt::Alignment );

  virtual void           appendGroupTitle( const QString& );

public slots:
  virtual void           setVisible( bool );

private:
  void                   updateTitle();
  void                   insertTitle();
  void                   removeTitle();

private:
  TitleMode              myMode;
  Title*                 myTitle;
  QWidgetAction*         myAction;
};

#endif // QTXMENU_H

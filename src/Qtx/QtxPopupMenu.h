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
#ifndef QTXPOPUPMENU_H
#define QTXPOPUPMENU_H

#include "Qtx.h"

#include <qstring.h>
#include <qiconset.h>
#include <qpopupmenu.h>

class QTX_EXPORT QtxPopupMenu : public QPopupMenu
{
  Q_OBJECT

protected:
  class TitleMenuItem;

public:
  enum { TitleAuto, TitleOn, TitleOff };

public:
  QtxPopupMenu( QWidget* = 0, const char* = 0 );
  virtual ~QtxPopupMenu();

  QString                titleText() const;
  QIconSet               titleIcon() const;

  int                    titlePolicy() const;
  int                    titleAlignment() const;

  virtual void           setTitleText( const QString& );
  virtual void           setTitleIcon( const QIconSet& );

  virtual void           setTitlePolicy( const int );
  virtual void           setTitleAlignment( const int );

public slots:
  virtual void           show();
  virtual void           hide();

protected:
  virtual TitleMenuItem* createTitleItem( const QString&, const QIconSet&, const int ) const;

private:
  void                   updateTitle();
  void                   insertTitle();
  void                   removeTitle();

private:
  int                    myId;
  QString                myText;
  QIconSet               myIcon;
  int                    myAlign;
  int                    myPolicy;
};

#endif

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
// File   : SUIT_MsgDlg.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#ifndef SUIT_MSGDLG_H
#define SUIT_MSGDLG_H

#include "SUIT.h"

#include <qdialog.h> 
#include <qpixmap.h>
#include <qmap.h>

class QLabel;
class QPushButton;
class QHBoxLayout;

class SUIT_EXPORT SUIT_MsgDlg : public QDialog
{
  Q_OBJECT

public:
  SUIT_MsgDlg( QWidget*, const QString&, const QString&, const QPixmap& = QPixmap() ) ;
  ~SUIT_MsgDlg();

  int  addButton( const QString&, const int = -1);
  void setPixmap( const QPixmap& );

public slots:
  void accept();

protected:
  QPushButton* button( const int ) const;
  QLabel*      iconLabel() const;
  QLabel*      messageLabel() const;

private:
  typedef QMap<QPushButton*,int> ButtonMap;

private:
  ButtonMap    myButtons;
  QHBoxLayout* myButtonLayout;
  QLabel*      myIconLab;
  QLabel*      myMsgLab;
};

#endif // SUIT_MSGDLG_H


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
//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//
//  File   : LightApp_ModuleDlg.h
//  Author : Michael ZORIN (mzn)
//  Module : SALOME

#ifndef LIGHTAPP_MODULEDLG_H
#define LIGHTAPP_MODULEDLG_H

#include "LightApp.h"
#include <qdialog.h> 
#include <qpixmap.h>

class QFrame;
class QLabel;
class QPushButton;

/*!
  \class LightApp_ModuleDlg
  Dialog allows to choose action on module activation when there is no document.
  It is possible to create new document, to open existing or to cancel module activation
*/
class LIGHTAPP_EXPORT LightApp_ModuleDlg : public QDialog
{
  Q_OBJECT

public:
  LightApp_ModuleDlg ( QWidget* parent, const QString& component, const QPixmap icon = QPixmap() ) ;
  ~LightApp_ModuleDlg ( ) { };

private slots:
  void onButtonClicked();

private:
    QFrame*      myComponentFrame;
    QLabel*      myComponentLab;
    QLabel*      myComponentIcon;
    QLabel*      myInfoLabel;
    QPushButton* myNewBtn;
    QPushButton* myOpenBtn;
    QPushButton* myLoadBtn;
    QPushButton* myCancelBtn;
};

#endif


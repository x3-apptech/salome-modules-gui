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
//  SALOME SalomeApp : implementation of desktop and GUI kernel
//
//  File   : LightApp_NameDlg.h
//  Author : Vadim SANDLER
//  Module : SALOME
//  $Header$

#ifndef LIGHTAPP_NAMEDLG_H
#define LIGHTAPP_NAMEDLG_H

#include "LightApp.h"
#include <qdialog.h>

class QLineEdit;
class QPushButton;

/*!
  \class LightApp_NameDlg
  Common <Rename> dialog box class
*/
class LIGHTAPP_EXPORT LightApp_NameDlg : public QDialog
{ 
  Q_OBJECT

public:
  LightApp_NameDlg( QWidget* parent = 0 );
  ~LightApp_NameDlg();
    
  void            setName( const QString& name );
  QString         name();
    
  static QString  getName( QWidget* parent = 0, const QString& oldName = QString::null );
    
protected slots:
  void accept();
  
private:
  QPushButton*    myButtonOk;
  QPushButton*    myButtonCancel;
  QLineEdit*      myLineEdit;
};

#endif // LightApp_NAMEDLG_H

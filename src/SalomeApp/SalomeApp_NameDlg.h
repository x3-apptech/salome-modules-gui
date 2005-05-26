//  SALOME SalomeApp : implementation of desktop and GUI kernel
//
//  Copyright (C) 2003  CEA/DEN, EDF R&D
//
//
//
//  File   : SalomeApp_NameDlg.h
//  Author : Vadim SANDLER
//  Module : SALOME
//  $Header$

#ifndef SALOMEAPP_NAMEDLG_H
#define SALOMEAPP_NAMEDLG_H

#include <qdialog.h>

class QLineEdit;
class QPushButton;

//=================================================================================
// class    : SalomeApp_NameDlg
// purpose  : Common <Rename> dialog box class
//=================================================================================
class SalomeApp_NameDlg : public QDialog
{ 
  Q_OBJECT

public:
  SalomeApp_NameDlg( QWidget* parent = 0 );
  ~SalomeApp_NameDlg();
    
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

#endif // SalomeApp_NAMEDLG_H

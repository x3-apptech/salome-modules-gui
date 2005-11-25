//  SALOME SalomeApp : implementation of desktop and GUI kernel
//
//  Copyright (C) 2003  CEA/DEN, EDF R&D
//
//
//
//  File   : QtxNameDlg.h
//  Author : Vadim SANDLER
//  Module : SALOME
//  $Header$

#ifndef QTX_NAMEDLG_H
#define QTX_NAMEDLG_H

#include "Qtx.h"
#include "QtxDialog.h"

class QLineEdit;
class QPushButton;

//=================================================================================
// class    : QtxNameDlg
/*! purpose  : Common <Rename> dialog box class*/
//=================================================================================
class QTX_EXPORT QtxNameDlg : public QtxDialog
{ 
  Q_OBJECT

public:
  QtxNameDlg( QWidget* parent = 0 );
  ~QtxNameDlg();
    
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

#endif // QTX_NAMEDLG_H

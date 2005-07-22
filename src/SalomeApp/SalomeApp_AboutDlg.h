// File:      SalomeApp_AboutDlg.h
// Created:   03.06.2005 13:49:25
// Author:    Sergey TELKOV
// Copyright (C) CEA 2005

#ifndef SALOMEAPP_ABOUTDLG_H
#define SALOMEAPP_ABOUTDLG_H

#include "SalomeApp.h"

#include <QtxDialog.h>

/*!
  Descr: Salome help about dialog
*/

class QLabel;

class SALOMEAPP_EXPORT SalomeApp_AboutDlg : public QtxDialog
{
  Q_OBJECT

public:
  SalomeApp_AboutDlg( const QString&, const QString&, QWidget* = 0 );
  virtual ~SalomeApp_AboutDlg();

protected:
  virtual void mousePressEvent( QMouseEvent* );

private:
  void checkLabel( QLabel* ) const;
  void changeFont( QWidget*, const bool = false, const bool = false,
                   const bool = false, const int = 0 ) const;
};

#endif

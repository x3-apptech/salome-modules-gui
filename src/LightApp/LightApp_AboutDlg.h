// File:      LightApp_AboutDlg.h
// Created:   03.06.2005 13:49:25
// Author:    Sergey TELKOV
// Copyright (C) CEA 2005

#ifndef LIGHTAPP_ABOUTDLG_H
#define LIGHTAPP_ABOUTDLG_H

#include "LightApp.h"

#include <QtxDialog.h>

/*!
  Descr: LightApp help about dialog
*/

class QLabel;

class LIGHTAPP_EXPORT LightApp_AboutDlg : public QtxDialog
{
  Q_OBJECT

public:
  LightApp_AboutDlg( const QString&, const QString&, QWidget* = 0 );
  virtual ~LightApp_AboutDlg();

protected:
  virtual void mousePressEvent( QMouseEvent* );

private:
  void checkLabel( QLabel* ) const;
  void changeFont( QWidget*, const bool = false, const bool = false,
                   const bool = false, const int = 0 ) const;
};

#endif

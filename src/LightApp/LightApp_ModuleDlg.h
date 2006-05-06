//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//
//  Copyright (C) 2005  CEA/DEN, EDF R&D
//
//
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


//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//
//  Copyright (C) 2005  CEA/DEN, EDF R&D
//
//
//
//  File   : SalomeApp_ModuleDlg.h
//  Author : Michael ZORIN (mzn)
//  Module : SALOME

#ifndef SALOMEAPP_MODULEDLG_H
#define SALOMEAPP_MODULEDLG_H

#include <qdialog.h> 
#include <qpixmap.h>

class QFrame;
class QLabel;
class QPushButton;

class SalomeApp_ModuleDlg : public QDialog
{
  Q_OBJECT

public:
  SalomeApp_ModuleDlg ( QWidget* parent, const QString& component, const QPixmap icon = QPixmap() ) ;
  ~SalomeApp_ModuleDlg ( ) { };

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


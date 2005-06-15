//  SALOME SalomeApp
//
//  Copyright (C) 2005  CEA/DEN, EDF R&D
//
//
//
//  File   : SalomeApp_StudyPropertiesDlg.h
//  Author : Sergey ANIKIN
//  Module : SALOME
//  $Header$

#ifndef SALOMEAPP_STUDY_PROPERTIES_DLG_H
#define SALOMEAPP_STUDY_PROPERTIES_DLG_H

#include "SalomeApp.h"
#include <SALOMEDSClient_definitions.hxx>
#include <qdialog.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlistview.h>

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS)

#include <SALOMEDSClient_Study.hxx>

class SalomeApp_ListView;
class QPushButton;
class QToolButton;

class SALOMEAPP_EXPORT SalomeApp_StudyPropertiesDlg : public QDialog
{ 
  Q_OBJECT

public:
  enum {
    prpAuthorId,
    prpModeId,
    prpDateId,
    prpSavedId,
    prpLockedId,
    prpModificationsId,
    prpLastId
  };

public:
  SalomeApp_StudyPropertiesDlg( QWidget* parent = 0 );
  ~SalomeApp_StudyPropertiesDlg();

  bool isChanged() { return myChanged; } 

public slots:
  void onOK();

private:
  void initData(); 
  bool acceptData(); 
  bool propChanged(); 
  
private:
  SalomeApp_ListView* myPropList;
  QPushButton*        myOKBtn;
  QPushButton*        myCancelBtn;
  bool                myChanged;  

  _PTR(Study)         myStudyDoc;
};
 
#endif // SALOMEAPP_STUDY_PROPERTIES_DLG_H

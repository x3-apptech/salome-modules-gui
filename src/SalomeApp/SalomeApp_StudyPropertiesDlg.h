//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  File   : SalomeApp_StudyPropertiesDlg.h
//  Author : Sergey ANIKIN
//  Module : SALOME
//
#ifndef SALOMEAPP_STUDY_PROPERTIES_DLG_H
#define SALOMEAPP_STUDY_PROPERTIES_DLG_H

#include "SalomeApp.h"

#include <QDialog>

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS)

#include <SALOMEDSClient_Study.hxx>

class SalomeApp_ListView;
class QPushButton;

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

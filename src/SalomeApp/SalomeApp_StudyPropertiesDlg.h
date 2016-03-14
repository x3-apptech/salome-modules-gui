// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
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

//  File   : SalomeApp_StudyPropertiesDlg.h
//  Author : Roman NIKOLAEV
//  Module : SALOME
//
#ifndef SALOMEAPP_STUDY_PROPERTIES_DLG_H
#define SALOMEAPP_STUDY_PROPERTIES_DLG_H

#include "SalomeApp.h"

#include <QDialog>

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS)

#include <SALOMEDSClient_Study.hxx>

class QLineEdit;
class QLabel;
class QCheckBox;
class QComboBox;
class QTextEdit;
class QTreeWidget;
class QPushButton;

class SALOMEAPP_EXPORT SalomeApp_StudyPropertiesDlg : public QDialog
{ 
  Q_OBJECT
public:
  SalomeApp_StudyPropertiesDlg( QWidget* parent = 0 );
  ~SalomeApp_StudyPropertiesDlg();

  bool isChanged() { return myIsChanged; }
  
public slots:
  void clickOnOk();
 
private:
 void initData();
  
private:
 _PTR(Study)          myStudyDoc;
 bool                 myIsChanged;
 QLineEdit*           myAuthor;
 QLabel*              myDate;
 QCheckBox*           myLocked;
 QLabel*              myModification;
 QComboBox*           myUnits;
 QTextEdit*           myComment;
 QTreeWidget*         myModifications;
 QTreeWidget*         myVersions;
 QPushButton*         myOkButton;
 QPushButton*         myCancelButton;
};
 
#endif // SALOMEAPP_STUDY_PROPERTIES_DLG_H

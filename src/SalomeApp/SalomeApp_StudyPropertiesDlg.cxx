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

//  SALOME SalomeApp
//  File   : SalomeApp_StudyPropertiesDlg.cxx
//  Author : Roman NIKOLAEV

#include "SalomeApp_StudyPropertiesDlg.h"
#include "SalomeApp_Study.h"

#include "SUIT_Session.h"
#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>

// CORBA Headers
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOMEDS_Attributes)

// QT Includes
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QTextEdit>
#include <QTreeWidget>
#include <QPushButton>

#include <iostream>
 
#define  DEFAULT_MARGIN 11
#define  DEFAULT_SPACING 6
#define     SPACER_SIZE 5
#define MIN_LIST_WIDTH  300
#define MIN_LIST_HEIGHT 150


/*!Constructor. Initialize study properties dialog.*/
SalomeApp_StudyPropertiesDlg::SalomeApp_StudyPropertiesDlg(QWidget* parent)
  : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint ),
    myIsChanged(false)
{
  setModal( true );

  setWindowTitle(tr("TLT_STUDY_PROPERTIES"));
  setSizeGripEnabled( true );

  setWindowFlags( windowFlags() & ~Qt::WindowContextHelpButtonHint );

  // Display study properties
  SalomeApp_Study* study =
    dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if (study)
    myStudyDoc = study->studyDS();

  //Author
  QLabel* authorLbl = new QLabel(tr("PRP_AUTHOR"),this);
  myAuthor = new QLineEdit(this);

  //Creation date
  QLabel* dateLbl = new QLabel(tr("PRP_DATE"),this);
  myDate = new QLabel(this);

  //Locked flag
  myLocked = new QCheckBox(tr("PRP_LOCKED"), this);

  //Modification flag
  myModification = new QLabel(this);

  //Units
  QLabel* unitsLbl = new QLabel(tr("PRP_UNITS"),this);
  myUnits = new QComboBox(this);

  //Comment
  QLabel* commentLabel = new QLabel(tr("PRP_COMMENT"),this);
  myComment = new QTextEdit(this);
  myComment->setMaximumHeight(80);

  //Modifications
  QLabel* modificationsLabel = new QLabel(tr("PRP_MODIFICATIONS"),this);  
  myModifications = new QTreeWidget(this);
  myModifications->setRootIsDecorated(false);
  myModifications->setUniformRowHeights(true);
  myModifications->setAllColumnsShowFocus(true);
  myModifications->setColumnCount(2);
  myModifications->setMaximumHeight(80);
  QStringList columnNames;
  columnNames.append(tr("PRP_AUTHOR"));
  columnNames.append(tr("PRP_DATE_MODIF"));
  myModifications->setHeaderLabels( columnNames );

  //Component versions
  QLabel* versionsLabel = new QLabel(tr("PRP_VERSIONS"),this);  
  myVersions = new QTreeWidget(this);
  myVersions->setRootIsDecorated(false);
  myVersions->setUniformRowHeights(true);
  myVersions->setAllColumnsShowFocus(true);
  myVersions->setColumnCount(2);
  myVersions->setMaximumHeight(80);
  columnNames.clear();
  columnNames.append(tr("PRP_COMPONENT"));
  columnNames.append(tr("PRP_VERSION"));
  myVersions->setHeaderLabels( columnNames );

  QFrame* buttonFrame = new QFrame(this);

  QHBoxLayout* horizontalLayout = new QHBoxLayout(buttonFrame);
  
  myOkButton = new QPushButton(tr("BUT_OK"), buttonFrame);
  myCancelButton = new QPushButton(tr("BUT_CANCEL"), buttonFrame);

  horizontalLayout->addWidget(myOkButton);
  horizontalLayout->addItem( new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum) );
  horizontalLayout->addWidget(myCancelButton);
  
  QGridLayout* gridLayout = new QGridLayout(this);
  gridLayout->setMargin(DEFAULT_MARGIN);
  gridLayout->setSpacing(DEFAULT_SPACING);
  gridLayout->addWidget(authorLbl, 0, 0, 1, 1);
  gridLayout->addWidget(myAuthor, 0, 1, 1, 1);
  gridLayout->addWidget(dateLbl, 1, 0, 1, 1);
  gridLayout->addWidget(myDate, 1, 1, 1, 1);
  gridLayout->addWidget(myLocked, 2, 0, 1, 1);
  gridLayout->addWidget(myModification, 2, 1, 1, 1);
  gridLayout->addWidget(unitsLbl, 3, 0, 1, 1);
  gridLayout->addWidget(myUnits, 3, 1, 1, 1);
  gridLayout->addItem(new QSpacerItem(/*72*/0, 0/*20*/, QSizePolicy::Expanding, QSizePolicy::Minimum), 3, 2, 1, 1);
  gridLayout->addWidget(commentLabel, 4, 0, 1, 1);
  gridLayout->addWidget(myComment, 4, 1, 1, 2);
  gridLayout->addWidget(modificationsLabel, 5, 0, 1, 1);
  gridLayout->addWidget(myModifications, 5, 1, 1, 2);
  gridLayout->addWidget(versionsLabel, 6, 0, 1, 1);
  gridLayout->addWidget(myVersions, 6, 1, 1, 2);
  gridLayout->addWidget(buttonFrame, 7, 0, 1, 3);

  connect(myOkButton,     SIGNAL(clicked()), this, SLOT(clickOnOk()));
  connect(myCancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  
  initData();
}

/*!
  Destructor.
*/
SalomeApp_StudyPropertiesDlg::~SalomeApp_StudyPropertiesDlg()
{
}

/*!
  Data initializetion for dialog.(Study author's name, date of creation etc.)
*/
void SalomeApp_StudyPropertiesDlg::initData()
{
  bool hasData = (myStudyDoc != NULL);
  _PTR(AttributeStudyProperties) propAttr;
  if (hasData)
    propAttr = myStudyDoc->GetProperties();
  hasData = hasData && propAttr;
  
  if (hasData) {
    //Creator and creation date
    myAuthor->setText(propAttr->GetUserName().c_str());
    int minutes, hours, day, month, year;
    if (propAttr->GetCreationDate(minutes, hours, day, month, year)) {
      QString strDate;
      strDate.sprintf("%2.2d/%2.2d/%2d %2.2d:%2.2d", day, month, year, hours, minutes);
      myDate->setText(strDate);
    }
    
    //Is Locked?
    myLocked->setChecked(propAttr->IsLocked());

    //Is Modified?
    bool isModified = false;
    SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>
      (SUIT_Session::session()->activeApplication()->activeStudy());
    isModified = study->isModified();
    if (propAttr->IsModified() != isModified) {
      propAttr->SetModified((int)isModified);
    }
    
    QString modif = propAttr->IsModified() ? tr( "PRP_MODIFIED" ) : tr( "PRP_NOT_MODIFIED" );
    myModification->setText(modif);
    
    //Units
    QStringList theList;
    theList <<""<<"mm"<<"cm"<<"m"<<"km"<<"inch";
    myUnits->insertItems(0,theList); 

    QString anUnit = propAttr->GetUnits().c_str();

    int aFound = -1;
    for (int i = 0; i < myUnits->count(); i++) {
      if ( myUnits->itemText(i) == anUnit ) {
        aFound = i;
        break;
      }
    }
    
    if (aFound >= 0) {
      myUnits->setCurrentIndex(aFound);
    }
    
    //Comments
    myComment->setPlainText(propAttr->GetComment().c_str());
    
    //Modifications
    std::vector<std::string> aUsers;
    std::vector<int>  aMins, aHours, aDays, aMonths, aYears;
    propAttr->GetModificationsList(aUsers, aMins, aHours, aDays, aMonths, aYears, false);
    int aCnt = aUsers.size();
    for ( int i = 0; i < aCnt; i++ ) {
      QString date;
      date.sprintf("%2.2d/%2.2d/%2d %2.2d:%2.2d",
                  aDays  [i],
                  aMonths[i],
                  aYears [i],
                  aHours [i],
                  aMins  [i]);
      QStringList aList;
      aList.append(aUsers[i].c_str());
      aList.append(date);
      QTreeWidgetItem* item = new  QTreeWidgetItem(aList);
      myModifications->addTopLevelItem(item);
    }        
  
    //Component versions
    std::vector<std::string> aComponents = propAttr->GetStoredComponents();
    for ( int i = 0; i < aComponents.size(); i++ ) {
      std::vector<int>  aMins, aHours, aDays, aMonths, aYears;
      std::vector<std::string> aVersions = propAttr->GetComponentVersions(aComponents[i]);
      QStringList aData;
      aData.append( aComponents[i].c_str() );
      aData.append( aVersions.empty() || aVersions[0] == "" ? "unknown" : aVersions[0].c_str() );
      QTreeWidgetItem* item = new QTreeWidgetItem( aData );
      if ( aVersions.size() > 1 )
	item->setForeground( 1, Qt::red );
      myVersions->addTopLevelItem(item);
    }
  }
  adjustSize();
}

/*!
  Fill properties attributes.
*/
void SalomeApp_StudyPropertiesDlg::clickOnOk()
{
  _PTR(AttributeStudyProperties) propAttr = myStudyDoc->GetProperties();
  //Firstly, store locked flag
  if(propAttr) {
    bool bLocked = myLocked->isChecked();
    if (propAttr->IsLocked() != bLocked) {
      propAttr->SetLocked(bLocked);
      myIsChanged = true;
    }
    
    bool needWarning = false;
    
    //Author
    if (QString(propAttr->GetUserName().c_str()) != myAuthor->text().trimmed()) {
      if(!propAttr->IsLocked()) {
        propAttr->SetUserName(myAuthor->text().trimmed().toStdString());
        myIsChanged = true;
      } else {
        needWarning = true;
      }
    }

    //Unit
    if (QString(propAttr->GetUnits().c_str()) != myUnits->currentText()) {
      if(!propAttr->IsLocked()) {
        propAttr->SetUnits(myUnits->currentText().toStdString());
        myIsChanged = true;
      } else {
        needWarning = true;
      }
    }
      
    //Comment      
    if (QString(propAttr->GetComment().c_str()) != myComment->toPlainText()) {
      if(!propAttr->IsLocked()) {
        propAttr->SetComment(myComment->toPlainText().toStdString());
        myIsChanged = true;
      } else {
        needWarning = true;
      }
    }
    
    if(needWarning) {
      SUIT_MessageBox::warning(SUIT_Session::session()->activeApplication()->desktop(),
                               QObject::tr("WRN_WARNING"),
                               QObject::tr("WRN_STUDY_LOCKED") );
    }
    
    accept();
  }
}

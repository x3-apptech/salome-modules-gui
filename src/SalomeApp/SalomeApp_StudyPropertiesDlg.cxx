//  SALOME SalomeApp
//
//  Copyright (C) 2005  CEA/DEN, EDF R&D
//
//
//
//  File   : SalomeApp_StudyPropertiesDlg.cxx
//  Author : Sergey ANIKIN
//  Module : SALOME
//  $Header$

#include "SalomeApp_StudyPropertiesDlg.h"
#include "SalomeApp_ListView.h"
#include "SalomeApp_Study.h"

#include "SUIT_Session.h"

#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

#include <qpushbutton.h>
#include <qlayout.h>
using namespace std;

#define  DEFAULT_MARGIN 11
#define DEFAULT_SPACING 6
#define     SPACER_SIZE 5
#define MIN_LIST_WIDTH  300
#define MIN_LIST_HEIGHT 150

class SalomeApp_PropItem : public SalomeApp_ListViewItem
{
public:
// constructor  
  SalomeApp_PropItem(SalomeApp_ListView* parent,
		     const QString       theName,
		     const bool          theEditable,
		     const int           theUserType) :
  SalomeApp_ListViewItem( parent, theName, theEditable )
  {
    setUserType(theUserType);
  }
// constructor
  SalomeApp_PropItem(SalomeApp_ListView*     parent,
		     SalomeApp_ListViewItem* after,
		     const QString     theName,
		     const bool        theEditable,
		     const int         theUserType) :
  SalomeApp_ListViewItem( parent, after, theName, theEditable )
  {
    setUserType(theUserType);
  }
// fills widget with initial values (list or single value)
  void fillWidgetWithValues( SalomeApp_EntityEdit* theWidget )
  {
    QStringList list;
    switch(getUserType()) {
    case SalomeApp_StudyPropertiesDlg::prpModeId:
      {
	list << QObject::tr("PRP_MODE_FROM_SCRATCH") << 
	        QObject::tr("PRP_MODE_FROM_COPYFROM");
	theWidget->insertList(list);
	break;
      }
    case SalomeApp_StudyPropertiesDlg::prpLockedId:
      {
	list << QObject::tr("PRP_NO") << QObject::tr("PRP_YES");
	theWidget->insertList(list, getValue() == QObject::tr("PRP_NO") ? 0 : 1);    
	break;
      }
    case SalomeApp_StudyPropertiesDlg::prpModificationsId:
      {
	SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
	if (study) { 
	  _PTR(Study) studyDoc = study->studyDS();
	  _PTR(AttributeStudyProperties) propAttr;
	  if ( studyDoc ) {
	    propAttr = studyDoc->GetProperties();
	    if ( propAttr ) {
	      std::vector<std::string> aUsers;
	      std::vector<int>  aMins, aHours, aDays, aMonths, aYears;
	      propAttr->GetModificationsList(aUsers, aMins, aHours, aDays, aMonths, aYears, false);
	      int aCnt = aUsers.size();
	      for ( int i = 0; i < aCnt; i++ ) {
		QString val;
		val.sprintf("%2.2d/%2.2d/%2d %2.2d:%2.2d", 
			    aDays  [i], 
			    aMonths[i], 
			    aYears [i], 
			    aHours [i], 
			    aMins  [i]);
		val = val + " : " + QString( aUsers[i].c_str() );
		list.prepend(val);
	      }
	      theWidget->setDuplicatesEnabled(true);
	      theWidget->insertList(list);    
	    }
	  }
	}
	break;
      }
    default:
      {
	SalomeApp_ListViewItem::fillWidgetWithValues(theWidget);
	break;
      }
    }
  }
// finishes editing of entity
  virtual UpdateType finishEditing( SalomeApp_EntityEdit* theWidget ) {
    if ( getUserType() == SalomeApp_StudyPropertiesDlg::prpModificationsId )
      return utCancel;
    else
      return SalomeApp_ListViewItem::finishEditing(theWidget);
  }
};

SalomeApp_StudyPropertiesDlg::SalomeApp_StudyPropertiesDlg(QWidget* parent)
     : QDialog(parent, "", TRUE, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu ),
       myChanged( false )
{
  setCaption(tr("TLT_STUDY_PROPERTIES"));
  setSizeGripEnabled( true );

  clearWFlags(Qt::WStyle_ContextHelp);

  QGridLayout* mainLayout = new QGridLayout(this);
  mainLayout->setMargin(DEFAULT_MARGIN);
  mainLayout->setSpacing(DEFAULT_SPACING);
  
  myPropList = new SalomeApp_ListView(this);
  myPropList->addColumn("");
  myPropList->addColumn("");
  myPropList->enableEditing(TRUE);
  myPropList->setMinimumSize(MIN_LIST_WIDTH, MIN_LIST_HEIGHT);
  mainLayout->addMultiCellWidget(myPropList, 0, 0, 0, 2);

  myOKBtn = new QPushButton(tr("BUT_OK"), this);
  mainLayout->addWidget(myOKBtn, 1, 0);

  myCancelBtn = new QPushButton(tr("BUT_CANCEL"), this);
  mainLayout->addWidget(myCancelBtn, 1, 2);

  QSpacerItem* spacer1 = new QSpacerItem(SPACER_SIZE, SPACER_SIZE, QSizePolicy::Expanding, QSizePolicy::Minimum);
  mainLayout->addItem(spacer1, 1, 1);

  // Display study properties
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if (study)
    myStudyDoc = study->studyDS();

  initData();

  connect(myOKBtn,     SIGNAL(clicked()), this, SLOT(onOK()));
  connect(myCancelBtn, SIGNAL(clicked()), this, SLOT(reject()));
}

SalomeApp_StudyPropertiesDlg::~SalomeApp_StudyPropertiesDlg()
{
}

void SalomeApp_StudyPropertiesDlg::initData()
{
  bool hasData = myStudyDoc;
  _PTR(AttributeStudyProperties) propAttr;
  if (hasData) 
    propAttr = myStudyDoc->GetProperties();
  hasData = hasData && propAttr;

  // Study author's name
  SalomeApp_PropItem* item = new SalomeApp_PropItem(myPropList, tr("PRP_AUTHOR")+":", true, prpAuthorId);
  if (hasData)
    item->setValue(propAttr->GetUserName().c_str());

  // Date of creation
  item = new SalomeApp_PropItem(myPropList, item, tr("PRP_DATE")+":", false, prpDateId);
  if (hasData) {
    int minutes, hours, day, month, year;
    if (propAttr->GetCreationDate(minutes, hours, day, month, year)) {
      QString strDate;
      strDate.sprintf("%2.2d/%2.2d/%2d %2.2d:%2.2d", day, month, year, hours, minutes);
      item->setValue(strDate);
    }
  }
  
  // Creation mode
//  item = new SalomeApp_PropItem(myPropList, item, tr("PRP_MODE")+":", true, prpModeId);
//  item->setEditingType( SalomeApp_EntityEdit::etComboBox);
//  if (hasData) item->setValue(propAttr->GetCreationMode());

  // Locked or not
  item = new SalomeApp_PropItem(myPropList, item, tr("PRP_LOCKED")+":", true, prpLockedId);
  item->setEditingType( SalomeApp_EntityEdit::etComboBox);  
  if (hasData) (propAttr->IsLocked()) ? item->setValue(tr("PRP_YES")) : item->setValue(tr("PRP_NO"));

  // Saved or not
  item = new SalomeApp_PropItem(myPropList, item, tr("PRP_MODIFIED")+":", false, prpSavedId);
  if (hasData) {
    if (propAttr->IsModified())
      item->setValue(tr("PRP_YES"));
    else
      item->setValue(tr("PRP_NO"));
  }

  // Modifications list
  item = new SalomeApp_PropItem(myPropList, item, tr("PRP_MODIFICATIONS")+":", true, prpModificationsId); 
  item->setEditingType( SalomeApp_EntityEdit::etComboBox);  
  if (hasData) { 
    std::vector<std::string> aUsers;
    std::vector<int> aMins, aHours, aDays, aMonths, aYears;
    propAttr->GetModificationsList(aUsers, aMins, aHours, aDays, aMonths, aYears, false);
    int aLast = aUsers.size()-1;
    if (aLast >= 0) {
      QString val;
      val.sprintf("%2.2d/%2.2d/%2d %2.2d:%2.2d", 
		  aDays  [aLast], 
		  aMonths[aLast], 
		  aYears [aLast], 
		  aHours [aLast], 
		  aMins  [aLast]);
      val = val + " : " + QString(aUsers[aUsers.size()-1].c_str());
      item->setValue(val);
    }
  }

  myOKBtn->setEnabled(hasData);
}

bool SalomeApp_StudyPropertiesDlg::acceptData()
{
  return TRUE;
}

void SalomeApp_StudyPropertiesDlg::onOK()
{
  myPropList->accept();

  if (acceptData()) {
    _PTR(AttributeStudyProperties) propAttr = myStudyDoc->GetProperties();
    myChanged = propChanged();
    if ( propAttr && myChanged ) {
      QListViewItemIterator it( myPropList );
      // iterate through all items of the listview
      for ( ; it.current(); ++it ) {
	SalomeApp_PropItem* item = (SalomeApp_PropItem*)(it.current());
	switch (item->getUserType()) {
	case prpAuthorId:
	  propAttr->SetUserName(item->getValue().stripWhiteSpace().latin1());
	  break;
	case prpModeId:
	  propAttr->SetCreationMode(item->getValue().stripWhiteSpace().latin1());
	  break;
	case prpLockedId:
	  propAttr->SetLocked(item->getValue().compare(tr("PRP_YES")) == 0);
	  break;
	default:
	  break;
	}
      }
    }
    accept();
  }
}

bool SalomeApp_StudyPropertiesDlg::propChanged() {
  _PTR(AttributeStudyProperties) propAttr = myStudyDoc->GetProperties();
  if ( propAttr ) {
    QListViewItemIterator it( myPropList );
    // iterate through all items of the listview
    for ( ; it.current(); ++it ) {
      SalomeApp_PropItem* item = (SalomeApp_PropItem*)(it.current());
      switch (item->getUserType()) {
      case prpAuthorId:
	if ( QString( propAttr->GetUserName().c_str() ) != item->getValue().stripWhiteSpace() ) {
	  return true;
	}
	break;
      case prpModeId:
	if ( QString( propAttr->GetCreationMode().c_str() ) != item->getValue().stripWhiteSpace() ) {
	  return true;
        }
	break;
      case prpLockedId:
	{
	  bool bLocked = item->getValue().compare( tr( "PRP_YES" ) ) == 0;
	  if ( propAttr->IsLocked() != bLocked ) {
	    return true;
	  }
	  break;
	}
      default:
	break;
      }
    }
  }
  return false;
}

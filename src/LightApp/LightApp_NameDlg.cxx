// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
//  File   : LightApp_NameDlg.cxx
//  Author : Vadim SANDLER
//  $Header$

#include <LightApp_NameDlg.h>
#include <SUIT_Application.h>
#include <SUIT_Desktop.h>
#include <SUIT_Tools.h>

#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>

#ifndef WIN32
using namespace std;
#endif

/*!
  Constructor
*/
LightApp_NameDlg::LightApp_NameDlg( QWidget* parent )
: QDialog( parent ? parent : NULL,//application()->desktop(), 
"LightApp_NameDlg",
true,
WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
  setCaption( tr("TLT_RENAME") );
  setSizeGripEnabled( TRUE );

  QVBoxLayout* topLayout = new QVBoxLayout( this );
  topLayout->setMargin( 11 ); topLayout->setSpacing( 6 );

  /***************************************************************/
  QGroupBox* GroupC1 = new QGroupBox( this, "GroupC1" );
  GroupC1->setColumnLayout(0, Qt::Vertical );
  GroupC1->layout()->setMargin( 0 ); GroupC1->layout()->setSpacing( 0 );
  QHBoxLayout* GroupC1Layout = new QHBoxLayout( GroupC1->layout() );
  GroupC1Layout->setAlignment( Qt::AlignTop );
  GroupC1Layout->setMargin( 11 ); GroupC1Layout->setSpacing( 6 );
  
  QLabel* TextLabel = new QLabel( GroupC1, "TextLabel1" );
  TextLabel->setText( tr( "NAME_LBL" ) );
  GroupC1Layout->addWidget( TextLabel );
  
  myLineEdit = new QLineEdit( GroupC1, "LineEdit1" );
  myLineEdit->setMinimumSize( 250, 0 );
  GroupC1Layout->addWidget( myLineEdit );
  
  /***************************************************************/
  QGroupBox* GroupButtons = new QGroupBox( this, "GroupButtons" );
  GroupButtons->setColumnLayout(0, Qt::Vertical );
  GroupButtons->layout()->setMargin( 0 ); GroupButtons->layout()->setSpacing( 0 ); 
  QHBoxLayout* GroupButtonsLayout = new QHBoxLayout( GroupButtons->layout() );
  GroupButtonsLayout->setAlignment( Qt::AlignTop );
  GroupButtonsLayout->setMargin( 11 ); GroupButtonsLayout->setSpacing( 6 );
  
  myButtonOk = new QPushButton( GroupButtons, "buttonOk" );
  myButtonOk->setText( tr( "BUT_OK"  ) );
  myButtonOk->setAutoDefault( TRUE ); myButtonOk->setDefault( TRUE );
  GroupButtonsLayout->addWidget( myButtonOk );

  GroupButtonsLayout->addStretch();
  
  myButtonCancel = new QPushButton( GroupButtons, "buttonCancel" );
  myButtonCancel->setText( tr( "BUT_CANCEL"  ) );
  myButtonCancel->setAutoDefault( TRUE );
  GroupButtonsLayout->addWidget( myButtonCancel );
  /***************************************************************/
  
  topLayout->addWidget( GroupC1 );
  topLayout->addWidget( GroupButtons );
  
  // signals and slots connections
  connect( myButtonOk,     SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( myButtonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  
  /* Move widget on the botton right corner of main widget */
  SUIT_Tools::centerWidget( this, parent );
}

/*!
  Destructor
*/
LightApp_NameDlg::~LightApp_NameDlg()
{
}

/*!
  Sets name
*/
void LightApp_NameDlg::setName( const QString& name )
{
  myLineEdit->setText( name );
  myLineEdit->end(false);
  myLineEdit->home(true);
}

/*!
  Returns name entered by user
*/
QString LightApp_NameDlg::name()
{
  return myLineEdit->text();
}

void LightApp_NameDlg::accept()
{
  if ( name().stripWhiteSpace().isEmpty() )
    return;
  QDialog::accept();
}

/*!
  Creates modal <Rename> dialog and returns name entered [ static ]
*/
QString LightApp_NameDlg::getName( QWidget* parent, const QString& oldName )
{
  QString n;
  LightApp_NameDlg* dlg = new LightApp_NameDlg( parent );
  if ( !oldName.isNull() )
    dlg->setName( oldName );
  if ( dlg->exec() == QDialog::Accepted ) 
    n = dlg->name();
  delete dlg;
  return n;
}

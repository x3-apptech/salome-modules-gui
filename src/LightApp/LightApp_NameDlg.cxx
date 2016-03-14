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

// File   : LightApp_NameDlg.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "LightApp_NameDlg.h"
#include <SUIT_Tools.h>

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

/*!
  Constructor
*/
LightApp_NameDlg::LightApp_NameDlg( QWidget* parent )
: QDialog( parent ? parent : NULL,//application()->desktop(), 
Qt::WindowTitleHint | Qt::WindowSystemMenuHint )
{
  setObjectName( "LightApp_NameDlg" );
  setModal( true );

  setWindowTitle( tr("TLT_RENAME") );
  setSizeGripEnabled( true );

  QVBoxLayout* topLayout = new QVBoxLayout( this );
  topLayout->setMargin( 11 ); topLayout->setSpacing( 6 );

  /***************************************************************/
  QGroupBox* GroupC1 = new QGroupBox( this );
  GroupC1->setObjectName( "GroupC1" );
  QHBoxLayout* GroupC1Layout = new QHBoxLayout( GroupC1 );
  GroupC1Layout->setAlignment( Qt::AlignTop );
  GroupC1Layout->setMargin( 11 ); GroupC1Layout->setSpacing( 6 );
  
  QLabel* TextLabel = new QLabel( GroupC1 );
  TextLabel->setObjectName( "TextLabel1" );
  TextLabel->setText( tr( "NAME_LBL" ) );
  GroupC1Layout->addWidget( TextLabel );
  
  myLineEdit = new QLineEdit( GroupC1 );
  myLineEdit->setObjectName( "LineEdit1" );
  myLineEdit->setMinimumSize( 250, 0 );
  GroupC1Layout->addWidget( myLineEdit );
  
  /***************************************************************/
  QGroupBox* GroupButtons = new QGroupBox( this );
  GroupButtons->setObjectName( "GroupButtons" );
  QHBoxLayout* GroupButtonsLayout = new QHBoxLayout( GroupButtons );
  GroupButtonsLayout->setAlignment( Qt::AlignTop );
  GroupButtonsLayout->setMargin( 11 ); GroupButtonsLayout->setSpacing( 6 );
  
  myButtonOk = new QPushButton( GroupButtons );
  myButtonOk->setObjectName( "buttonOk" );
  myButtonOk->setText( tr( "BUT_OK"  ) );
  myButtonOk->setAutoDefault( true ); myButtonOk->setDefault( true );
  GroupButtonsLayout->addWidget( myButtonOk );

  GroupButtonsLayout->addStretch();
  
  myButtonCancel = new QPushButton( GroupButtons );
  myButtonCancel->setObjectName( "buttonCancel" );
  myButtonCancel->setText( tr( "BUT_CANCEL"  ) );
  myButtonCancel->setAutoDefault( true );
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

/*!
  Accepts if name isn't empty
*/
void LightApp_NameDlg::accept()
{
  if ( name().trimmed().isEmpty() )
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

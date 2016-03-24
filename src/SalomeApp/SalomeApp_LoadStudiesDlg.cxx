// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "SalomeApp_LoadStudiesDlg.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QStringList>

#define SPACING_SIZE             6
#define MARGIN_SIZE             11
#define MIN_LISTBOX_WIDTH      150
#define MIN_LISTBOX_HEIGHT     100

/*!
  \class SalomeApp_LoadStudiesDlg
  \brief Dialog box which allows selecting study to be loaded 
  from the list.
*/

/*!
  \brief Constructor
  \param parent a parent widget
  \param studies list of study names
*/
SalomeApp_LoadStudiesDlg::SalomeApp_LoadStudiesDlg( QWidget* parent, const QStringList& studies )
: QDialog( parent )
{
  setModal( true );

  setWindowTitle( tr("DLG_LOAD_STUDY_CAPTION") );
  setSizeGripEnabled( true );

  QVBoxLayout* topLayout = new QVBoxLayout( this );
  topLayout->setMargin( MARGIN_SIZE );
  topLayout->setSpacing( SPACING_SIZE );
  
  QLabel* lab = new QLabel( tr( "MEN_STUDIES_CHOICE" ), this );
  
  myButtonOk = new QPushButton( tr( "BUT_OK" ), this );
  myButtonOk->setAutoDefault( true );
  myButtonOk->setDefault( true );
    
  QPushButton* buttonCancel = new QPushButton( tr( "BUT_CANCEL" ), this );
  
  QHBoxLayout* btnLayout = new QHBoxLayout;
  btnLayout->setSpacing( SPACING_SIZE );
  btnLayout->setMargin( 0 );
  btnLayout->addWidget( myButtonOk );
  btnLayout->addStretch();
  btnLayout->addWidget( buttonCancel );

  myList = new QListWidget( this );
  myList->setMinimumSize( MIN_LISTBOX_WIDTH, MIN_LISTBOX_HEIGHT );
  myList->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, 
                                               QSizePolicy::Expanding ) );
  myList->setSelectionMode( QAbstractItemView::SingleSelection );

  topLayout->addWidget( lab );
  topLayout->addWidget( myList );
  topLayout->addLayout( btnLayout );
  
  connect( myButtonOk,   SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( myList,       SIGNAL( itemDoubleClicked( QListWidgetItem* ) ),
                                              this, SLOT( accept() ) );
  connect( myList,       SIGNAL( itemSelectionChanged() ),
                                              this, SLOT( updateState() ) );
  myList->addItems( studies );

  updateState();
}

/*!
  \brief Destructor
*/
SalomeApp_LoadStudiesDlg::~SalomeApp_LoadStudiesDlg()
{
}

/*!
  \brief Updates buttons state.
*/
void SalomeApp_LoadStudiesDlg::updateState()
{
  myButtonOk->setEnabled( myList->currentItem() != 0 );
}

/*!
  \brief Get selected study name
  \return selected study name or null string if study is not selected
*/
QString SalomeApp_LoadStudiesDlg::selectedStudy()
{
  QString study;
  if ( myList->currentItem() )
    study = myList->currentItem()->text();
  return study;
}

/*!
  \brief Executes dialog box to select study from the list 
         and returns the study selected.
  \param parent parent widget
  \param studies list of study names
  \return select study (or null string if dialog box is rejected)
*/
QString SalomeApp_LoadStudiesDlg::selectStudy( QWidget* parent, const QStringList& studies )
{
  SalomeApp_LoadStudiesDlg dlg( parent, studies );
  QString study;
  if ( dlg.exec() == QDialog::Accepted )
    study = dlg.selectedStudy();
  return study;
}

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

// File:      SalomeApp_ExitDlg.cxx
// Author:    Margarita KARPUNINA, Open CASCADE S.A.S.
//
#include "SalomeApp_ExitDlg.h"

#include <QLabel> 
#include <QVBoxLayout> 
#include <QHBoxLayout> 
#include <QGridLayout> 
#include <QPushButton>
#include <QMessageBox>
#include <QCheckBox>

/*!
 * \brief creates a Exit dialog box
 * \param parent a parent widget
 * \param modal bool argument, if true the dialog box is a modal dialog box
 * \param f style flags
 */
SalomeApp_ExitDlg::SalomeApp_ExitDlg( QWidget* parent )
  : QDialog( parent )
{
  setModal( true );
  setWindowTitle( tr( "INF_DESK_EXIT" ) );

  QVBoxLayout* m_vbL = new QVBoxLayout( this );
  m_vbL->setMargin( 11 );
  m_vbL->setSpacing( 6 );

  QLabel* m_lIcon = new QLabel( this );
  QPixmap pm = QMessageBox::standardIcon( QMessageBox::Question );
  m_lIcon->setPixmap( pm );
  m_lIcon->setScaledContents( false );
  m_lIcon->setAlignment( Qt::AlignCenter );

  QLabel* m_lDescr = new QLabel( this );
  m_lDescr->setText( tr( "QUE_DESK_EXIT" ) );
  m_lDescr->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

  myServersShutdown = new QCheckBox( tr( "SHUTDOWN_SERVERS" ), this );
  myServersShutdown->setChecked( true );
  
  QVBoxLayout* m_vl1 = new QVBoxLayout();
  m_vl1->setMargin( 10 ); m_vl1->setSpacing( 16 );
  m_vl1->addWidget( m_lDescr );
  m_vl1->addWidget( myServersShutdown );

  QHBoxLayout* m_hl1 = new QHBoxLayout();
  m_hl1->setMargin( 0 ); m_hl1->setSpacing( 6 );
  m_hl1->addWidget( m_lIcon );
  m_hl1->addStretch(); 
  m_hl1->addLayout( m_vl1 );
  m_hl1->addStretch();

  QPushButton* m_pbOk     = new QPushButton( tr( "BUT_OK" ),     this );
  QPushButton* m_pbCancel = new QPushButton( tr( "BUT_CANCEL" ), this );

  QGridLayout* m_hl2 = new QGridLayout();
  m_hl2->setMargin( 0 ); m_hl2->setSpacing( 6 );
  m_hl2->addWidget( m_pbOk, 0, 0 );
  m_hl2->setColumnStretch( 1, 5 );
  m_hl2->addWidget( m_pbCancel, 0, 2 );
  
  m_vbL->addStretch();
  m_vbL->addLayout( m_hl1 );
  m_vbL->addStretch();
  m_vbL->addLayout( m_hl2 );

  connect( m_pbOk,     SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( m_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*!
 * \brief Destructor
 */
SalomeApp_ExitDlg::~SalomeApp_ExitDlg()
{
}

/*!
 * \brief get the check box status
 */
bool SalomeApp_ExitDlg::isServersShutdown()
{
  return myServersShutdown->isChecked();
}
 

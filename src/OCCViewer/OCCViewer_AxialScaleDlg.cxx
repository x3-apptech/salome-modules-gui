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

#include "OCCViewer_AxialScaleDlg.h"
#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewPort3d.h"
#include "OCCViewer_ViewModel.h"

#include <QtxDoubleSpinBox.h>

#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

/*!
  \class OCCViewer_AxialScaleDlg
  \brief Dialog allowing to assign parameters of axes scaling
*/

/*!
  \brief Constructor
  \param view - view window
  \param parent - parent widget
*/
OCCViewer_AxialScaleDlg::OCCViewer_AxialScaleDlg( OCCViewer_ViewWindow* view, OCCViewer_Viewer* model )
  : QDialog( view ),
    myView( view ),
    myModel( model )
{
  setWindowTitle( tr( "DLG_SCALING" ) );
  setModal( false );
  setSizeGripEnabled( true );

  // Create layout for this dialog
  QVBoxLayout* layoutDlg = new QVBoxLayout( this );
  layoutDlg->setSpacing( 6 );
  layoutDlg->setMargin( 11 );

  // Create croup box with grid layout
  QGroupBox* aGroupBox = new QGroupBox( this );
  QHBoxLayout* aHBoxLayout = new QHBoxLayout( aGroupBox );
  aHBoxLayout->setMargin( 11 );
  aHBoxLayout->setSpacing( 6 );

  // "X" scaling
  QLabel* TextLabelX = new QLabel( tr( "LBL_X" ), aGroupBox );
  m_sbXcoeff = new QtxDoubleSpinBox( 1e-7, RealLast(), 0.1, aGroupBox );
  m_sbXcoeff->setMinimumWidth( 80 );
  m_sbXcoeff->setValue( 1.0 );

  // "Y" scaling
  QLabel* TextLabelY = new QLabel( tr( "LBL_Y" ), aGroupBox );
  m_sbYcoeff = new QtxDoubleSpinBox( 1e-7, RealLast(), 0.1, aGroupBox );
  m_sbYcoeff->setMinimumWidth( 80 );
  m_sbYcoeff->setValue( 1.0 );

  // "Z" scaling
  QLabel* TextLabelZ = new QLabel( tr( "LBL_Z" ), aGroupBox );
  m_sbZcoeff = new QtxDoubleSpinBox( 1e-7, RealLast(), 0.1, aGroupBox );
  m_sbZcoeff->setMinimumWidth( 80 );
  m_sbZcoeff->setValue( 1.0 );

  // Create <Reset> button
  m_bReset = new QPushButton( tr( "&Reset" ), aGroupBox );

  // Layout widgets in the group box
  aHBoxLayout->addWidget( TextLabelX );
  aHBoxLayout->addWidget( m_sbXcoeff );
  aHBoxLayout->addWidget( TextLabelY );
  aHBoxLayout->addWidget( m_sbYcoeff );
  aHBoxLayout->addWidget( TextLabelZ );
  aHBoxLayout->addWidget( m_sbZcoeff );
  aHBoxLayout->addSpacing( 10 );
  aHBoxLayout->addWidget( m_bReset );

  // OK, CANCEL, Apply button
  QGroupBox* aGroupBox2 = new QGroupBox( this );
  QHBoxLayout* aHBoxLayout2 = new QHBoxLayout( aGroupBox2 );
  aHBoxLayout2->setMargin( 11 );
  aHBoxLayout2->setSpacing( 6 );
  // Create <OK> button
  QPushButton* m_bOk = new QPushButton( tr( "O&K" ), aGroupBox2 );
  m_bOk->setDefault( true );
  m_bOk->setAutoDefault( true );
  // Create <Apply> button
  QPushButton* m_bApply = new QPushButton( tr( "&Apply" ), aGroupBox2 );
  m_bApply->setAutoDefault( true );
  // Create <Cancel> button
  QPushButton* m_bCancel = new QPushButton( tr( "&Cancel" ), aGroupBox2 );
  m_bCancel->setAutoDefault( true );

  // Layout buttons
  aHBoxLayout2->addWidget( m_bOk );
  aHBoxLayout2->addWidget( m_bApply );
  aHBoxLayout2->addSpacing( 10 );
  aHBoxLayout2->addStretch();
  aHBoxLayout2->addWidget( m_bCancel );

  // Layout top level widgets
  layoutDlg->addWidget( aGroupBox );
  layoutDlg->addWidget( aGroupBox2 );

  // signals and slots connections
  connect( m_bCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( m_bOk,     SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( m_bApply,  SIGNAL( clicked() ), this, SLOT( apply() ) );
  connect( m_bReset,  SIGNAL( clicked() ), this, SLOT( reset() ) );

  connect( view,      SIGNAL( Hide( QHideEvent* ) ), this, SLOT( hide() ) );

  resize( minimumSizeHint() );
}

/*!
  \brief Destructor
*/
OCCViewer_AxialScaleDlg::~OCCViewer_AxialScaleDlg()
{
  myModel = 0;
}

/*!
  \brief Update widgets values from the view
*/
void OCCViewer_AxialScaleDlg::Update()
{
  // Get values from the OCC view
  double aScaleFactor[3];
  myView->getViewPort()->getAxialScale( aScaleFactor[0], aScaleFactor[1], aScaleFactor[2] );
  m_sbXcoeff->setValue( aScaleFactor[0] );
  m_sbYcoeff->setValue( aScaleFactor[1] );
  m_sbZcoeff->setValue( aScaleFactor[2] );
}

/*!
  \brief Called when <OK> button is pressed
*/
void OCCViewer_AxialScaleDlg::accept()
{
  if ( apply() )
    QDialog::accept();
}

/*!
  \brief Called when <Apply> button is pressed
*/
bool OCCViewer_AxialScaleDlg::apply()
{
  double aScaleFactor[3] = { m_sbXcoeff->value(), m_sbYcoeff->value(), m_sbZcoeff->value() };
  myView->getViewPort()->setAxialScale( aScaleFactor[0], aScaleFactor[1], aScaleFactor[2] );
  
  if( myModel && !myModel->getViewer3d().IsNull() ){
    myModel->getViewer3d()->Update();
  }
  
  return true;
}

/*!
  \brief Called when <Reset> button is pressed
*/
void OCCViewer_AxialScaleDlg::reset()
{
  m_bReset->setFocus();
  m_sbXcoeff->setValue( 1.0 );
  m_sbYcoeff->setValue( 1.0 );
  m_sbZcoeff->setValue( 1.0 );
}

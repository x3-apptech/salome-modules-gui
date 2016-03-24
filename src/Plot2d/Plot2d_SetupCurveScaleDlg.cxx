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

// File   : Plot2d_SetupCurveScaleDlg.cxx
//
#include "Plot2d_SetupCurveScaleDlg.h"

#include <SUIT_Tools.h>

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

#include <QtxDoubleSpinBox.h>

const int MARGIN_SIZE     = 11;
const int SPACING_SIZE    = 6;
const int MIN_COMBO_WIDTH = 100;
const int MIN_SPIN_WIDTH  = 50;

/*!
  \class Plot2d_SetupCurveScaleDlg
  \brief Dialog box for modifying 2d curve scale factor.
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
Plot2d_SetupCurveScaleDlg::Plot2d_SetupCurveScaleDlg( /*curveList lst, */QWidget* parent )
: QDialog( parent )
{
  setModal( true );
  setWindowTitle( tr("TLT_SETUP_CURVE_SCALE") );
  setSizeGripEnabled( true );

  /************************************************************************/
  QGroupBox* GroupC1 = new QGroupBox( this );
  QHBoxLayout* GroupC1Layout = new QHBoxLayout( GroupC1 );
  GroupC1Layout->setSpacing( SPACING_SIZE );
  GroupC1Layout->setMargin( MARGIN_SIZE );

  QLabel* aScaleLab = new QLabel( tr( "CURVE_SCALE_FACTOR" ), GroupC1 );
  myValueSpin = new QtxDoubleSpinBox( GroupC1 );
  myValueSpin->setMinimum( 0.01 );
  myValueSpin->setSingleStep( 0.1 );
  myValueSpin->setMinimumWidth( MIN_SPIN_WIDTH );

  GroupC1Layout->addWidget( aScaleLab );
  GroupC1Layout->addWidget( myValueSpin );

  /************************************************************************/
  QGroupBox* GroupButtons = new QGroupBox( this );
  QHBoxLayout* GroupButtonsLayout = new QHBoxLayout( GroupButtons );
  GroupButtonsLayout->setSpacing( SPACING_SIZE );
  GroupButtonsLayout->setMargin( MARGIN_SIZE );

  myOkBtn     = new QPushButton( tr( "BUT_OK" ),     this );
  myCancelBtn = new QPushButton( tr( "BUT_CANCEL" ), this );

  GroupButtonsLayout->addWidget( myOkBtn );
  GroupButtonsLayout->addSpacing( 10 );
  GroupButtonsLayout->addWidget( myCancelBtn );

  /************************************************************************/
  QVBoxLayout* topLayout = new QVBoxLayout( this );
  topLayout->setSpacing( SPACING_SIZE );
  topLayout->setMargin( MARGIN_SIZE );
  topLayout->addWidget( GroupC1 );
  topLayout->addWidget( GroupButtons );
 
  // default settings
  setScale( 1.0 );   // no scale

  // connections
  connect( myOkBtn,       SIGNAL( clicked() ),           this, SLOT( accept() ) );
  connect( myCancelBtn,   SIGNAL( clicked() ),           this, SLOT( reject() ) );

  SUIT_Tools::centerWidget( this, parent );
}

/*!
  \brief Destructor.
*/
Plot2d_SetupCurveScaleDlg::~Plot2d_SetupCurveScaleDlg()
{
}

/*!
  \brief Set scale factor.
  \param coef scale factor
  \sa getScale()
*/
void Plot2d_SetupCurveScaleDlg::setScale( const double coef )
{
  if ( coef > myValueSpin->maximum() ){
    myValueSpin->setMaximum( coef );
  }
  myValueSpin->setValue( coef );
}

/*!
  \brief Get scale factor.
  \return chosen scale factor
  \sa setScale()
*/
double Plot2d_SetupCurveScaleDlg::getScale() const
{
  return myValueSpin->value();
}
/*!
  \brief Clear value in the "Scale factor" spinbox.
*/
void Plot2d_SetupCurveScaleDlg::setUndefinedValue() {
  myValueSpin->setCleared(true);
  myValueSpin->setSpecialValueText("");
}

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
// File   : QtxIntSpinSlider.cxx
// Author : Maxim GLIBIN, OpenCASCADE S.A.S. (maxim.glibin@opencascade.com)

#include "QtxIntSpinSlider.h"

/*!
 * CONSTRUCTOR of spin box
 */
QtxIntSpinSlider::QtxIntSpinSlider( QWidget* theParent ) : QtxSlider( theParent )
{
  // Get default value of slider
  int aMin = mySlider->minimum();
  int aMax = mySlider->maximum();
  int aStep = mySlider->singleStep();
  
  // Create spin box
  myIntSpinBox = new QtxIntSpinBox( aMin, aMax, aStep, theParent );
  myIntSpinBox->setFixedWidth( 100 );

  // Add widget in main layout
  mainLayout->addWidget( myIntSpinBox );

  // Signals and slots connections
  connect( myIntSpinBox, SIGNAL(valueChanged( int )), this, SLOT(IntSpinHasChanged( int )) );
  connect( this, SIGNAL(valueUpdated( int )), this, SLOT(setValue( int )) );
}

QtxIntSpinSlider::QtxIntSpinSlider( int theMin, int theMax, int theStep, QWidget* theParent )
{
  // Set value of slider
  mySlider->setRange( theMin, theMax );
  mySlider->setSingleStep( theStep );
  
  // Create spin box
  myIntSpinBox = new QtxIntSpinBox( theMin, theMax, theStep, theParent );
  myIntSpinBox->setFixedWidth( 100 );

  // Add widgets in main layout
  mainLayout->addWidget( myIntSpinBox );

  // Signals and slots connections
  connect( myIntSpinBox, SIGNAL(valueChanged( int )), this, SLOT(IntSpinHasChanged( int )) );
  connect( this, SIGNAL(valueUpdated( int )), this, SLOT(setValue( int )) );
}

/*!
 * DESTRUCTOR of spin box
 */
QtxIntSpinSlider::~QtxIntSpinSlider()
{
  // Empty
}

/*!
  SLOT: Called when the value of spin box change
*/
void QtxIntSpinSlider::IntSpinHasChanged( int theValue )
{
  mySlider->blockSignals( true );
  mySlider->setValue( theValue );
  mySlider->blockSignals( false );
  emit valueChanged( theValue );
}

/*!
 * Get font of spin box
*/
QFont QtxIntSpinSlider::font()
{
  return myIntSpinBox->font();
}

/*!
 * Set font of spin box
*/
void QtxIntSpinSlider::setFont( QFont& theFont )
{
  myIntSpinBox->setFont( theFont );
}

/*!
 * Get value of spin box
*/
int QtxIntSpinSlider::value()
{
  return myIntSpinBox->value();
}

/*!
 * Set value of spin box
*/
void QtxIntSpinSlider::setValue( int theValue )
{
  myIntSpinBox->setValue( theValue );
}

void QtxIntSpinSlider::setUnit( QString& theUnit )
{
  myIntSpinBox->setSuffix( theUnit );
}

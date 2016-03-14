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
// File   : QtxDoubleSpinSlider.cxx
// Author : Maxim GLIBIN, OpenCASCADE S.A.S. (maxim.glibin@opencascade.com)

#include "QtxDoubleSpinSlider.h"

/*!
 * CONSTRUCTOR of double spin box
 */
QtxDoubleSpinSlider::QtxDoubleSpinSlider( QWidget* theParent ) : QtxSlider( theParent )
{
  // Get default value of slider
  double aMin = mySlider->minimum() / 100.;
  double aMax = mySlider->maximum() / 100.;
  double aStep = mySlider->singleStep() / 100.;
  
  // Create double spin box
  myDoubleSpinBox = new QtxDoubleSpinBox( aMin, aMax, aStep, theParent );
  myDoubleSpinBox->setFixedWidth( 100 );

  // Add widget in main layout
  mainLayout->addWidget( myDoubleSpinBox );

  // Signals and slots connections
  connect( myDoubleSpinBox, SIGNAL(valueChanged( double )), this, SLOT(DoubleSpinHasChanged( double )) );
  connect( this, SIGNAL(valueUpdated( int )), this, SLOT(setValue( int )) );
}

QtxDoubleSpinSlider::QtxDoubleSpinSlider( double theMin, double theMax, double theStep, QWidget* theParent ) : QtxSlider( theParent )
{
  // Set value of slider
  mySlider->setRange( int(theMin*100), int(theMax*100) );
  mySlider->setSingleStep( int(theStep*100) );
  
  // Create double spin box
  myDoubleSpinBox = new QtxDoubleSpinBox( theMin, theMax, theStep, theParent );
  myDoubleSpinBox->setFixedWidth( 100 );

  // Add widget in main layout
  mainLayout->addWidget( myDoubleSpinBox );

  // Signals and slots connections
  connect( myDoubleSpinBox, SIGNAL(valueChanged( double )), this, SLOT(DoubleSpinHasChanged( double )) );
  connect( this, SIGNAL(valueUpdated( int )), this, SLOT(setValue( int )) );
}

/*!
 * DESTRUCTOR of double spin box
 */
QtxDoubleSpinSlider::~QtxDoubleSpinSlider()
{
  // Empty
}

/*!
  SLOT: Called when the value of double spin box
*/
void QtxDoubleSpinSlider::DoubleSpinHasChanged( double theValue )
{
  
  int aNewValue = int( theValue * 100 );
  mySlider->blockSignals( true );
  mySlider->setValue( aNewValue );
  mySlider->blockSignals( false );
  emit valueChanged( theValue );
}

/*!
 * Get font currently set for the double spin box
*/
QFont QtxDoubleSpinSlider::font()
{
  return myDoubleSpinBox->font();
}

/*!
 * Set font currently set for the double spin box
*/
void QtxDoubleSpinSlider::setFont( QFont& theFont )
{
  myDoubleSpinBox->setFont( theFont );
}

/*!
 * Get value in double spin box
*/
double QtxDoubleSpinSlider::value()
{
  return myDoubleSpinBox->value();
}

/*!
 * Set value in double spin box
*/
void QtxDoubleSpinSlider::setValue( double theValue )
{
  myDoubleSpinBox->setValue( theValue );
}

/*!
 * Set value in double spin box
*/
void QtxDoubleSpinSlider::setValue( int theValue )
{
  double aNewValue = theValue / 100.;
  this->setValue( aNewValue );
}

/*!
 * Set precision of double spin box
*/
void QtxDoubleSpinSlider::setPrecision( int thePrecision )
{
  myDoubleSpinBox->setPrecision( thePrecision );
}

/*!
 * Get precision of double spin box
*/
int QtxDoubleSpinSlider::precision()
{
 return myDoubleSpinBox->getPrecision();
}

void QtxDoubleSpinSlider::setUnit( QString& theUnit )
{
  myDoubleSpinBox->setSuffix( theUnit );
}

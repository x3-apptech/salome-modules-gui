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
// File   : QtxSlider.cxx
// Author : Maxim GLIBIN, OpenCASCADE S.A.S. (maxim.glibin@opencascade.com)

#include "QtxSlider.h"

/*!
 * CONSTRUCTOR of slider
 */
QtxSlider::QtxSlider( QWidget* theParent ) : QWidget( theParent )
{
  // Create slider
  mySlider = new QSlider( Qt::Horizontal, theParent );
  mySlider->setFocusPolicy( Qt::NoFocus );
  mySlider->setMinimumSize( 250, 0 );
  mySlider->setTracking( false );

  // Add widgets in main layout
  mainLayout = new QHBoxLayout();
  mainLayout->addWidget( mySlider );
  mainLayout->setContentsMargins( 0, 0, 0, 0 );
  this->setLayout( mainLayout );

  // Signals and slots connections
  connect( mySlider, SIGNAL(sliderMoved( int )), this, SLOT(SliderHasMoved( int )) );
  connect( mySlider, SIGNAL(valueChanged( int )), this, SLOT(SliderHasMoved( int )) );
}

QtxSlider::QtxSlider( int theMin, int theMax, int theStep, QWidget* theParent ) : QWidget( theParent )
{
  // Create slider
  mySlider = new QSlider( Qt::Horizontal, theParent );
  mySlider->setFocusPolicy( Qt::NoFocus );
  mySlider->setMinimumSize( 200, 0 );
  mySlider->setRange( theMin, theMax );
  mySlider->setSingleStep( theStep );
  mySlider->setPageStep( 10 );
  mySlider->setTracking( false );

  // Add widgets in main layout
  mainLayout = new QHBoxLayout();
  mainLayout->addWidget( mySlider );
  mainLayout->setContentsMargins( 0, 0, 0, 0 );
  this->setLayout( mainLayout );

  // Signals and slots connections
  connect( mySlider, SIGNAL(sliderMoved( int )), this, SLOT(SliderHasMoved( int )) );
  connect( mySlider, SIGNAL(valueChanged( int )), this, SLOT(SliderHasMoved( int )) );
}

/*!
 * DESTRUCTOR of slider
 */
QtxSlider::~QtxSlider()
{
  // Empty
}

/*!
  SLOT: Called when the value of slider change
*/
void QtxSlider::SliderHasMoved( int theValue )
{
  emit valueUpdated( theValue );
}

/*!
 * Get value of slider
*/
int QtxSlider::value()
{
  return mySlider->value();
}

/*!
 * Set value of slider
*/
void QtxSlider::setValue( int theValue )
{
  mySlider->setValue( theValue );
}

/*!
 * Set range of slider
*/
void QtxSlider::setRange( int theMin, int theMax )
{
  mySlider->setRange( theMin, theMax );
}

/*!
 * Set single step of slider
*/
void QtxSlider::setSingleStep( int theStep )
{
  mySlider->setSingleStep( theStep );
}

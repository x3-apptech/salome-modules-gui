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

#include "SVTK_PsOptionsDlg.h"

#include <QtxDoubleSpinBox.h>

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QSlider>
#include <QPushButton>

/*!
 * Constructor
 */
SVTK_PsOptionsDlg::SVTK_PsOptionsDlg( QWidget* theParent):
  QDialog( theParent )
{
  setWindowTitle( tr( "DLG_PSOPTIONS_TITLE" ) );
  QVBoxLayout* aTopLayout = new QVBoxLayout( this );
  aTopLayout->setSpacing( 6 );
  aTopLayout->setMargin( 6 );
  
  // 1. Options
  QGroupBox* anOptionsBox = new QGroupBox( tr( "PS_OPTIONS" ), this );
  QGridLayout* anOptionsLayout = new QGridLayout( anOptionsBox );
  anOptionsLayout->setSpacing( 6 );
  anOptionsLayout->setMargin( 11 );

  // 1.1 Line width factor
  QLabel* lineLabel = new QLabel( tr( "PS_LINE_WIDTH" ), anOptionsBox);
  myLineFactor = new QtxDoubleSpinBox(0.0, 10.0, 0.1, anOptionsBox);
  myLineFactor->setValue(0.714);
  
  // 1.2 Point size factor
  QLabel* pointLabel = new QLabel( tr( "PS_POINT_SIZE" ), anOptionsBox);
  myPointFactor = new QtxDoubleSpinBox(0.0, 10.0, 0.1, anOptionsBox);
  myPointFactor->setValue(0.714);

  // 1.3 Sort method ("No sorting (fastest, poor)", "Simple sorting (fast, good)" and "BSP sorting (slow, best)"
  QLabel* sortLabel = new QLabel( tr( "PS_SORT_METHOD" ), anOptionsBox);
  mySortType = new QComboBox( anOptionsBox );
  mySortType->addItem( tr( "PS_NO_SORTING" ) );
  mySortType->addItem( tr( "PS_SIMPLE_SORTING" ) );
  mySortType->addItem( tr( "PS_BPS_SORTING" ) );
  
  // 1.4 Rasterize 3D geometry
  myRasterize3D = new QCheckBox( tr( "PS_RASTERIZE_3D" ), anOptionsBox );
  
  // 1.5 Use shfill shading operator
  myPs3Shading = new QCheckBox( tr( "PS_USE_SHFILL" ), anOptionsBox );

  // Add widgets to layout
  anOptionsLayout->addWidget( lineLabel, 0, 0 );
  anOptionsLayout->addWidget( myLineFactor, 0, 1 );
  anOptionsLayout->addWidget( pointLabel, 1, 0 );
  anOptionsLayout->addWidget( myPointFactor, 1, 1 );
  anOptionsLayout->addWidget( sortLabel, 2, 0 );
  anOptionsLayout->addWidget( mySortType, 2, 1 );
  anOptionsLayout->addWidget( myRasterize3D, 3, 0, 1, 2 );
  anOptionsLayout->addWidget( myPs3Shading, 4, 0, 1, 2 );

  
  QGroupBox* aButtonBox = new QGroupBox(theParent);

  QPushButton* okBtn    = new QPushButton(tr("BUT_OK"), aButtonBox);
  QPushButton* closeBtn = new QPushButton(tr("BUT_CLOSE"), aButtonBox);

  QSpacerItem* aSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

  QHBoxLayout* aLay = new QHBoxLayout(aButtonBox);
  aLay->setMargin(6);
  aLay->setSpacing(11);

  aLay->addWidget(okBtn);
  aLay->addItem(aSpacer);
  aLay->addWidget(closeBtn);

  connect(okBtn,    SIGNAL(clicked()), SLOT(accept()));
  connect(closeBtn, SIGNAL(clicked()), SLOT(reject()));
  
  aTopLayout->addWidget( anOptionsBox );
  aTopLayout->addWidget( aButtonBox );
}

/*!
 * Destructor
 */  
SVTK_PsOptionsDlg::~SVTK_PsOptionsDlg()
{
}

/*!
 * Return line scale factor
 */  
double SVTK_PsOptionsDlg::getLineFactor() const {
  return myLineFactor->value();
}

/*!
 * Return point scale factor
 */  
double SVTK_PsOptionsDlg::getPointFactor() const {
  return myPointFactor->value();
}

/*!
 * Return sort type
 */  
int SVTK_PsOptionsDlg::getSortType() const {
  return mySortType->currentIndex();
}

/*!
 * Return rasterize 3D flag
 */  
bool SVTK_PsOptionsDlg::isRasterize3D() const {
  return myRasterize3D->isChecked();
}

/*!
 * Return Ps3Shading flag
 */  
bool SVTK_PsOptionsDlg::isPs3Shading() const {
  return myPs3Shading->isChecked();
}

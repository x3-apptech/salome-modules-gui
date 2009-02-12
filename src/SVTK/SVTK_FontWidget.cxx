//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  VISU VISUGUI : GUI for SMESH component
//  File   : 
//  Author : Sergey LITONIN
//  Module : 
//
#include "SVTK_FontWidget.h"

#include <QToolButton>
#include <QComboBox>
#include <QColorDialog>
#include <QCheckBox>
#include <QHBoxLayout>

#include <vtkTextProperty.h>

/*!
 * Class       : SVTK_FontWidget
 * Description : Dialog for specifynig font
 */

/*!
  Constructor
*/
SVTK_FontWidget::SVTK_FontWidget( QWidget* theParent )
: QWidget( theParent )
{
  myColorBtn = new QToolButton( this );
  myColorBtn->setMinimumWidth( 20 );

  myFamily = new QComboBox( this );
  myFamily->insertItem( myFamily->count(), tr( "ARIAL" ) );
  myFamily->insertItem( myFamily->count(), tr( "COURIER" ) );
  myFamily->insertItem( myFamily->count(), tr( "TIMES" ) );

  myBold = new QCheckBox( tr( "BOLD" ), this );
  myItalic = new QCheckBox( tr( "ITALIC" ), this );
  myShadow = new QCheckBox( tr( "SHADOW" ), this );

  QHBoxLayout* aHBLayout = new QHBoxLayout;
  aHBLayout->setSpacing( 5 );
  aHBLayout->addWidget(myColorBtn);
  aHBLayout->addWidget(myFamily);
  aHBLayout->addWidget(myBold);
  aHBLayout->addWidget(myItalic);
  aHBLayout->addWidget(myShadow);
  this->setLayout(aHBLayout);

  connect( myColorBtn, SIGNAL( clicked() ), SLOT( onColor() ) );
}

/*!
  Destructor
*/
SVTK_FontWidget::~SVTK_FontWidget()
{
}

void SVTK_FontWidget::SetColor( const QColor& theColor )
{
  QPalette palette;
  palette.setColor(myColorBtn->backgroundRole(), theColor);
  myColorBtn->setPalette(palette);
}

QColor SVTK_FontWidget::GetColor() const
{
  return myColorBtn->palette().color( myColorBtn->backgroundRole() );
}

void SVTK_FontWidget::onColor()
{
  QColor aColor = QColorDialog::getColor( GetColor(), this );
  if ( aColor.isValid() )
    SetColor( aColor );
}

void SVTK_FontWidget::SetData( const QColor& theColor,
                              const int theFamily,
                              const bool theBold,
                              const bool theItalic,
                              const bool theShadow )
{
  SetColor( theColor );

  if ( theFamily == VTK_ARIAL )
    myFamily->setCurrentIndex( 0 );
  else if ( theFamily == VTK_COURIER )
    myFamily->setCurrentIndex( 1 );
  else
    myFamily->setCurrentIndex( 2 );

  myBold->setChecked( theBold );
  myItalic->setChecked( theItalic );
  myShadow->setChecked( theShadow );
}

void SVTK_FontWidget::GetData( QColor& theColor,
			       int& theFamily,
			       bool& theBold,
			       bool& theItalic,
			       bool& theShadow ) const
{
  theColor = GetColor();

  int anItem =myFamily->currentIndex();
  if ( anItem == 0 )
    theFamily = VTK_ARIAL;
  else if ( anItem == 1 )
    theFamily = VTK_COURIER;
  else
    theFamily = VTK_TIMES;

  theBold = myBold->isChecked();
  theItalic = myItalic->isChecked();
  theShadow = myShadow->isChecked();
}

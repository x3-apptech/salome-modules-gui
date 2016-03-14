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

#include "ViewerTools_FontWidgetBase.h"

#include <QToolButton>
#include <QComboBox>
#include <QColorDialog>
#include <QCheckBox>
#include <QHBoxLayout>

/*!
 * Class       : ViewerTools_FontWidgetBase
 * Description : Dialog for specifynig font
 */

/*!
  Constructor
*/
ViewerTools_FontWidgetBase::ViewerTools_FontWidgetBase( QWidget* theParent )
: QWidget( theParent )
{
}

/*!
  Destructor
*/
ViewerTools_FontWidgetBase::~ViewerTools_FontWidgetBase()
{
}

void ViewerTools_FontWidgetBase::Initialize()
{
  myColorBtn = new QToolButton( this );
  myColorBtn->setMinimumWidth( 20 );

  myFamily = new QComboBox( this );
  InitializeFamilies();

  myBold = new QCheckBox( tr( "BOLD" ), this );
  myItalic = new QCheckBox( tr( "ITALIC" ), this );
  myShadow = new QCheckBox( tr( "SHADOW" ), this );

  QHBoxLayout* aHBLayout = new QHBoxLayout;
  aHBLayout->setMargin( 0 );
  aHBLayout->setSpacing( 5 );
  aHBLayout->addWidget(myColorBtn);
  aHBLayout->addWidget(myFamily);
  aHBLayout->addWidget(myBold);
  aHBLayout->addWidget(myItalic);
  aHBLayout->addWidget(myShadow);
  aHBLayout->addStretch();
  this->setLayout(aHBLayout);

  connect( myColorBtn, SIGNAL( clicked() ), SLOT( onColor() ) );

  if( myFamily->count() == 0 )
  {
    myFamily->hide();
    myBold->hide();
    myItalic->hide();
    myShadow->hide();
  }
}

void ViewerTools_FontWidgetBase::SetColor( const QColor& theColor )
{
  QPalette palette;
  palette.setColor(myColorBtn->backgroundRole(), theColor);
  myColorBtn->setPalette(palette);
}

QColor ViewerTools_FontWidgetBase::GetColor() const
{
  return myColorBtn->palette().color( myColorBtn->backgroundRole() );
}

void ViewerTools_FontWidgetBase::onColor()
{
  QColor aColor = QColorDialog::getColor( GetColor(), this );
  if ( aColor.isValid() )
    SetColor( aColor );
}

void ViewerTools_FontWidgetBase::SetData( const QColor& theColor,
                                          const int theFamily,
                                          const bool theBold,
                                          const bool theItalic,
                                          const bool theShadow )
{
  SetColor( theColor );

  myBold->setChecked( theBold );
  myItalic->setChecked( theItalic );
  myShadow->setChecked( theShadow );
}

void ViewerTools_FontWidgetBase::GetData( QColor& theColor,
                                          int& theFamily,
                                          bool& theBold,
                                          bool& theItalic,
                                          bool& theShadow ) const
{
  theColor = GetColor();

  theBold = myBold->isChecked();
  theItalic = myItalic->isChecked();
  theShadow = myShadow->isChecked();
}

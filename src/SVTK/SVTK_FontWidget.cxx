//  VISU VISUGUI : GUI for SMESH component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : 
//  Author : Sergey LITONIN
//  Module : 

#include "SVTK_FontWidget.h"

#include <qtoolbutton.h>
#include <qcombobox.h>
#include <qcolordialog.h>
#include <qcheckbox.h>

#include <vtkTextProperty.h>

/*!
 * Class       : SVTK_FontWidget
 * Description : Dialog for specifynig font
 */

/*!
  Constructor
*/
SVTK_FontWidget::SVTK_FontWidget( QWidget* theParent )
: QHBox( theParent )
{
  setSpacing( 5 );
  myColorBtn = new QToolButton( this );
  myColorBtn->setMinimumWidth( 20 );

  myFamily = new QComboBox( this );
  myFamily->insertItem( tr( "ARIAL" ) );
  myFamily->insertItem( tr( "COURIER" ) );
  myFamily->insertItem( tr( "TIMES" ) );

  myBold = new QCheckBox( tr( "BOLD" ), this );
  myItalic = new QCheckBox( tr( "ITALIC" ), this );
  myShadow = new QCheckBox( tr( "SHADOW" ), this );

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
  myColorBtn->setPaletteBackgroundColor( theColor );
}

QColor SVTK_FontWidget::GetColor() const
{
  return myColorBtn->paletteBackgroundColor();
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
    myFamily->setCurrentItem( 0 );
  else if ( theFamily == VTK_COURIER )
    myFamily->setCurrentItem( 1 );
  else
    myFamily->setCurrentItem( 2 );

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

  int anItem =myFamily->currentItem();
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
